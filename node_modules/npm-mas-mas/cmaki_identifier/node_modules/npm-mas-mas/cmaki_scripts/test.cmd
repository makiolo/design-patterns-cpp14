@echo off

if "%Configuration%" == "Release" (
    set MODE=Release
) else (
    set MODE=Debug
)

echo running in mode %MODE% ...
cd %MODE%
ctest . --no-compress-output --output-on-failure -T Test -C %MODE% -V
set lasterror=%errorlevel%
cd ..

if %lasterror% neq 0 exit /b %lasterror%
