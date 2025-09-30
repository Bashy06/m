@echo off
echo Building Rust CR3 Cheat Framework...
echo.

echo Building kernel driver...
cd driver
cl.exe /nologo /Zl /W4 /O1 /kernel /D_AMD64_ ^
/I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\km" ^
/Fo"..\build\drivers\\" /Fe"..\build\drivers\RustCR3Driver.sys" ^
driverEntry.cpp DirbaseBypass.cpp communication.cpp ^
/link /nologo /SUBSYSTEM:NATIVE /DRIVER /MACHINE:X64 ^
/OUT:..\build\drivers\RustCR3Driver.sys

echo Building userland...
cd ..\user
cl.exe /std:c++17 /O2 /I..\shared /Fe"..\build\bin\RustLoader.exe" ^
interface.cpp injector\main.cpp mapper.cpp

echo.
echo Build complete!
echo.
echo Files:
echo - build\drivers\RustCR3Driver.sys
echo - build\bin\RustLoader.exe
echo.
pause