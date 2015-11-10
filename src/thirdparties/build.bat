
set "CMAKE_PATH=c:\Program Files (x86)\CMake\bin\cmake.exe"
set "THIRDPARTIES_DIR=%~dp0..\..\..\THIRDPARTIES"
set "BUILD_DIR=%THIRDPARTIES_DIR%\build"
set "SCRIPT_DIR=%~dp0"

IF NOT EXIST "%BUILD_DIR%" (
	mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

"%CMAKE_PATH%" -G "Visual Studio 12 2013" "%SCRIPT_DIR%" > "%SCRIPT_DIR%\build.logs.txt"
"%CMAKE_PATH%" --build . --target khewil-thirdparties-build-install >> "%SCRIPT_DIR%\build.logs.txt"

cd "%SCRIPT_DIR%"
