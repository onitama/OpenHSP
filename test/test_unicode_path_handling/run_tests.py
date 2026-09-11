#!/usr/bin/env python3
"""Compare HSP compiler path/source encoding support and retain failure evidence."""

from __future__ import annotations

import argparse
from dataclasses import asdict, dataclass
from datetime import datetime
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
from typing import Dict, Iterable, List, Optional, Sequence, Tuple

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CHARACTERS = {
    "ascii": "ascii_A1",
    "ascii_space": "space ascii_A1",
    "cp932": "日本語_表ソ能",
    "cp932_space": "空白 日本語_表ソ能",
    "bmp": "BMP_☃",
    "supplementary": "EXT_😀",
}


@dataclass(frozen=True)
class Target:
    name: str
    hspcmp: Path
    common: Path
    dll: Optional[Path]


@dataclass
class Result:
    target: str
    case_id: str
    interface: str
    character_class: str
    source_encoding: str
    path_locus: str
    status: str
    detail: str
    artifact_dir: str
    command: List[str]
    exit_code: Optional[int]


def representable_cp932(text: str) -> bool:
    try:
        text.encode("cp932")
        return True
    except UnicodeEncodeError:
        return False


def common_path_argument(path: Path) -> str:
    """Return the directory-prefix form required by legacy hspcmp 3.7."""
    value = str(path)
    if value.endswith(("\\", "/")):
        return value
    return value + ("\\" if os.name == "nt" else "/")


def pe_bits(path: Path) -> Optional[int]:
    with path.open("rb") as stream:
        if stream.read(2) != b"MZ":
            return None
        stream.seek(0x3C)
        offset = struct.unpack("<I", stream.read(4))[0]
        stream.seek(offset + 4)
        machine = struct.unpack("<H", stream.read(2))[0]
    if machine == 0x14C:
        return 32
    if machine == 0x8664:
        return 64
    raise RuntimeError(f"unsupported PE machine 0x{machine:04x}: {path}")


