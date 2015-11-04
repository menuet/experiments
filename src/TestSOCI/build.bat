
set SCRIPT_DIR=%~dp0
set OUTPUT_DIR=%SCRIPT_DIR%output
set BUILD_PROP_THIRDPARTIES_INSTALL_DIR=C:/DEV/EXPERIMENTS/THIRDPARTIES/install

IF NOT EXIST %OUTPUT_DIR% (
	mkdir %OUTPUT_DIR%
)

cd %OUTPUT_DIR%

"c:\Program Files (x86)\CMake\bin\cmake.exe" -G "Visual Studio 12 2013" .. -DBUILD_PROP_THIRDPARTIES_INSTALL_DIR=%BUILD_PROP_THIRDPARTIES_INSTALL_DIR% > %SCRIPT_DIR%\build.logs.txt

cd %SCRIPT_DIR%
