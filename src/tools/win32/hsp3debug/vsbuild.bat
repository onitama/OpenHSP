REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild hsp3debug.sln -t:Rebuild -p:Configuration=Release;Platform="Win32"
MSBuild hsp3debug.sln -t:Rebuild -p:Configuration=Release-unicode;Platform="Win32"
MSBuild hsp3debug.sln -t:Rebuild -p:Configuration=Release-unicode;Platform="x64"
copy /B /Y Release\hsp3debug.dll ..\..\..\hsp3\Release
copy /B /Y Release-unicode\hsp3debug_u8.dll ..\..\..\hsp3\Release
copy /B /Y x64\Release-unicode\hsp3debug_64.dll ..\..\..\hsp3\Release
