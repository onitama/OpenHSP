REM Batch build script for Visual Studio 2022
echo off
MSBuild win32gui/hsp3.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32gui/hsp3.sln -t:Rebuild -p:Configuration=hsprt;Platform="x86"
MSBuild win32gui/hsp3.sln -t:Rebuild -p:Configuration=Release-unicode;Platform="x86"
MSBuild win32gui/hsp3.sln -t:Rebuild -p:Configuration=hsprt-unicode;Platform="x86"
MSBuild hsp3_64.sln -t:Rebuild -p:Configuration=Release;Platform="x64"
MSBuild hsp3_64.sln -t:Rebuild -p:Configuration=hrt64_release;Platform="x64"
MSBuild win32/hsp3cl.sln -t:Rebuild -p:Configuration=Release;Platform="x86"
MSBuild win32/hsp3cl.sln -t:Rebuild -p:Configuration=hsp3cl_hrt;Platform="x86"

if not exist Release mkdir Release
if not exist Release\runtime mkdir Release\runtime
if not exist Release64 mkdir Release64
if not exist Release64\runtime mkdir Release64\runtime

copy /B /Y win32gui\hsprt\hsp3.exe Release\hsprt
copy /B /Y win32gui\Release\hsp3.exe Release
copy /B /Y win32gui\Release-unicode\hsp3utf.exe Release
copy /B /Y win32gui\hsprt-unicode\hsp3utf.hrt Release\runtime
copy /B /Y x64\Release\hsp3_64.exe Release64
copy /B /Y x64\hrt_release\hsp3_64.hrt Release64\runtime
copy /B /Y win32\Release\hsp3cl.exe Release
copy /B /Y win32\hsp3cl_hrt\hsp3cl.hrt Release\runtime

