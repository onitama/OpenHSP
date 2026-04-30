REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=hsprt;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_Release;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_hsprt;Platform="x64"

if not exist ..\hsp3\Release mkdir ..\hsp3\Release
if not exist ..\hsp3\Release\runtime mkdir ..\hsp3\Release\runtime

copy /B /Y win32\Release\hsp3dish.exe ..\hsp3\Release
copy /B /Y win32\hsprt\hsp3dish.exe ..\hsp3\Release\runtime\hsp3dish.hrt
copy /B /Y win32gp\x64\Release\hsp3gp.exe ..\hsp3\Release
copy /B /Y win32gp\x64\hsprt\hsp3gp.hrt ..\hsp3\Release\runtime
copy /B /Y win32gp\x64\angle_Release\hsp3gpdx.exe ..\hsp3\Release
copy /B /Y win32gp\x64\angle_hsprt\hsp3gpdx.hrt ..\hsp3\Release\runtime

dir ..\hsp3\Release
dir ..\hsp3\Release\runtime
