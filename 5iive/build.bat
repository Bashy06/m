@echo off
echo Building 5iive Anti-Cheat...

:: Clean
if exist "bin" rmdir /s /q "bin"
mkdir bin
mkdir bin\driver
mkdir bin\user

:: Build Driver
echo Building driver...
cd driver
msbuild "driver.vccproj" /p:Configuration=Release /p:Platform=x64 /p:OutDir=..\bin\driver\
cd ..

:: Build User Application  
echo Building user application...
cd user
cl /nologo /O2 /EHsc /I..\shared interface.cpp mapper.cpp ProcessUtils.cpp userland.cpp /Fe:..\bin\user\5iiveLoader.exe
cd ..

:: Build Injector
echo Building injector...
cd user\injector  
cl /nologo /O2 /EHsc /I..\..\shared main.cpp /Fe:..\..\bin\user\injector.exe
cd ..\..

:: Build Menu (if needed)
echo Building menu components...
cd menu
cl /nologo /O2 /EHsc /I..\shared /DIMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS overlay_dx9.cpp imgui_menu.cpp /Fe:..\bin\user\menu.exe
cd ..

echo.
echo Build Complete!
echo Files in 'bin' folder:
dir bin /s

pause