REM Batch build script for Visual Studio 2022
echo off
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=hsprt;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_Release;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_hsprt;Platform="x64"

if not exist Release mkdir Release
if not exist Release\runtime mkdir Release\runtime
if not exist Release64 mkdir Release64
if not exist Release64\runtime mkdir Release64\runtime

copy /B /Y win32\Release\hsp3dish.exe Release
copy /B /Y win32\hsprt\hsp3dish.exe Release\runtime\hsp3dish.hrt
copy /B /Y win32gp\x64\Release\hsp3gp.exe Release64
copy /B /Y win32gp\x64\hsprt\hsp3gp.hrt Release64\runtime
copy /B /Y win32gp\x64\angle_Release\hsp3gpdx.exe Release64
copy /B /Y win32gp\x64\angle_hsprt\hsp3gpdx.hrt Release64\runtime

