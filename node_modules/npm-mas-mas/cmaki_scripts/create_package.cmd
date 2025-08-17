@echo off

if DEFINED COMPILER (
	echo Using COMPILER: %COMPILER%
) else (
	set COMPILER="Visual Studio"
	echo Env var COMPILER is not defined. Using by default: %COMPILER%
)

if DEFINED COMPILER_VERSION (
	echo Using COMPILER_VERSION: %COMPILER_VERSION%
) else (
	set COMPILER_VERSION=16
	echo Env var COMPILER_VERSION is not defined. Using by default: %COMPILER_VERSION%
)

if "%Configuration%" == "Release" (
	set MODE=Release
) else (
	set MODE=Debug
)

if "%NPP_CI%" == "FALSE" (
	conan install . --build missing -s compiler=%COMPILER% -s build_type=%MODE% -s compiler.version=%COMPILER_VERSION%
)

conan create . npm-mas-mas/testing -s compiler=%COMPILER% -s build_type=%MODE% -s compiler.version=%COMPILER_VERSION% -tf None

