@echo off
title Rust Anti-Cheat Builder
echo ========================================
echo    Rust Anti-Cheat Build System
echo ========================================

set BUILD_CONFIG=Release
set PLATFORM=x64
set OUTPUT_DIR=bin\%BUILD_CONFIG%

:: Clean previous build
if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%"

:: Build Driver
echo.
echo [1/3] Building Driver...
msbuild driver\driver.vcxproj /p:Configuration=%BUILD_CONFIG% /p:Platform=%PLATFORM% /p:OutDir=..\%OUTPUT_DIR%\ /m

if %errorlevel% neq 0 (
    echo ERROR: Driver build failed!
    pause
    exit /b 1
)

:: Build Userland
echo.
echo [2/3] Building Userland...
msbuild userland\userland.vcxproj /p:Configuration=%BUILD_CONFIG% /p:Platform=%PLATFORM% /p:OutDir=..\%OUTPUT_DIR%\ /m

if %errorlevel% neq 0 (
    echo ERROR: Userland build failed!
    pause
    exit /b 1
)

:: Build Overlay
echo.
echo [3/3] Building Overlay...
msbuild overlay\overlay.vcxproj /p:Configuration=%BUILD_CONFIG% /p:Platform=%PLATFORM% /p:OutDir=..\%OUTPUT_DIR%\ /m

if %errorlevel% neq 0 (
    echo ERROR: Overlay build failed!
    pause
    exit /b 1
)

:: Copy dependencies
echo.
echo Copying dependencies...
copy "resources\*" "%OUTPUT_DIR%\" 2>nul

echo.
echo ========================================
echo    Build Completed Successfully!
echo ========================================
echo Output directory: %OUTPUT_DIR%
echo.
dir "%OUTPUT_DIR%"
echo.
pause