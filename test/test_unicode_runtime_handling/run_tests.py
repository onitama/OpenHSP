#!/usr/bin/env python3
"""Compare HSP 3.7 and development HSP3CL Unicode file/DPM behavior."""

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
from typing import Optional, Sequence


ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
PATH_SUITE = ROOT / "test/test_unicode_path_handling"
TOKENS = {
    "ascii": "ascii_A1",
    "cp932": "日本語_表ソ能",
    "bmp": "BMP_☃",
    "supplementary": "EXT_😀",
}
OPERATIONS = ("binary", "note", "exist", "copy", "delete", "mkdir-chdir", "dirlist")


@dataclass(frozen=True)
class Target:
    name: str
    runtime: Path
    compiler: Path
    compiler_dll: Optional[Path]
    common: Path
    source_encoding: str
    utf8_input: bool


@dataclass
class Result:
    target: str
    case_id: str
    operation: str
    character_class: str
    locus: str
    status: str
    stage: str
    detail: str
    artifact_dir: str
    compile_command: list[str]
    run_command: list[str]
    compile_exit_code: Optional[int]
    run_exit_code: Optional[int]


def cp932_ok(value: str) -> bool:
    try:
        value.encode("cp932")
        return True
    except UnicodeEncodeError:
        return False


def nested_path(root: Path, target_length: int, filename: str) -> Path:
    result = root / filename
    while len(os.fsencode(str(result))) < target_length:
        remaining = target_length - len(os.fsencode(str(result)))
        component_length = min(100, remaining - 1)
        if component_length < 1:
            break
        result = result.parent / ("A" * component_length) / result.name
    return result


def common_arg(path: Path) -> str:
    value = str(path)
    return value if value.endswith(("\\", "/")) else value + os.sep


def pe_bits(path: Path) -> Optional[int]:
    with path.open("rb") as stream:
        if stream.read(2) != b"MZ":
            return None
        stream.seek(0x3C)
        offset = struct.unpack("<I", stream.read(4))[0]
        stream.seek(offset + 4)
        machine = struct.unpack("<H", stream.read(2))[0]
    return {0x14C: 32, 0x8664: 64}.get(machine, 0)


def run(command: Sequence[object], cwd: Path, timeout: int = 20) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run([str(item) for item in command], cwd=str(cwd),
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          timeout=timeout, check=False)


def log_text(data: bytes) -> str:
    return "".join(
        f"--- decoded as {encoding} ---\n{data.decode(encoding, errors='replace')}\n"
        for encoding in ("utf-8", "cp932")
    )


def find_msbuild() -> Path:
    vswhere = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
    vswhere /= "Microsoft Visual Studio/Installer/vswhere.exe"
    completed = subprocess.run(
        [str(vswhere), "-latest", "-products", "*", "-requires",
         "Microsoft.Component.MSBuild", "-find", r"MSBuild\**\Bin\MSBuild.exe"],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=False)
    paths = [Path(line) for line in completed.stdout.splitlines() if line.strip()]
    if not paths:
        raise RuntimeError("Visual Studio MSBuild was not found")
    return paths[0]


def build_workers() -> dict[int, Path]:
    msbuild = find_msbuild()
    project = PATH_SUITE / "dll_worker.vcxproj"
    for platform in ("Win32", "x64"):
        completed = subprocess.run(
            [str(msbuild), str(project), "/m", "/nologo", "/v:minimal",
             "/p:Configuration=Release", f"/p:Platform={platform}"],
            cwd=str(PATH_SUITE), check=False)
        if completed.returncode:
            raise RuntimeError(f"failed to build DLL worker for {platform}")
    result = {
        32: ROOT / "artifacts/unicode_path/tools/Win32/dll_worker.exe",
        64: ROOT / "artifacts/unicode_path/tools/x64/dll_worker.exe",
    }
    if any(not path.is_file() for path in result.values()):
        raise RuntimeError("DLL workers were not produced")
    return result


