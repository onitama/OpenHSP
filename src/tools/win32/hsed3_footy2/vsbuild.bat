REM Batch build script for Visual Studio 2017/2019
echo off
MSBuild hsed_vc2017.sln -t:Rebuild -p:Configuration=Release;Platform="win32"
MSBuild hsed_vc2017.sln -t:Rebuild -p:Configuration=release_en;Platform="win32"
copy /B /Y Release\hsed3.exe ..\..\..\hsp3\Release
copy /B /Y release_en\hsed3.exe ..\..\..\hsp3\Release\hsed3_en.exe
