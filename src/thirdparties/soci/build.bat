
set MSBUILD_PATH="C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe"
set VSDEV_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
set CMAKE_PATH="c:\Program Files (x86)\CMake\bin\cmake.exe"
set THIRDPARTIES_DIR=C:\DEV\EXPERIMENTS\THIRDPARTIES
set THIRDPARTIES_SOURCE_DIR=%THIRDPARTIES_DIR%\src
set THIRDPARTIES_BUILD_DIR=%THIRDPARTIES_DIR%\build
set THIRDPARTIES_INSTALL_DIR=%THIRDPARTIES_DIR%\install

set MY_SCRIPT_DIR=%~dp0

set MY_SOURCE_DIR=%THIRDPARTIES_SOURCE_DIR%\soci-3.2.3
set MY_BUILD_DIR=%THIRDPARTIES_BUILD_DIR%\soci-3.2.3
set MY_INSTALL_DIR=%THIRDPARTIES_INSTALL_DIR%\soci-3.2.3

set THIRDPARTY_SQLITE_DIR=%THIRDPARTIES_INSTALL_DIR%\sqlite-3.9.1
set THIRDPARTY_SQLITE_INC_DIR=%THIRDPARTY_SQLITE_DIR%\inc\sqlite3
set THIRDPARTY_SQLITE_LIB_DIR=%THIRDPARTY_SQLITE_DIR%\lib

set THIRDPARTY_BOOST_DIR=C:\DEV\THIRDPARTIES\Nuget\install\boost.1.59.0.1\build\native
set THIRDPARTY_BOOST_INC_DIR=%THIRDPARTY_BOOST_DIR%\inc
set THIRDPARTY_BOOST_LIB_DIR=%THIRDPARTY_BOOST_DIR%\lib\vs2015_x86_d

IF NOT EXIST "%MY_BUILD_DIR%" (
	mkdir "%MY_BUILD_DIR%"
)

cd %MY_BUILD_DIR%

"c:\Program Files (x86)\CMake\bin\cmake.exe" -G "Visual Studio 12 2013" -D "SQLITE_ROOT_DIR=%THIRDPARTY_SQLITE_DIR%" -D "SQLITE3_INCLUDE_DIR=%THIRDPARTY_SQLITE_INC_DIR%" -D "SQLITE3_LIBRARIES=%THIRDPARTY_SQLITE_LIB_DIR%" -D "BOOST_INCLUDE_DIR=%THIRDPARTY_BOOST_INC_DIR%" -D "BOOST_LIBRARIES=%THIRDPARTY_BOOST_LIB_DIR%" -D BOOST_RELEASE_VERSION=1.59.0 "%MY_SOURCE_DIR%" > %MY_SCRIPT_DIR%\build.logs.txt

%MSBUILD_PATH% "core\soci_core_static.vcxproj" "-p:Configuration=Debug" >> "%MY_SCRIPT_DIR%\build.logs.txt"
%MSBUILD_PATH% "core\soci_core.vcxproj" "-p:Configuration=Debug" >> "%MY_SCRIPT_DIR%\build.logs.txt"
%MSBUILD_PATH% "backends\sqlite3\soci_sqlite3_static.vcxproj" "-p:Configuration=Debug" >> "%MY_SCRIPT_DIR%\build.logs.txt"
%MSBUILD_PATH% "backends\sqlite3\soci_sqlite3.vcxproj" "-p:Configuration=Debug" >> "%MY_SCRIPT_DIR%\build.logs.txt"

xcopy "%MY_BUILD_DIR%\lib\Debug\*.lib" "%MY_INSTALL_DIR%\lib\Debug" /I /R /Y
xcopy "%MY_BUILD_DIR%\bin\Debug\*.dll" "%MY_INSTALL_DIR%\bin\Debug" /I /R /Y
xcopy "%MY_SOURCE_DIR%\core\*.h" "%MY_INSTALL_DIR%\inc\soci" /I /R /Y
xcopy "%MY_SOURCE_DIR%\backends\sqlite3\soci-sqlite3.h" "%MY_INSTALL_DIR%\inc\soci" /I /R /Y

cd %MY_SCRIPT_DIR%