def script_for(operation: str, token: str) -> tuple[str, dict[str, bytes], dict[str, bool]]:
    """Return HSP source, initial files, and expected final file presence."""
    name = token + ".dat"
    other = token + "_copy.dat"
    initial: dict[str, bytes] = {}
    expected: dict[str, bool] = {}
    if operation == "binary":
        source = f'''sdim data,64\npoke data,0,65\npoke data,1,66\npoke data,2,67\npoke data,3,68\nbsave "{name}",data,4\nmemset data,0,64\nbload "{name}",data,2,1\nif peek(data,0)!=66 : mes "RESULT FAIL binary-0" : end\nif peek(data,1)!=67 : mes "RESULT FAIL binary-1" : end\nmes "RESULT PASS"\nend\n'''
        expected[name] = True
    elif operation == "note":
        note_value = "HSP_NOTE_" + token
        source = f'''text="{note_value}"\nnotesel text\nnotesave "{name}"\nsdim loaded,256\nnotesel loaded\nnoteload "{name}"\nif loaded!="{note_value}" : mes "RESULT FAIL note" : end\nmes "RESULT PASS"\nend\n'''
        expected[name] = True
    elif operation == "exist":
        initial[name] = b"EXIST_DATA"
        source = f'''exist "{name}"\nif strsize!=10 : mes "RESULT FAIL exist" : end\nmes "RESULT PASS"\nend\n'''
    elif operation == "copy":
        initial[name] = b"COPY_DATA"
        source = f'''bcopy "{name}","{other}"\nmes "RESULT PASS"\nend\n'''
        expected[other] = True
    elif operation == "delete":
        initial[name] = b"DELETE_DATA"
        source = f'''delete "{name}"\nmes "RESULT PASS"\nend\n'''
        expected[name] = False
    elif operation == "mkdir-chdir":
        source = f'''mkdir "{token}"\nchdir "{token}"\nsdim data,16\npoke data,0,77\nbsave "inside.dat",data,1\nmes "RESULT PASS"\nend\n'''
        expected[token + "/inside.dat"] = True
    elif operation == "dirlist":
        initial[name] = b"DIRLIST_DATA"
        source = f'''sdim listing,4096\ndirlist listing,"*.dat",1\nif instr(listing,0,"{name}")<0 : mes "RESULT FAIL dirlist" : end\nmes "RESULT PASS"\nend\n'''
    else:
        raise ValueError(operation)
    return source, initial, expected


