@echo off

echo [0/3] preinstall
set PATH=%CMAKI_PWD%\node_modules\cmaki_scripts;%PATH%
env | sort

powershell -c "$source = 'https://raw.githubusercontent.com/makiolo/npm-mas-mas/master/cmaki_scripts/cmaki_depends.cmd'; $dest = $env:TEMP + '\bootstrap.cmd'; $WebClient = New-Object System.Net.WebClient; $WebClient.DownloadFile($source,$dest); Invoke-Expression $dest"
if %errorlevel% neq 0 exit /b %errorlevel%

if exist package.json (
  
  echo [1/3] prepare
  :: call ncu -u
  npm cache clean --force
  
  echo [2/3] compile
  npm install
  if %errorlevel% neq 0 exit /b %errorlevel%
  
  echo [3/3] run tests
  npm test
  if %errorlevel% neq 0 exit /b %errorlevel%

) else (

  echo [1/3] prepare
  if exist node_modules\cmaki_scripts (rmdir /s /q node_modules\cmaki_scripts)
  powershell -c "$source = 'https://raw.githubusercontent.com/makiolo/npm-mas-mas/master/cmaki_scripts/bootstrap.cmd'; $dest = $env:TEMP + '\bootstrap.cmd'; $WebClient = New-Object System.Net.WebClient; $WebClient.DownloadFile($source,$dest); Invoke-Expression $dest"
  if %errorlevel% neq 0 exit /b %errorlevel%

  echo [2/3] compile
  call node_modules\cmaki_scripts\install.cmd
  if %errorlevel% neq 0 exit /b %errorlevel%

  echo [3/3] run tests
  call node_modules\cmaki_scripts\test.cmd
  if %errorlevel% neq 0 exit /b %errorlevel%

)

