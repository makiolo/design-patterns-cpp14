@echo off

setlocal enableextensions


:: export COMPILER="${COMPILER:-$(conan profile show default | grep -e "\<compiler\>=" | cut -d"=" -f2)}"
:: export COMPILER_VERSION="${COMPILER_VERSION:-$(conan profile show default | grep -e "\<compiler.version\>=" | cut -d"=" -f2)}"

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

if DEFINED GENERATOR (
    echo Using Visual Studio generator: %GENERATOR%
) else (
    set GENERATOR=Visual Studio 16 2019
    echo Env var GENERATOR is not defined. Using by default: %GENERATOR%
)

if "%Configuration%" == "Release" (
    set MODE=Release
) else (
    set MODE=Debug
)

if "%Platform%" == "x86" (
    set ARCH=x86
) else (
    set GENERATOR=%GENERATOR% Win64
    set ARCH=x86_64
)

echo running in mode %COMPILER% %COMPILER_VERSION% %ARCH% %MODE% ...
if exist %MODE% (rmdir /s /q %MODE%)
md %MODE%

:: setup
cd %MODE%

conan install %CMAKI_PWD% --build never -s build_type=%MODE% -s arch=%ARCH% -s arch_build=%ARCH% -s compiler=%COMPILER% -s compiler.version=%COMPILER_VERSION%

IF DEFINED Configuration (
    IF DEFINED Platform (
        cmake %CMAKI_PWD% -DWITH_CONAN=1 -DCMAKE_BUILD_TYPE=%MODE% -G"%GENERATOR%" -DCMAKE_INSTALL_PREFIX=%CMAKI_INSTALL%
    ) ELSE (
        cmake %CMAKI_PWD% -DWITH_CONAN=1 -DCMAKE_BUILD_TYPE=%MODE% -DCMAKE_INSTALL_PREFIX=%CMAKI_INSTALL%
    )
) ELSE (
    cmake %CMAKI_PWD% -DWITH_CONAN=1 -DCMAKE_BUILD_TYPE=%MODE% -DCMAKE_INSTALL_PREFIX=%CMAKI_INSTALL%
)

set lasterror=%errorlevel%
cd %CMAKI_PWD%
exit /b %lasterror%