class Suite:
    def __init__(self, target: Target, output: Path, worker: Optional[Path]):
        self.target = target
        self.output = output
        self.worker = worker
        self.results: list[Result] = []
        self.temp = Path(tempfile.mkdtemp(prefix=f"hsp-runtime-{target.name}-"))

    def unsupported(self, case_id: str, operation: str, character_class: str,
                    locus: str, detail: str, status: str = "UNSUPPORTED") -> None:
        rel = Path(self.target.name) / case_id
        result = Result(self.target.name, case_id, operation, character_class, locus,
                        status, "prepare", detail, str(rel), [], [], None, None)
        self._save(result)

    def nested_ax_dir(self, case_id: str, target_length: int) -> str:
        work = self.temp / hashlib.sha1(case_id.encode()).hexdigest()[:12]
        return str(nested_path(work, target_length, "case.ax").parent.relative_to(work))

    def _save(self, result: Result) -> None:
        directory = self.output / result.artifact_dir
        directory.mkdir(parents=True, exist_ok=True)
        (directory / "result.json").write_text(
            json.dumps(asdict(result), ensure_ascii=False, indent=2), encoding="utf-8")
        self.results.append(result)

    def execute(self, case_id: str, operation: str, character_class: str,
                locus: str, source: str, initial: dict[str, bytes],
                expected: dict[str, bool], run_dir_name: str = "run",
                ax_dir_name: str = "build", dpm_builder=None,
                run_argument: Optional[str] = None,
                expected_rejection: bool = False,
                retain_work: bool = True) -> None:
        rel = Path(self.target.name) / case_id
        artifact = self.output / rel
        artifact.mkdir(parents=True, exist_ok=True)
        work = self.temp / hashlib.sha1(case_id.encode()).hexdigest()[:12]
        build = work / "build"
        run_dir = work / run_dir_name
        ax_dir = work / ax_dir_name
        build.mkdir(parents=True)
        run_dir.mkdir(parents=True, exist_ok=True)
        ax_dir.mkdir(parents=True, exist_ok=True)
        source_path = build / "case.hsp"
        ax_build = build / "case.ax"
        hsp_run_dir = str(run_dir).replace("\\", "\\\\")
        source = source.replace("__RUN_DIR__", hsp_run_dir)
        source = ('onerror goto *runtime_error\n' + source +
                  '\n*runtime_error\nmes "RESULT FAIL HSP_ERROR"\nend\n')
        source_path.write_bytes(source.encode(self.target.source_encoding))
        for name, data in initial.items():
            path = run_dir / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        if self.target.compiler_dll is None:
            compile_command = [self.target.compiler, "-d", "-u"]
            if self.target.utf8_input:
                compile_command.append("-i")
            compile_command += ["-o" + str(ax_build),
                                "--compath=" + common_arg(self.target.common),
                                source_path]
        else:
            compile_command = [self.worker, "runtime-compile", self.target.compiler_dll,
                               source_path, common_arg(self.target.common), ax_build,
                               "unused", "1" if self.target.utf8_input else "0"]
        run_command: list[object] = []
        status, stage, detail = "FAIL", "compile", "not run"
        compile_code = run_code = None
        try:
            compiled = run(compile_command, build, 40)
            compile_code = compiled.returncode
            (artifact / "compile.bin").write_bytes(compiled.stdout)
            (artifact / "compile.txt").write_text(log_text(compiled.stdout), encoding="utf-8")
            if compile_code:
                detail = f"compiler exited with {compile_code}"
            else:
                if dpm_builder is not None:
                    dpm_builder(work, build, run_dir, artifact)
                ax_path = ax_dir / "case.ax"
                if ax_path != ax_build:
                    shutil.copy2(ax_build, ax_path)
                argument = str(ax_path) if run_argument is None else run_argument
                run_command = [self.target.runtime, argument]
                executed = run(run_command, run_dir)
                run_code = executed.returncode
                (artifact / "runtime.bin").write_bytes(executed.stdout)
                (artifact / "runtime.txt").write_text(log_text(executed.stdout), encoding="utf-8")
                stage = "runtime"
                problems = []
                if expected_rejection:
                    safe_rejection = run_code == 0xFFFFFFFF or (
                        os.name != "nt" and run_code == 255)
                    if safe_rejection:
                        status = "KNOWN_LIMITATION"
                        detail = "rejected safely with -1"
                    elif run_code is not None and run_code >= 0xC0000000:
                        status = "CRASH"
                        detail = f"unsafe Windows exception exit 0x{run_code:08X}"
                    else:
                        problems.append(f"expected safe -1 rejection, got {run_code}")
                else:
                    if run_code:
                        problems.append(f"runtime exited with {run_code}")
                    if b"RESULT PASS" not in executed.stdout:
                        problems.append("RESULT PASS marker missing")
                for name, should_exist in expected.items():
                    present = (run_dir / name).is_file()
                    if present != should_exist:
                        problems.append(f"{name}: expected present={should_exist}, got {present}")
                if operation == "copy" and (run_dir / (TOKENS[character_class] + "_copy.dat")).is_file():
                    if (run_dir / (TOKENS[character_class] + "_copy.dat")).read_bytes() != b"COPY_DATA":
                        problems.append("copied bytes differ")
                tested_file = run_dir / (TOKENS[character_class] + ".dat")
                if operation == "binary" and tested_file.is_file():
                    if tested_file.read_bytes() != b"ABCD":
                        problems.append("saved binary bytes differ")
                if operation == "note" and tested_file.is_file():
                    expected_text = ("HSP_NOTE_" + TOKENS[character_class]).encode(
                        self.target.source_encoding)
                    if expected_text not in tested_file.read_bytes():
                        problems.append(f"saved text is not {self.target.source_encoding}")
                if operation == "dpm" and (run_dir / "copied.dat").is_file():
                    if (run_dir / "copied.dat").read_bytes() != b"DPM_ASSET":
                        problems.append("DPM copy did not take precedence over disk file")
                if problems:
                    status = "FAIL"
                    detail = "; ".join(problems)
                elif not expected_rejection:
                    status, detail = "PASS", "all checks passed"
        except Exception as error:
            status, stage, detail = "INFRA", stage, f"{type(error).__name__}: {error}"
        result = Result(self.target.name, case_id, operation, character_class, locus,
                        status, stage, detail, str(rel), [str(x) for x in compile_command],
                        [str(x) for x in run_command], compile_code, run_code)
        self._save(result)
        # Retain exact source/AX/DPM/files unless the path cannot be copied
        # below the artifact directory without exceeding PATH_MAX.
        if retain_work and work.exists():
            shutil.copytree(work, artifact / "work", dirs_exist_ok=True)
        if work.exists():
            shutil.rmtree(work)

    def file_cases(self) -> None:
        for cls, token in TOKENS.items():
            if self.target.source_encoding == "cp932" and not cp932_ok(token):
                for operation in OPERATIONS:
                    self.unsupported(f"file-{operation}-{cls}", operation, cls, "file",
                                     "name is not representable in CP932 source/runtime")
                self.unsupported(f"file-absolute-{cls}", "absolute", cls, "absolute-path",
                                 "name is not representable in CP932 source/runtime")
                continue
            for operation in OPERATIONS:
                source, initial, expected = script_for(operation, token)
                self.execute(f"file-{operation}-{cls}", operation, cls, "file",
                             source, initial, expected)
            absolute_name = token + "_absolute.dat"
            absolute_separator = "\\\\" if os.name == "nt" else "/"
            absolute_source = (f'sdim data,16\npoke data,0,65\n'
                               f'bsave "__RUN_DIR__{absolute_separator}{absolute_name}",data,1\n'
                               'mes "RESULT PASS"\nend\n')
            self.execute(f"file-absolute-{cls}", "absolute", cls, "absolute-path",
                         absolute_source, {}, {absolute_name: True})

    def boundary_cases(self) -> None:
        for cls, token in TOKENS.items():
            if self.target.source_encoding == "cp932" and not cp932_ok(token):
                for locus in ("cwd", "ax-path", "ax-path-space"):
                    self.unsupported(f"launch-{locus}-{cls}", "launch", cls, locus,
                                     "path is not representable by the baseline encoding")
                continue
            source = 'mes "RESULT PASS"\nend\n'
            self.execute(f"launch-cwd-{cls}", "launch", cls, "cwd", source, {}, {},
                         run_dir_name=token)
            self.execute(f"launch-ax-path-{cls}", "launch", cls, "ax-path", source, {}, {},
                         ax_dir_name=token)
            self.execute(f"launch-ax-path-space-{cls}", "launch", cls, "ax-path-space",
                         source, {}, {}, ax_dir_name="space " + token,
                         expected_rejection=not self.target.utf8_input)

        # Long arguments are known inputs and must be rejected without crashing
        # or hanging, even when the referenced AX does not exist.
        for length in (261, 512, 1024, 4094, 4096):
            argument = "A" * (length - 3) + ".ax"
            self.execute(f"launch-limit-ascii-{length}", "launch-limit", "ascii",
                         "argument-length", 'mes "RESULT PASS"\nend\n', {}, {},
                         run_argument=argument, expected_rejection=True)

        # This existing path remains below the Windows character limit and the
        # baseline CP932 byte limit, while exceeding the current UTF-8 byte cap.
        long_component = "日" * 70
        self.execute("launch-limit-multibyte-existing", "launch-limit", "cp932",
                     "utf8-byte-length", 'mes "RESULT PASS"\nend\n', {}, {},
                     ax_dir_name=long_component,
                     expected_rejection=self.target.utf8_input)

        if os.name != "nt":
            for length in (261, 512, 1024, 2048, 4090):
                case_id = f"launch-limit-existing-nested-{length}"
                self.execute(case_id, "launch-limit", "ascii", "path-length",
                             'mes "RESULT PASS"\nend\n', {}, {},
                             ax_dir_name=self.nested_ax_dir(case_id, length),
                             expected_rejection=False, retain_work=False)

    def make_dpm(self, token: str, automatic: bool):
        asset_name = token + ".txt"
        def builder(work: Path, build: Path, run_dir: Path, artifact: Path) -> None:
            pack_dir = work / "pack"
            pack_dir.mkdir()
            asset = pack_dir / asset_name
            asset.write_bytes(b"DPM_ASSET")
            pack_source = pack_dir / "pack.hsp"
            pack_source.write_bytes((f'#pack "{asset_name}"\nmes "PACK"\n').encode(
                self.target.source_encoding))
            # MAKEPACK always adds start.ax to the generated packfile.
            pack_ax = pack_dir / "start.ax"
            pack_base = pack_dir / "container"
            command = [self.worker, "pack-flow", self.target.compiler_dll, pack_source,
                       common_arg(self.target.common), pack_ax,
                       "1" if self.target.utf8_input else "0", pack_base, asset_name, "0"]
            completed = run(command, pack_dir, 40)
            (artifact / "dpm-build.bin").write_bytes(completed.stdout)
            (artifact / "dpm-build.txt").write_text(log_text(completed.stdout), encoding="utf-8")
            if completed.returncode:
                raise RuntimeError(f"DPM builder exited with {completed.returncode}")
            made = pack_base.with_suffix(".dpm")
            destination = run_dir / ("data.dpm" if automatic else token + ".dpm")
            shutil.copy2(made, destination)
            asset.unlink()
        return builder

    def dpm_cases(self) -> None:
        if self.target.compiler_dll is None:
            for cls in TOKENS:
                for mode in ("explicit", "automatic"):
                    self.unsupported(f"dpm-{mode}-{cls}", "dpm", cls, "dpm",
                                     "hspcmp DLL is not configured", status="SKIPPED")
            for cls in TOKENS:
                self.unsupported(f"dpm-cwd-{cls}", "dpm", cls, "cwd",
                                 "hspcmp DLL is not configured", status="SKIPPED")
            return
        for cls, token in TOKENS.items():
            if self.target.source_encoding == "cp932" and not cp932_ok(token):
                for mode in ("explicit", "automatic"):
                    self.unsupported(f"dpm-{mode}-{cls}", "dpm", cls, "dpm",
                                     "DPM name/entry is not representable in CP932")
                continue
            asset = token + ".txt"
            for mode in ("explicit", "automatic"):
                prefix = "" if mode == "automatic" else f'chdpm "{token}.dpm"\n'
                source = prefix + f'''exist "{asset}"\nif strsize!=9 : mes "RESULT FAIL dpm-exist" : end\nsdim data,64\nbload "{asset}",data\nif peek(data,0)!=68 : mes "RESULT FAIL dpm-bload" : end\nsdim text,64\nnotesel text\nnoteload "{asset}"\nif text!="DPM_ASSET" : mes "RESULT FAIL dpm-noteload" : end\nbcopy "{asset}","copied.dat"\nexist "fallback.dat"\nif strsize!=13 : mes "RESULT FAIL disk-fallback-exist" : end\nmemset data,0,64\nbload "fallback.dat",data\nif peek(data,0)!=68 : mes "RESULT FAIL disk-fallback-bload" : end\nmes "RESULT PASS"\nend\n'''
                initial = {asset: b"DISK_ASSET", "fallback.dat": b"DISK_FALLBACK"}
                self.execute(f"dpm-{mode}-{cls}", "dpm", cls, "dpm", source, initial,
                             {"copied.dat": True}, dpm_builder=self.make_dpm(token, mode == "automatic"))

        # Isolate relative automatic-DPM lookup from DPM/entry-name encoding:
        # only the process current directory varies across character classes.
        asset = TOKENS["ascii"] + ".txt"
        source = f'''exist "{asset}"\nif strsize!=9 : mes "RESULT FAIL dpm-cwd-exist" : end\nmes "RESULT PASS"\nend\n'''
        for cls, token in TOKENS.items():
            self.execute(f"dpm-cwd-{cls}", "dpm", cls, "cwd", source,
                         {asset: b"DISK_ASSET"}, {}, run_dir_name=token,
                         dpm_builder=self.make_dpm(TOKENS["ascii"], True))

    def all(self) -> list[Result]:
        self.file_cases()
        self.boundary_cases()
        self.dpm_cases()
        shutil.rmtree(self.temp, ignore_errors=True)
        return self.results


