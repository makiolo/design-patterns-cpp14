@echo off

pip install pyyaml
if %errorlevel% neq 0 exit /b %errorlevel%

pip install poster
if %errorlevel% neq 0 exit /b %errorlevel%
