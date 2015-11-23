
@echo off

set "SOURCE_DIR=%~dp0"
set "SOURCE_DIR=%SOURCE_DIR:~0,-1%"
set "LOG_FILE=%SOURCE_DIR%\build.logs.txt"
set "OUTPUT_DIR=%SOURCE_DIR%_output"
set "BUILD_DIR=%OUTPUT_DIR%\build"
set "INSTALL_DIR=%OUTPUT_DIR%\install"
set "CMAKE_EXE=C:\Program Files (x86)\CMake\bin\cmake.exe"

call :FUNC_LOG "Starting" "%CLEAR_LOGS%"

call :FUNC_LOG_APPEND "SOURCE_DIR=%SOURCE_DIR%"
call :FUNC_LOG_APPEND "LOG_FILE=%LOG_FILE%"
call :FUNC_LOG_APPEND "OUTPUT_DIR=%OUTPUT_DIR%"
call :FUNC_LOG_APPEND "BUILD_DIR=%BUILD_DIR%"
call :FUNC_LOG_APPEND "INSTALL_DIR=%INSTALL_DIR%"
call :FUNC_LOG_APPEND "CMAKE_EXE=%CMAKE_EXE%"

call :FUNC_LOG_APPEND "Checking build directory %BUILD_DIR%"
if exist "%BUILD_DIR%" (
	call :FUNC_LOG_APPEND "Directory %BUILD_DIR% already exists"
) else (
	call :FUNC_LOG_APPEND "Creating directory %BUILD_DIR%"
	mkdir "%BUILD_DIR%"
)

call :FUNC_LOG_APPEND "Setting current directory to %BUILD_DIR%"
cd "%BUILD_DIR%"

call :FUNC_LOG_APPEND "Generating Visual Studio 2013 files in directory %BUILD_DIR%"
"%CMAKE_EXE%" -G "Visual Studio 12 2013" -D "CMAKE_INSTALL_PREFIX=%INSTALL_DIR%" "%SOURCE_DIR%\" >> "%LOG_FILE%" 2>&1

call :FUNC_LOG_APPEND ""
call :FUNC_LOG_APPEND "Building targets %BUILD_TARGETS%"
call :FUNC_LOG_APPEND ""

for %%G in (%BUILD_TARGETS%) do (
	call :FUNC_LOG_APPEND ""
	call :FUNC_LOG_APPEND ""
	call :FUNC_LOG_APPEND "***********************"
	call :FUNC_LOG_APPEND "Building target"
	call :FUNC_LOG_APPEND "***********************"
	call :FUNC_LOG_APPEND ""
	"%CMAKE_EXE%" --build "%BUILD_DIR%" --target %%G --config Release >> "%LOG_FILE%" 2>&1
	if errorlevel 1 goto :ERR
)

cd "%SOURCE_DIR%\"
exit /B 0
goto :EOF


:ERR
echo "An error occured while building ! See log file %LOG_FILE%"
cd "%SOURCE_DIR%\"
if exist "C:\Program Files (x86)\Notepad++\notepad++.exe" (
	"C:\Program Files (x86)\Notepad++\notepad++.exe" "%LOG_FILE%"
) else (
	notepad "%LOG_FILE%"
)
exit /B 1
goto :EOF


:FUNC_LOG_APPEND
call :FUNC_LOG "%~1" "false"
goto :EOF

:FUNC_LOG
if "%~2" == "true" (
	echo "*** %~1" > %LOG_FILE% 2>&1
) else (
	echo "*** %~1" >> %LOG_FILE% 2>&1
)
goto :EOF
