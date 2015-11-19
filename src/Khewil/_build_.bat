
@echo off

set "SOURCE_DIR=%~dp0"
set "BUILD_DIR=%SOURCE_DIR%build"
set "CMAKE_EXE=C:\Program Files (x86)\CMake\bin\cmake.exe"

if "%CLEAR_LOGS%" == "true" (
	echo Start building %BUILD_PROJECTS% > "%SOURCE_DIR%build.logs.txt" 2>&1
) else (
	echo Start building %BUILD_PROJECTS% >> "%SOURCE_DIR%build.logs.txt" 2>&1
)

if exist "%BUILD_DIR%" (
	echo "Directory %BUILD_DIR% already exists" >> "%SOURCE_DIR%build.logs.txt" 2>&1
) else (
	echo "Creating directory %BUILD_DIR%" >> "%SOURCE_DIR%build.logs.txt" 2>&1
	mkdir %BUILD_DIR%
)

echo Setting current directory to: %BUILD_DIR% >> "%SOURCE_DIR%build.logs.txt" 2>&1
cd %BUILD_DIR%

echo Generating Visual Studio 2013 files in directory %BUILD_DIR% >> "%SOURCE_DIR%build.logs.txt" 2>&1
"%CMAKE_EXE%" "-GVisual Studio 12 2013" "%SOURCE_DIR%" >> "%SOURCE_DIR%build.logs.txt" 2>&1

set BUILD_PROJECTS=%BUILD_PROJECTS:"=%

for %%G in (%BUILD_PROJECTS%) do (
	echo Building project: %%G >> build.logs.txt 2>&1
	"%CMAKE_EXE%" --build "%BUILD_DIR%" --target %%G --config Release >> "%SOURCE_DIR%build.logs.txt" 2>&1
	if errorlevel 1 goto :ERR
)

cd "%SOURCE_DIR%"
exit /B 0
goto :EOF


:ERR
echo An error occured while building >> "%SOURCE_DIR%build.logs.txt" 2>&1
cd "%SOURCE_DIR%"
notepad "%SOURCE_DIR%build.logs.txt"
exit /B 1
goto :EOF
