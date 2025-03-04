REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild win32gui/hsp3.sln -t:Clean -p:Configuration=Release;Platform="x86"
MSBuild win32gui/hsp3.sln -t:Clean -p:Configuration=hsprt;Platform="x86"
MSBuild win32gui/hsp3.sln -t:Clean -p:Configuration=Release-unicode;Platform="x86"
MSBuild win32gui/hsp3.sln -t:Clean -p:Configuration=hsprt-unicode;Platform="x86"
MSBuild hsp3_64.sln -t:Clean -p:Configuration=Release;Platform="x64"
MSBuild hsp3_64.sln -t:Clean -p:Configuration=hrt64_release;Platform="x64"
MSBuild win32/hsp3cl.sln -t:Clean -p:Configuration=Release;Platform="x86"
MSBuild win32/hsp3cl.sln -t:Clean -p:Configuration=hsp3cl_hrt;Platform="x86"
