@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "ROOT=%SCRIPT_DIR%..\.."
set "BASELINE_ROOT=%~1"
if "%BASELINE_ROOT%"=="" set "BASELINE_ROOT=C:\hsp37"
set "CANDIDATE_EXE=%~2"
if "%CANDIDATE_EXE%"=="" set "CANDIDATE_EXE=%ROOT%\src\hspcmp\Release64\hspcmp.exe"
set "CANDIDATE_DLL=%~3"
if "%CANDIDATE_DLL%"=="" set "CANDIDATE_DLL=%ROOT%\src\hspcmp\Release64\hspcmp_64.dll"

python "%SCRIPT_DIR%run_tests.py" ^
  --baseline-hspcmp "%BASELINE_ROOT%\hspcmp.exe" ^
  --baseline-hspcmp-dll "%BASELINE_ROOT%\hspcmp.dll" ^
  --baseline-common "%BASELINE_ROOT%\common" ^
  --candidate-hspcmp "%CANDIDATE_EXE%" ^
  --candidate-hspcmp-dll "%CANDIDATE_DLL%" ^
  --candidate-common "%ROOT%\common"
exit /b %ERRORLEVEL%
