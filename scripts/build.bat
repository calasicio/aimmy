@echo off
setlocal

for %%I in ("%~dp0..") do set "ROOT_DIR=%%~fI"

set "BUILD_DIR=%ROOT_DIR%\build"
set "RUNTIME_DIR=%ROOT_DIR%\runtime"
set "SOURCE=%BUILD_DIR%\bin\aimmy.exe"
set "DEST=%RUNTIME_DIR%\aimmy.exe"

echo.
echo [Aimmy] Configuring CMake...
cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%"

if errorlevel 1 (
    echo.
    echo [Aimmy] CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo [Aimmy] Building Debug...
cmake --build "%BUILD_DIR%" --config Debug

if errorlevel 1 (
    echo.
    echo [Aimmy] Build failed.
    pause
    exit /b 1
)

if not exist "%SOURCE%" (
    echo.
    echo [Aimmy] Built executable was not found:
    echo %SOURCE%
    pause
    exit /b 1
)

if not exist "%RUNTIME_DIR%" (
    mkdir "%RUNTIME_DIR%"
)

echo.
echo [Aimmy] Copying executable...
copy /Y "%SOURCE%" "%DEST%" >nul

if errorlevel 1 (
    echo.
    echo [Aimmy] Failed to copy executable.
    pause
    exit /b 1
)

echo.
echo ========================================
echo [Aimmy] Build successful!
echo [Aimmy] Runtime: %DEST%
echo ========================================
echo.

pause