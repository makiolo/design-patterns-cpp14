@echo off

if "%Configuration%" == "Release" (
    set MODE=Release
) else (
    set MODE=Debug
)

echo running in mode %MODE% ...
cd %MODE%
cmake --build . --config %MODE% --target install
set lasterror=%errorlevel%
cd ..
exit /b %lasterror%
