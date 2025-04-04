REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild win32/hsp3dish.sln -t:Clean -p:Configuration=Release;Platform="Win32"
MSBuild win32/hsp3dish.sln -t:Clean -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Clean -p:Configuration=Release;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Clean -p:Configuration=hsprt;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Clean -p:Configuration=angle_Release;Platform="Win32"
MSBuild win32gp/hsp3gp.sln -t:Clean -p:Configuration=angle_hsprt;Platform="Win32"
