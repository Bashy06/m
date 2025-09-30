@echo off
echo Building 5iive Anti-Cheat...

:: Clean
if exist "bin" rmdir /s /q "bin"
mkdir bin

:: Build Driver
echo Building driver...
cd driver
msbuild "driver.vccproj" /p:Configuration=Release /p:Platform=x64 /p:OutDir=..\bin\
if %errorlevel% neq 0 (
    echo Driver build failed!
    pause
    exit /b 1
)
cd ..

:: Build User
echo Building user application...
cd user
cl /nologo /O2 /EHsc interface.cpp mapper.cpp ProcessUtils.cpp userland.cpp /link /out:..\bin\5iiveLoader.exe
cd ..

echo.
echo Build complete! Check bin folder.
dir bin
pause