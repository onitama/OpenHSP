REM Batch build script for Visual Studio 2022
echo off
MSBuild win32/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32dll/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
MSBuild win32dll/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"

if not exist Release mkdir Release
if not exist Release\runtime mkdir Release\runtime
if not exist Release64 mkdir Release64
if not exist Release64\runtime mkdir Release64\runtime

copy /B /Y win32\Release\hspcmp.exe Release
copy /B /Y win32dll\Release\hspcmp.dll Release
copy /B /Y win32\x64\Release\hspcmp.exe Release64
copy /B /Y win32dll\x64\Release\hspcmp_64.dll Release64