def compare(baseline: list[Result], candidate: list[Result]) -> list[dict[str, str]]:
    left = {x.case_id: x for x in baseline}
    right = {x.case_id: x for x in candidate}
    rows = []
    for case_id in sorted(set(left) | set(right)):
        bs = left.get(case_id).status if case_id in left else "MISSING"
        cs = right.get(case_id).status if case_id in right else "MISSING"
        if "INFRA" in (bs, cs) or "MISSING" in (bs, cs):
            verdict = "INFRA"
        elif bs == cs == "PASS":
            verdict = "SAME_PASS"
        elif bs == "PASS":
            verdict = "REGRESSION"
        elif cs == "PASS":
            verdict = "IMPROVEMENT"
        elif bs in ("FAIL", "CRASH") and cs == "KNOWN_LIMITATION":
            verdict = "IMPROVEMENT"
        elif bs == "KNOWN_LIMITATION" and cs in ("FAIL", "CRASH"):
            verdict = "REGRESSION"
        else:
            verdict = "SHARED_LIMITATION"
        rows.append({"case_id": case_id, "baseline": bs, "candidate": cs, "verdict": verdict})
    return rows


def write_summary(output: Path, targets: list[Target], results: list[Result], rows: list[dict[str, str]]) -> None:
    metadata = []
    for target in targets:
        metadata.append({"name": target.name, "runtime": str(target.runtime),
                         "runtime_bits": pe_bits(target.runtime),
                         "compiler_dll": (str(target.compiler_dll)
                                          if target.compiler_dll else None),
                         "compiler": str(target.compiler),
                         "compiler_bits": (pe_bits(target.compiler_dll)
                                            if target.compiler_dll else None),
                         "common": str(target.common),
                         "source_encoding": target.source_encoding})
    payload = {"created": datetime.now().isoformat(), "targets": metadata,
               "results": [asdict(x) for x in results], "comparisons": rows}
    (output / "summary.json").write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")
    counts: dict[str, int] = {}
    for row in rows:
        counts[row["verdict"]] = counts.get(row["verdict"], 0) + 1
    lines = ["# HSP3CL Unicode runtime comparison", "", "## Verdicts", ""]
    lines += [f"- {key}: {value}" for key, value in sorted(counts.items())]
    lines += ["", "## Cases", "", "| Case | Baseline | Candidate | Verdict |",
              "|---|---:|---:|---|"]
    lines += [f'| {r["case_id"]} | {r["baseline"]} | {r["candidate"]} | {r["verdict"]} |' for r in rows]
    (output / "summary.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline-root", type=Path)
    parser.add_argument("--candidate-runtime", type=Path,
                        default=(ROOT / "hsp3cl" if os.name != "nt"
                                 else ROOT / "src/hsp3/Release64/hsp3cl_64.exe"))
    parser.add_argument("--candidate-hspcmp", type=Path,
                        default=(ROOT / "hspcmp" if os.name != "nt"
                                 else ROOT / "src/hspcmp/Release64/hspcmp.exe"))
    parser.add_argument("--candidate-dll", type=Path,
                        default=(None if os.name != "nt"
                                 else ROOT / "src/hspcmp/Release64/hspcmp_64.dll"))
    parser.add_argument("--candidate-common", type=Path, default=ROOT / "common")
    parser.add_argument("--output", type=Path)
    return parser.parse_args()


