# HSP Unicode path/source compatibility tests

This suite compares the Windows HSP 3.7 x86 ANSI/CP932 compiler with the
development x64 compiler. Every case runs independently: a failed compiler or
DLL call is recorded and the remaining cases continue.

From the repository root, the normal comparison is:

```text
test\test_unicode_path_handling\run_tests.bat
```

The defaults are `C:\hsp37` for the baseline and
`src\hspcmp\Release64\hspcmp.exe` / `hspcmp_64.dll` for the candidate. Optional
batch arguments are the baseline package root, candidate EXE, and candidate DLL.

On Linux, run `python3 test/test_unicode_path_handling/run_tests.py`. It uses
the repository-root `hspcmp` and runs CLI cases; DLL cases are recorded as
`SKIPPED` because no `hspcmp.dll` is available.

The equivalent explicit command is:

```text
python test/test_unicode_path_handling/run_tests.py ^
  --baseline-hspcmp C:\hsp37\hspcmp.exe ^
  --baseline-hspcmp-dll C:\hsp37\hspcmp.dll ^
  --baseline-common C:\hsp37\common ^
  --candidate-hspcmp src\hspcmp\Release64\hspcmp.exe ^
  --candidate-hspcmp-dll src\hspcmp\Release64\hspcmp_64.dll ^
  --candidate-common common
```

## Coverage

The matrix distinguishes these filename and source-string ranges:

- strict ASCII: `ascii_A1`
- ASCII with a space: `space ascii_A1`
- Windows CP932/ANSI: `日本語_表ソ能` (`表`, `ソ`, and `能` have `0x5c` as
  their second CP932 byte)
- CP932 with a space: `空白 日本語_表ソ能`
- BMP outside CP932: `BMP_☃` (U+2603)
- supplementary plane: `EXT_😀` (U+1F600)

CP932, UTF-8, and UTF-8-with-BOM sources are compiled through both the CLI and
DLL. Main source, relative include, output, and common paths are varied one at
a time. `#packopt name`, DPM creation, and DPM reload/extraction are also tested.
UTF-16 source files are outside the current hspcmp input contract and are not
part of this suite.

Missing-source and malformed-UTF-8 controls verify diagnostic behavior and
that a compiler error does not prevent later cases from running.
CLI and DLL boundary cases also pass missing source arguments at
261/512/1024/4094/4096 bytes. Safe rejection is recorded separately as
`KNOWN_LIMITATION`; access violations and worker exceptions are `CRASH`, so a
crash cannot be mistaken for a successful negative test.
On Linux, `cli-existing-nested-path-*` compiles existing sources whose path
components are at most 100 bytes while the complete paths are 261, 512, 1024,
2048, and 4090 bytes.
An existing source path made from repeated multibyte characters stays within
the Windows character limit while crossing internal byte-oriented limits.

The legacy DLL API accepts Windows ACP/CP932 `char *` paths. BMP and
supplementary-plane paths that cannot be represented by that contract are
reported as `UNSUPPORTED_BY_API` instead of being passed as lossy strings. The
x86 HSP 3.7 DLL is called by an x86 native worker and the x64 DLL by an x64
worker; Visual Studio 2022 Build Tools are required to build them.
The worker disables Windows critical-error/GP-fault dialogs and converts DLL
crashes into exit code `128` plus a `WORKER_CRASH` log entry, so unattended runs
do not wait for an OK button.

Common paths passed through `--compath` or `hsc_compath` are normalized with a
trailing directory separator, as required by the HSP 3.7 directory-prefix API.

## Results and exit status

Results are stored below `artifacts/unicode_path/runs/<timestamp>/`:

- `summary.json`: machine-readable target metadata, cases, and comparisons
- `summary.md`: comparison table
- per-case raw output, UTF-8/CP932 decoded output, command, and result JSON
- failed case source/include/assets and partially generated files under `work/`

Passed case work files are removed, but their logs remain. Comparison mode exits
nonzero only for infrastructure errors or a regression (baseline `PASS` and
candidate not `PASS`). Candidate improvements and shared Unicode limitations
are reported without stopping the run.

Single-target compatibility remains available through `--hspcmp`, `--common`,
and optional `--hspcmp-dll`. The low-level C++ path test remains under
`test/test_hsp3pathio` and can be run separately with:

```text
make -C test/test_hsp3pathio clean run
```