def file_hash(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def run(command: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(list(command), cwd=str(cwd), stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT, check=False)


def decoded_log(data: bytes) -> str:
    parts = []
    for encoding in ("utf-8", "cp932"):
        parts += [f"--- decoded as {encoding} ---\n",
                  data.decode(encoding, errors="replace")]
        if not parts[-1].endswith("\n"):
            parts.append("\n")
    return "".join(parts)


def write_fixture(path: Path, text: str, encoding: str, bom: bool = False) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    data = text.encode(encoding)
    path.write_bytes((b"\xef\xbb\xbf" if bom else b"") + data)


def nested_path(root: Path, target_length: int, filename: str) -> Path:
    result = root / filename
    while len(os.fsencode(str(result))) < target_length:
        remaining = target_length - len(os.fsencode(str(result)))
        component_length = min(100, remaining - 1)
        if component_length < 1:
            break
        result = result.parent / ("A" * component_length) / result.name
    return result


def find_msbuild() -> Path:
    vswhere = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
    vswhere /= "Microsoft Visual Studio/Installer/vswhere.exe"
    if not vswhere.is_file():
        raise RuntimeError("vswhere.exe was not found")
    completed = subprocess.run(
        [str(vswhere), "-latest", "-products", "*", "-requires",
         "Microsoft.Component.MSBuild", "-find", r"MSBuild\**\Bin\MSBuild.exe"],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=False)
    paths = [Path(line.strip()) for line in completed.stdout.splitlines() if line.strip()]
    if not paths:
        raise RuntimeError("Visual Studio MSBuild was not found")
    return paths[0]


def build_workers() -> Dict[int, Path]:
    msbuild = find_msbuild()
    project = HERE / "dll_worker.vcxproj"
    for platform in ("Win32", "x64"):
        completed = subprocess.run(
            [str(msbuild), str(project), "/m", "/nologo", "/v:minimal",
             "/p:Configuration=Release", f"/p:Platform={platform}"],
            cwd=str(HERE), check=False)
        if completed.returncode:
            raise RuntimeError(f"failed to build DLL worker for {platform}")
    workers = {
        32: ROOT / "artifacts/unicode_path/tools/Win32/dll_worker.exe",
        64: ROOT / "artifacts/unicode_path/tools/x64/dll_worker.exe",
    }
    for bits, worker in workers.items():
        if not worker.is_file() or pe_bits(worker) != bits:
            raise RuntimeError(f"invalid {bits}-bit worker: {worker}")
    return workers


class TargetSuite:
    def __init__(self, target: Target, run_dir: Path, workers: Dict[int, Path]):
        self.target = target
        self.run_dir = run_dir
        self.workers = workers
        self.results: List[Result] = []
        self.work_root = Path(tempfile.mkdtemp(prefix=f"hspu-{target.name}-"))

    def case_work(self, case_id: str) -> Path:
        short_id = hashlib.sha1(case_id.encode("utf-8")).hexdigest()[:10]
        return self.work_root / short_id

    def save(self, result: Result) -> None:
        directory = self.run_dir / result.artifact_dir
        directory.mkdir(parents=True, exist_ok=True)
        (directory / "result.json").write_text(
            json.dumps(asdict(result), ensure_ascii=False, indent=2), encoding="utf-8")
        self.results.append(result)

    def unsupported(self, case_id: str, interface: str, character_class: str,
                    encoding: str, locus: str, status: str, detail: str) -> None:
        relative = Path(self.target.name) / case_id
        self.save(Result(self.target.name, case_id, interface, character_class,
                         encoding, locus, status, detail, str(relative), [], None))

    def execute(self, case_id: str, interface: str, character_class: str,
                encoding: str, locus: str, command: Sequence[str], cwd: Path,
                checks: Iterable[Tuple[Path, Optional[bytes], str]], work: Path,
                expected_failure: bool = False,
                expected_status: str = "EXPECTED_REJECTION",
                accepted_status: str = "FAIL",
                retain_work: bool = True) -> None:
        relative = Path(self.target.name) / case_id
        directory = self.run_dir / relative
        directory.mkdir(parents=True, exist_ok=True)
        command = [str(item) for item in command]
        (directory / "command.json").write_text(
            json.dumps({"command": command, "cwd": str(cwd)}, ensure_ascii=False,
                       indent=2), encoding="utf-8")
        status, detail, exit_code = "FAIL", "not run", None
        try:
            completed = run(command, cwd)
            exit_code = completed.returncode
            (directory / "output.bin").write_bytes(completed.stdout)
            (directory / "output.txt").write_text(decoded_log(completed.stdout),
                                                    encoding="utf-8")
            if expected_failure and exit_code:
                if exit_code == 0xFFFFFFFF:
                    status, detail = expected_status, "rejected safely with -1"
                elif exit_code == 128 or exit_code >= 0xC0000000 or b"WORKER_CRASH" in completed.stdout:
                    status, detail = "CRASH", f"unsafe exception exit {exit_code}"
                else:
                    status, detail = expected_status, f"rejected safely with {exit_code}"
            elif expected_failure:
                status = accepted_status
                detail = "invalid input was accepted"
            elif exit_code:
                if exit_code == 0xFFFFFFFF:
                    detail = "process exited with -1"
                elif exit_code == 128 or exit_code >= 0xC0000000 or b"WORKER_CRASH" in completed.stdout:
                    status, detail = "CRASH", f"unsafe exception exit {exit_code}"
                else:
                    detail = f"process exited with {exit_code}"
            else:
                problems = []
                for path, marker, description in checks:
                    if not path.is_file():
                        problems.append(f"{description}: missing {path.name}")
                    elif marker is not None and marker not in path.read_bytes():
                        problems.append(f"{description}: marker missing")
                if problems:
                    detail = "; ".join(problems)
                else:
                    status, detail = "PASS", "all checks passed"
        except Exception as error:
            status, detail = "INFRA_ERROR", f"{type(error).__name__}: {error}"
        result = Result(self.target.name, case_id, interface, character_class,
                        encoding, locus, status, detail, str(relative), command, exit_code)
        self.save(result)
        if status != "PASS" and retain_work and work.exists():
            shutil.copytree(work, directory / "work", dirs_exist_ok=True)
        if work.exists():
            shutil.rmtree(work)

    def compile_case(self, interface: str, character_class: str,
                     encoding: str, locus: str, bom: bool = False) -> None:
        token = CHARACTERS[character_class]
        case_id = f"{interface}-compile-{encoding}-{locus}-{character_class}"
        if interface == "dll" and self.target.dll is None:
            self.unsupported(case_id, interface, character_class, encoding, locus,
                             "SKIPPED", "hspcmp DLL is not configured")
            return
        if os.name != "nt" and encoding == "cp932":
            self.unsupported(case_id, interface, character_class, encoding, locus,
                             "SKIPPED", "CP932 source input is Windows-only")
            return
        work = self.case_work(case_id)
        source_dir = work / (token if locus == "main" else "source")
        source_name = token + ".hsp" if locus == "main" else "main.hsp"
        include_name = token + ".as" if locus == "include" else "include.as"
        output_dir = work / (token if locus == "output" else "output")
        output_name = token + ".ax" if locus == "output" else "result.ax"
        common_dir = work / token if locus == "common" else self.target.common
        is_cp932 = encoding == "cp932"

        if is_cp932 and locus == "include" and not representable_cp932(include_name):
            self.unsupported(case_id, interface, character_class, encoding, locus,
                             "UNSUPPORTED_BY_ENCODING",
                             "include name cannot be represented in a CP932 source")
            return
        if (interface == "dll" and locus in ("main", "output", "common")
                and not representable_cp932(token)):
            self.unsupported(case_id, interface, character_class, encoding, locus,
                             "UNSUPPORTED_BY_API",
                             "legacy DLL path argument cannot represent this name in CP932")
            return

        source_dir.mkdir(parents=True, exist_ok=True)
        output_dir.mkdir(parents=True, exist_ok=True)
        if locus == "common":
            common_dir.mkdir(parents=True, exist_ok=True)
        marker_token = token if (not is_cp932 or representable_cp932(token)) else "CP932_表"
        marker = f"MARKER_{interface}_{marker_token}"
        codec = "cp932" if is_cp932 else "utf-8"
        write_fixture(source_dir / include_name, f'mes "INCLUDE_{marker}"\n',
                      codec, bom)
        common_line = ""
        if locus == "common":
            write_fixture(common_dir / "hspdef.as",
                          f'#define global unicode_common_marker "COMMON_{marker}"\n',
                          codec, bom)
            common_line = "mes unicode_common_marker\n"
        write_fixture(source_dir / source_name,
                      f'#include "{include_name}"\n{common_line}'
                      f'mes "{marker}"\nmes __file__\n',
                      codec, bom)
        output = output_dir / output_name
        utf8_input = not is_cp932
        if interface == "cli":
            command = [str(self.target.hspcmp), "-d", "-u"]
            if utf8_input:
                command.append("-i")
            command += ["-o" + str(output),
                        "--compath=" + common_path_argument(common_dir),
                        str(source_dir / source_name)]
        else:
            worker = self.workers[pe_bits(self.target.dll)]
            command = [str(worker), "compile", str(self.target.dll),
                       str(source_dir / source_name),
                       common_path_argument(common_dir), str(output),
                       "unused", "1" if utf8_input else "0"]
        checks = [(output, marker.encode("utf-8"), "compiled marker"),
                  (output, ("INCLUDE_" + marker).encode("utf-8"),
                   "include marker")]
        if locus == "common":
            checks.append((output, ("COMMON_" + marker).encode("utf-8"),
                           "common-path marker"))
        self.execute(case_id, interface, character_class, encoding, locus,
                     command, source_dir, checks, work)

    def packopt_case(self, character_class: str) -> None:
        case_id = f"dll-packopt-utf8-{character_class}"
        if self.target.dll is None:
            self.unsupported(case_id, "dll", character_class, "utf8", "packopt",
                             "SKIPPED", "hspcmp DLL is not configured")
            return
        work = self.case_work(case_id)
        work.mkdir(parents=True, exist_ok=True)
        source, output = work / "packopt.hsp", work / "start.ax"
        value = "packopt_" + CHARACTERS[character_class]
        write_fixture(source, f'#packopt name "{value}"\nmes "PACKOPT_MARKER"\n',
                      "utf-8")
        worker = self.workers[pe_bits(self.target.dll)]
        command = [str(worker), "compile-pack", str(self.target.dll), str(source),
                   common_path_argument(self.target.common), str(output), "unused", "1"]
        self.execute(case_id, "dll", character_class, "utf8", "packopt",
                     command, work,
                     [(work / "packfile", f";!name={value}\r\n".encode("utf-8"),
                       "UTF-8 packopt value")], work)

    def pack_case(self, character_class: str, extract: bool) -> None:
        operation = "roundtrip" if extract else "create"
        case_id = f"dll-pack-{operation}-utf8-{character_class}"
        if self.target.dll is None:
            self.unsupported(case_id, "dll", character_class, "utf8", "pack",
                             "SKIPPED", "hspcmp DLL is not configured")
            return
        token = CHARACTERS[character_class]
        asset_name = token + ".txt"
        if extract and not representable_cp932(asset_name):
            self.unsupported(case_id, "dll", character_class, "utf8", "pack",
                             "UNSUPPORTED_BY_API",
                             "legacy pack_get path argument cannot represent this name in CP932")
            return
        work = self.case_work(case_id)
        work.mkdir(parents=True, exist_ok=True)
        source, output = work / "pack.hsp", work / "start.ax"
        asset = work / asset_name
        asset_contents = b"UNICODE_PACK_ASSET\n"
        asset.write_bytes(asset_contents)
        write_fixture(source, f'#pack "{asset_name}"\nmes "PACK_MARKER"\n', "utf-8")
        pack_base = work / "pack-result"
        worker = self.workers[pe_bits(self.target.dll)]
        command = [str(worker), "pack-flow", str(self.target.dll), str(source),
                   common_path_argument(self.target.common), str(output), "1", str(pack_base),
                   asset_name, "1" if extract else "0"]
        checks: List[Tuple[Path, Optional[bytes], str]] = [
            (pack_base.with_suffix(".dpm"), None, "DPM output")]
        if extract:
            checks.append((asset, asset_contents, "restored packed asset"))
        self.execute(case_id, "dll", character_class, "utf8", "pack",
                     command, work, checks, work)

    def negative_case(self, kind: str) -> None:
        case_id = f"cli-negative-{kind}"
        work = self.case_work(case_id)
        work.mkdir(parents=True, exist_ok=True)
        output = work / "unexpected.ax"
        if kind == "missing-source":
            source = work / "does-not-exist.hsp"
        else:
            source = work / "invalid-utf8.hsp"
            source.write_bytes(b'mes "INVALID_\xf0\x28\x8c\x28"\n')
        command = [str(self.target.hspcmp), "-d", "-i", "-u",
                   "-o" + str(output),
                   "--compath=" + common_path_argument(self.target.common),
                   str(source)]
        self.execute(case_id, "cli", "ascii", "utf8", kind, command, work,
                     [], work, expected_failure=True,
                     accepted_status="KNOWN_LIMITATION" if kind == "invalid-utf8" else "FAIL")

    def path_limit_case(self, interface: str, length: int) -> None:
        case_id = f"{interface}-known-limit-missing-source-{length}"
        work = self.case_work(case_id)
        work.mkdir(parents=True, exist_ok=True)
        source = nested_path(work, length, "missing.hsp")
        output = work / "unexpected.ax"
        if interface == "cli":
            command = [str(self.target.hspcmp), "-d", "-i", "-u",
                       "-o" + str(output),
                       "--compath=" + common_path_argument(self.target.common),
                       str(source)]
        else:
            if self.target.dll is None:
                self.unsupported(case_id, interface, "ascii", "utf8", "path-limit",
                                 "SKIPPED", "hspcmp DLL is not configured")
                return
            worker = self.workers[pe_bits(self.target.dll)]
            command = [str(worker), "compile", str(self.target.dll), str(source),
                       common_path_argument(self.target.common), str(output),
                       "unused", "1"]
        self.execute(case_id, interface, "ascii", "utf8", "path-limit",
                     command, work, [], work, expected_failure=True,
                     expected_status="KNOWN_LIMITATION")

    def multibyte_path_limit_case(self, interface: str) -> None:
        case_id = f"{interface}-known-limit-existing-multibyte-source"
        work = self.case_work(case_id)
        source_dir = work / ("日" * 70)
        source_dir.mkdir(parents=True, exist_ok=True)
        source = source_dir / "main.hsp"
        output = work / "result.ax"
        encoding = "cp932" if self.target.name == "baseline" else "utf-8"
        write_fixture(source, 'mes "MULTIBYTE_PATH_LIMIT"\n', encoding)
        if interface == "cli":
            command = [str(self.target.hspcmp), "-d", "-u"]
            if encoding == "utf-8":
                command.append("-i")
            command += ["-o" + str(output),
                        "--compath=" + common_path_argument(self.target.common),
                        str(source)]
        else:
            if self.target.dll is None:
                self.unsupported(case_id, interface, "cp932", encoding, "path-limit",
                                 "SKIPPED", "hspcmp DLL is not configured")
                return
            worker = self.workers[pe_bits(self.target.dll)]
            command = [str(worker), "compile", str(self.target.dll), str(source),
                       common_path_argument(self.target.common), str(output), "unused",
                       "1" if encoding == "utf-8" else "0"]
        self.execute(case_id, interface, "cp932", encoding, "path-limit",
                     command, source_dir,
                     [(output, b"MULTIBYTE_PATH_LIMIT", "compiled marker")], work)

    def nested_path_case(self) -> None:
        if os.name == "nt":
            return
        for length in (261, 512, 1024, 2048, 4090):
            case_id = f"cli-existing-nested-path-{length}"
            work = self.case_work(case_id)
            source = nested_path(work, length, "main.hsp")
            output = work / "result.ax"
            write_fixture(source, 'mes "NESTED_PATH_PASS"\n', "utf-8")
            command = [str(self.target.hspcmp), "-d", "-u", "-i",
                       "-o" + str(output),
                       "--compath=" + common_path_argument(self.target.common),
                       str(source)]
            self.execute(case_id, "cli", "ascii", "utf8", "path-limit",
                         command, source.parent, [(output, None, "compiled output")], work,
                         retain_work=False)

    def all_cases(self) -> List[Result]:
        for interface in ("cli", "dll"):
            for encoding in ("cp932", "utf8"):
                for locus in ("main", "include", "output", "common"):
                    for character_class in CHARACTERS:
                        self.compile_case(interface, character_class, encoding, locus)
            self.compile_case(interface, "ascii", "utf8-bom", "main", bom=True)
        for character_class in CHARACTERS:
            self.packopt_case(character_class)
            self.pack_case(character_class, extract=False)
            self.pack_case(character_class, extract=True)
        self.negative_case("missing-source")
        self.negative_case("invalid-utf8")
        for interface in ("cli", "dll"):
            for length in (261, 512, 1024, 4094, 4096):
                self.path_limit_case(interface, length)
            self.multibyte_path_limit_case(interface)
        self.nested_path_case()
        if self.work_root.exists():
            shutil.rmtree(self.work_root)
        return self.results


def metadata(target: Target) -> Dict[str, object]:
    result: Dict[str, object] = {"name": target.name, "common": str(target.common)}
    for label, path in (("hspcmp", target.hspcmp), ("dll", target.dll)):
        if path is None:
            result[label] = None
            continue
        stat = path.stat()
        item: Dict[str, object] = {
            "path": str(path), "bits": pe_bits(path), "size": stat.st_size,
            "mtime": datetime.fromtimestamp(stat.st_mtime).isoformat(),
            "sha256": file_hash(path),
        }
        if label == "hspcmp":
            item["banner"] = decoded_log(run([str(path)], path.parent).stdout)
        result[label] = item
    return result


def compare(baseline: List[Result], candidate: List[Result]) -> List[Dict[str, str]]:
    left = {item.case_id: item for item in baseline}
    right = {item.case_id: item for item in candidate}
    rows = []
    for case_id in sorted(set(left) | set(right)):
        bs = left[case_id].status if case_id in left else "MISSING"
        cs = right[case_id].status if case_id in right else "MISSING"
        if "INFRA_ERROR" in (bs, cs) or "MISSING" in (bs, cs):
            verdict = "INFRA_ERROR"
        elif bs == cs == "PASS":
            verdict = "SAME_PASS"
        elif bs == cs == "EXPECTED_REJECTION":
            verdict = "SAME_EXPECTED_REJECTION"
        elif bs == "PASS":
            verdict = "REGRESSION"
        elif cs == "PASS":
            verdict = "IMPROVEMENT"
        elif bs == "CRASH" and cs == "KNOWN_LIMITATION":
            verdict = "IMPROVEMENT"
        elif bs == "KNOWN_LIMITATION" and cs == "CRASH":
            verdict = "REGRESSION"
        elif cs == "CRASH" and bs != "CRASH":
            verdict = "REGRESSION"
        elif bs == cs:
            verdict = "SHARED_LIMITATION"
        elif cs.startswith("UNSUPPORTED"):
            verdict = "CANDIDATE_LIMITATION"
        else:
            verdict = "SHARED_LIMITATION"
        rows.append({"case_id": case_id, "baseline": bs,
                     "candidate": cs, "verdict": verdict})
    return rows


def write_summary(run_dir: Path, targets: List[Dict[str, object]],
                  results: List[Result], comparison: List[Dict[str, str]]) -> None:
    payload = {"generated_at": datetime.now().astimezone().isoformat(),
               "character_classes": CHARACTERS, "targets": targets,
               "results": [asdict(item) for item in results],
               "comparison": comparison}
    (run_dir / "summary.json").write_text(
        json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")
    counts: Dict[str, int] = {}
    for row in comparison:
        counts[row["verdict"]] = counts.get(row["verdict"], 0) + 1
    lines = ["# Unicode path/source comparison", "", "## Verdict counts", ""]
    lines += [f"- {name}: {count}" for name, count in sorted(counts.items())]
    lines += ["", "## Cases", "", "| Case | Baseline | Candidate | Verdict |",
              "|---|---:|---:|---|"]
    lines += [f"| `{row['case_id']}` | {row['baseline']} | {row['candidate']} | {row['verdict']} |"
              for row in comparison]
    (run_dir / "summary.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    for prefix in ("baseline", "candidate"):
        parser.add_argument(f"--{prefix}-hspcmp", type=Path)
        parser.add_argument(f"--{prefix}-hspcmp-dll", type=Path)
        parser.add_argument(f"--{prefix}-common", type=Path)
    parser.add_argument("--artifacts-dir", type=Path)
    parser.add_argument("--hspcmp", type=Path)
    parser.add_argument("--hspcmp-dll", type=Path)
    parser.add_argument("--common", type=Path, default=ROOT / "common")
    parser.add_argument("--pathio-exe", type=Path,
                        help="accepted for compatibility; run separately")
    return parser.parse_args()


def make_target(name: str, executable: Path, common: Path,
                dll: Optional[Path]) -> Target:
    target = Target(name, executable.resolve(), common.resolve(),
                    dll.resolve() if dll else None)
    if not target.hspcmp.is_file() or not target.common.is_dir():
        raise RuntimeError(f"invalid {name} compiler/common path")
    if target.dll and not target.dll.is_file():
        raise RuntimeError(f"invalid {name} DLL path: {target.dll}")
    return target


def main() -> int:
    args = arguments()
    run_dir = (args.artifacts_dir or ROOT / "artifacts/unicode_path/runs" /
               datetime.now().strftime("%Y%m%d-%H%M%S")).resolve()
    run_dir.mkdir(parents=True, exist_ok=True)
    try:
        if args.baseline_hspcmp or args.candidate_hspcmp:
            required = (args.baseline_hspcmp, args.baseline_common,
                        args.candidate_hspcmp, args.candidate_common)
            if any(value is None for value in required):
                raise RuntimeError("comparison mode needs both hspcmp and common paths")
            targets = [
                make_target("baseline", args.baseline_hspcmp, args.baseline_common,
                            args.baseline_hspcmp_dll),
                make_target("candidate", args.candidate_hspcmp, args.candidate_common,
                            args.candidate_hspcmp_dll),
            ]
        else:
            executable = args.hspcmp or (
                ROOT / "hspcmp" if os.name != "nt"
                else ROOT / "src/hspcmp/Release64/hspcmp.exe")
            targets = [make_target("candidate", executable, args.common,
                                   args.hspcmp_dll)]
        workers = build_workers() if any(target.dll for target in targets) else {}
        target_metadata = [metadata(target) for target in targets]
        grouped, all_results = [], []
        for target in targets:
            print(f"=== {target.name}: {target.hspcmp} ===")
            results = TargetSuite(target, run_dir, workers).all_cases()
            grouped.append(results)
            all_results += results
            for item in results:
                print(f"{item.status:24} {item.case_id}")
        comparison = compare(grouped[0], grouped[1]) if len(grouped) == 2 else []
        write_summary(run_dir, target_metadata, all_results, comparison)
        regressions = sum(row["verdict"] == "REGRESSION" for row in comparison)
        infra = any(item.status == "INFRA_ERROR" for item in all_results)
        print(f"Artifacts: {run_dir}")
        print(f"Regressions: {regressions}; infrastructure errors: {int(infra)}")
        return 1 if regressions or infra else 0
    except Exception as error:
        (run_dir / "fatal.txt").write_text(
            f"{type(error).__name__}: {error}\n", encoding="utf-8")
        print(f"FATAL: {error}\nArtifacts: {run_dir}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
