REM Batch build script for Visual Studio 2022
echo off
MSBuild hsp3debug.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild hsp3debug.sln -t:Rebuild -p:Configuration=Release-unicode;Platform="x64"
copy /B /Y Win32\Release\hsp3debug.dll ..\..\..\..\package\win32
copy /B /Y x64\Release-unicode\hsp3debug_64.dll ..\..\..\..\package\win64
