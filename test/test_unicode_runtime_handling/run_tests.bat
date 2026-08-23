@echo off
setlocal
cd /d "%~dp0\..\.."
python test\test_unicode_runtime_handling\run_tests.py %*
exit /b %errorlevel%
