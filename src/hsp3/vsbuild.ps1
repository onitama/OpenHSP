# Don't halt on errors
$ErrorActionPreference = "SilentlyContinue"

& MSBuild "win32gui/hsp3.sln" -t:Rebuild -p:Configuration=Release;Platform="x86"
& MSBuild "win32gui/hsp3.sln" -t:Rebuild -p:Configuration=hsprt;Platform="x86"
& MSBuild "win32gui/hsp3.sln" -t:Rebuild -p:Configuration=Release-unicode;Platform="x86"
& MSBuild "win32gui/hsp3.sln" -t:Rebuild -p:Configuration=hsprt-unicode;Platform="x86"
& MSBuild "hsp3_64.sln" -t:Rebuild -p:Configuration=Release;Platform="x64"
& MSBuild "hsp3_64.sln" -t:Rebuild -p:Configuration=hrt64_release;Platform="x64"
& MSBuild "win32/hsp3cl.sln" -t:Rebuild -p:Configuration=Release;Platform="x86"
& MSBuild "win32/hsp3cl.sln" -t:Rebuild -p:Configuration=hsp3cl_hrt;Platform="x86"

Copy-Item -Path "win32gui\hsprt\hsp3.exe" -Destination "Release\hsprt" -Force
Copy-Item -Path "win32gui\Release\hsp3.exe" -Destination "Release" -Force
Copy-Item -Path "win32gui\Release-unicode\hsp3utf.exe" -Destination "Release" -Force
Copy-Item -Path "win32gui\hsprt-unicode\hsp3utf.hrt" -Destination "Release\runtime" -Force
Copy-Item -Path "x64\Release\hsp3_64.exe" -Destination "Release" -Force
Copy-Item -Path "x64\hrt_release\hsp3_64.hrt" -Destination "Release\runtime" -Force
Copy-Item -Path "win32\Release\hsp3cl.exe" -Destination "Release" -Force
Copy-Item -Path "win32\hsp3cl_hrt\hsp3cl.hrt" -Destination "Release\runtime" -Force