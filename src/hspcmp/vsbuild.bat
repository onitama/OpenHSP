REM Batch build script for Visual Studio 2022
echo off
MSBuild win32/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32dll/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
MSBuild win32dll/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
copy /B /Y win32\Release\hspcmp.exe ..\..\package\win32
copy /B /Y win32dll\Release\hspcmp.dll ..\..\package\win32
copy /B /Y win32\x64\Release\hspcmp.exe ..\..\package\win64
copy /B /Y win32dll\x64\Release\hspcmp_64.dll ..\..\package\win64
