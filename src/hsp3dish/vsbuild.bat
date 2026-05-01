REM Batch build script for Visual Studio 2022
echo off
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=hsprt;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_Release;Platform="x64"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_hsprt;Platform="x64"

copy /B /Y win32\Release\hsp3dish.exe ..\..\package\win32
copy /B /Y win32\hsprt\hsp3dish.exe ..\..\package\win32\runtime\hsp3dish.hrt
copy /B /Y win32gp\x64\Release\hsp3gp.exe ..\..\package\win64
copy /B /Y win32gp\x64\hsprt\hsp3gp.hrt ..\..\package\win64\runtime
copy /B /Y win32gp\x64\angle_Release\hsp3gpdx.exe ..\..\package\win64
copy /B /Y win32gp\x64\angle_hsprt\hsp3gpdx.hrt ..\..\package\win64\runtime