def main() -> int:
    args = arguments()
    output = args.output or ROOT / "artifacts/unicode_runtime/runs" / datetime.now().strftime("%Y%m%d-%H%M%S")
    output.mkdir(parents=True, exist_ok=True)
    if os.name == "nt":
        baseline_root = (args.baseline_root or Path(r"C:\hsp37")).resolve()
        targets = [
            Target("baseline-3.7", baseline_root / "hsp3cl.exe",
                   baseline_root / "hspcmp.exe", baseline_root / "hspcmp.dll",
                   baseline_root / "common", "cp932", False),
            Target("candidate", args.candidate_runtime, args.candidate_hspcmp,
                   args.candidate_dll, args.candidate_common, "utf-8", True),
        ]
    elif args.baseline_root:
        baseline_root = args.baseline_root.resolve()
        targets = [
            Target("baseline-3.7", baseline_root / "hsp3cl",
                   baseline_root / "hspcmp", None,
                   baseline_root / "common", "utf-8", True),
            Target("candidate", args.candidate_runtime, args.candidate_hspcmp,
                   args.candidate_dll, args.candidate_common, "utf-8", True),
        ]
    else:
        targets = [Target("candidate", args.candidate_runtime, args.candidate_hspcmp,
                          args.candidate_dll, args.candidate_common, "utf-8", True)]
    missing = [str(path) for target in targets for path in
               (target.runtime, target.compiler, target.common, target.compiler_dll)
               if path is not None and not path.exists()]
    if missing:
        print("Missing required inputs:\n" + "\n".join(missing), file=sys.stderr)
        return 2
    try:
        workers = (build_workers() if any(target.compiler_dll for target in targets)
                   else {})
    except Exception as error:
        print(f"Infrastructure error: {error}", file=sys.stderr)
        return 2
    all_results: list[Result] = []
    grouped = []
    for target in targets:
        print(f"Running {target.name} ...", flush=True)
        worker = (workers[pe_bits(target.compiler_dll)]
                  if target.compiler_dll is not None else None)
        current = Suite(target, output, worker).all()
        grouped.append(current)
        all_results.extend(current)
    rows = compare(grouped[0], grouped[1]) if len(grouped) == 2 else []
    write_summary(output, targets, all_results, rows)
    print(f"Results: {output}")
    regressions = sum(x["verdict"] == "REGRESSION" for x in rows)
    infra = sum(x["verdict"] == "INFRA" for x in rows)
    print(f"REGRESSION={regressions} INFRA={infra}")
    return 1 if regressions or infra else 0


if __name__ == "__main__":
    raise SystemExit(main())
