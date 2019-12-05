REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild win32/hsp3dish.sln -t:Rebuild -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_Release;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Rebuild -p:Configuration=angle_hsprt;Platform="Win32"
copy /B /Y win32\Release\hsp3dish.exe ..\hsp3\Release
copy /B /Y win32\hsprt\hsp3dish.exe ..\hsp3\Release\runtime\hsp3dish.hrt
copy /B /Y win32gp\Release\hsp3gp.exe ..\hsp3\Release
copy /B /Y win32gp\hsprt\hsp3gp.hrt ..\hsp3\Release\runtime
copy /B /Y win32gp\angle_Release\hsp3gpdx.exe ..\hsp3\Release
copy /B /Y win32gp\angle_hsprt\hsp3gpdx.hrt ..\hsp3\Release\runtime
