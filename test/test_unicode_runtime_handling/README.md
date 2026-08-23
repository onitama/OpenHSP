# HSP3CL Unicode runtime compatibility tests

This suite compares file and DPM handling in the HSP 3.7 x86 CP932 runtime
with the development x64 UTF-8 runtime. Run it from the repository root:

```text
test\test_unicode_runtime_handling\run_tests.bat
```

Defaults are `C:\hsp37\hsp3cl.exe` and
`src\hsp3\Release64\hsp3cl_64.exe`. Use `--help` to override runtime,
compiler DLL, common-directory, or output paths. Visual Studio Build Tools are
needed to build the x86/x64 compiler-DLL worker shared with
`test_unicode_path_handling`.

On Linux, run `python3 test/test_unicode_runtime_handling/run_tests.py`. It uses
the repository-root `hspcmp` and `hsp3cl`; compiler-DLL-dependent DPM cases are
recorded as `SKIPPED`. Use `--candidate-hspcmp` to override the compiler path.
To compare another Linux build, add `--baseline-root ../OpenHSP-3.7/`.

## Coverage

Cases distinguish ASCII, CP932, BMP outside CP932, and supplementary-plane
names. They exercise `bload`, `bsave`, offsets, `noteload`, `notesave`, `exist`,
`bcopy`, `delete`, `mkdir`, `chdir`, and `dirlist`, plus Unicode working
directories and AX launch paths.

Launch-boundary cases also cover spaces, safely rejected missing arguments at
261/512/1024/4094/4096 bytes, and an existing multibyte path that fits the
legacy CP932 limit but exceeds the development runtime's current UTF-8 byte
limit. On Linux, `launch-limit-existing-nested-*` launches existing AX files
whose path components are at most 100 bytes while the complete paths are 261,
512, 1024, 2048, and 4090 bytes. Expected `-1` rejection is recorded as
`KNOWN_LIMITATION`; a timeout,
access violation, or stack-corruption exit remains a failure.

Normal DPM files are built separately by each target's own compiler. Explicit
`chdpm` and automatic `data.dpm` loading exercise `exist`, `bload`, and `bcopy`
from packed assets. Cross-version and encrypted-DPM compatibility are outside
this iteration.

Baseline source is CP932 and candidate source is UTF-8. A name that cannot be
expressed by the baseline contract is `UNSUPPORTED`, not a baseline failure.

## Evidence and exit status

Every case is independent. Compilation or runtime failure does not stop later
cases. `artifacts/unicode_runtime/runs/<timestamp>` retains source, AX, DPM,
input/output files, raw logs, dual-decoded logs, per-case JSON, `summary.json`,
and `summary.md`, including successful fixtures.

Comparisons are `SAME_PASS`, `IMPROVEMENT`, `REGRESSION`, or
`SHARED_LIMITATION`. The runner exits nonzero only for a regression (baseline
passes and candidate does not) or an infrastructure error.
