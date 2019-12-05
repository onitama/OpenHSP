REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild win32/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32dll/hspcmp.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
copy /B /Y win32\Release\hspcmp.exe ..\hsp3\Release
copy /B /Y win32dll\Release\hspcmp.dll ..\hsp3\Release
