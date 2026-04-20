
@echo off
echo ===================================================
echo			Build Assembler Script
echo ===================================================
echo.

REM Try to find MSBuild in common locations
set MSBUILD_PATH=""

REM Visual Studio 2026
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" (
	set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
	goto :found
)

REM Visual Studio 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
	set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
	goto :found
)
 
REM Try using PATH
where msbuild >nul 2>&1
if %errorlevel%==0 (
	set MSBUILD_PATH=msbuild
	goto :found
)

echo ERROR: MSBuild not found!
echo Please install Visual Studio or Visual Studio Build Tools
echo Or open a Developer Command Prompt and run: msbuild RNDR_Sim.sln
echo.
pause
exit /b 1

:found
echo Found MSBuild at: %MSBUILD_PATH%
echo.

REM Set default configuration and platform
set CONFIG=Release
set PLATFORM=x64

REM Parse command line arguments
:parse
if "%~1"=="" goto :build
if /i "%~1"=="debug" set CONFIG=Debug
if /i "%~1"=="release" set CONFIG=Release
if /i "%~1"=="x86" set PLATFORM=Win32
if /i "%~1"=="x64" set PLATFORM=x64
if /i "%~1"=="win32" set PLATFORM=Win32
shift
goto :parse

:build
echo Building Assambler...
echo Configuration: %CONFIG%
echo Platform: %PLATFORM%
echo.

set "CURRENT_DIR=%cd%"
CD "C:\Workplace\ByteFrost\Assembler v2\ByteFrostAssembler"
ECHO CD "C:\Workplace\ByteFrost\Assembler v2\ByteFrostAssembler"
%MSBUILD_PATH% ByteFrostAssembler.sln /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /m /v:minimal

if %errorlevel%==0 (
	echo.
	echo ===================================================
	echo			BUILD SUCCESSFUL!
	echo ===================================================
	echo off
	echo copy %PLATFORM%\%CONFIG%\ByteFrostAssembler.exe C:\Workplace\ByteFrost\bin
	copy .\%PLATFORM%\%CONFIG%\ByteFrostAssembler.exe C:\Workplace\ByteFrost\bin
	echo.
) else (
	echo.
	echo ===================================================
	echo			BUILD FAILED!
	echo ===================================================
	echo Please check the error messages above.
	echo.
)
cd /d "%CURRENT_DIR%"
