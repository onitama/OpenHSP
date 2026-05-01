REM Batch build script for Visual Studio 2022
echo off
MSBuild hsed3.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild hsed3.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
copy /B /Y Release\hsed3.exe ..\..\package\win32\hsed3le.exe
copy /B /Y x64\Release\hsed3.exe ..\..\package\win64\hsed3le.exe
