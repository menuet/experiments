
set MSBUILD_PATH="C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe"
set VSDEV_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
set CMAKE_PATH="c:\Program Files (x86)\CMake\bin\cmake.exe"
set THIRDPARTIES_DIR=C:\DEV\EXPERIMENTS\THIRDPARTIES
set THIRDPARTIES_SOURCE_DIR=%THIRDPARTIES_DIR%\src
set THIRDPARTIES_BUILD_DIR=%THIRDPARTIES_DIR%\build
set THIRDPARTIES_INSTALL_DIR=%THIRDPARTIES_DIR%\install

set MY_SCRIPT_DIR=%~dp0

set MY_SOURCE_DIR=%THIRDPARTIES_SOURCE_DIR%\sqlite-amalgamation-3090100
set MY_BUILD_DIR=%THIRDPARTIES_BUILD_DIR%\sqlite-amalgamation-3090100
set MY_INSTALL_DIR=%THIRDPARTIES_INSTALL_DIR%\sqlite-3.9.1

IF NOT EXIST "%MY_BUILD_DIR%" (
	mkdir "%MY_BUILD_DIR%"
)

cd %MY_BUILD_DIR%

%CMAKE_PATH% -G "Visual Studio 12 2013" -D "BUILD_PROP_SOURCE_DIR=%MY_SOURCE_DIR%" -D "BUILD_PROP_INSTALL_DIR=%MY_INSTALL_DIR%" "%MY_SCRIPT_DIR%" > "%MY_SCRIPT_DIR%\build.logs.txt"

%MSBUILD_PATH% "sqlite3-build.vcxproj" >> "%MY_SCRIPT_DIR%\build.logs.txt"
%MSBUILD_PATH% "sqlite3-install.vcxproj" >> "%MY_SCRIPT_DIR%\build.logs.txt"

cd %MY_SCRIPT_DIR%
