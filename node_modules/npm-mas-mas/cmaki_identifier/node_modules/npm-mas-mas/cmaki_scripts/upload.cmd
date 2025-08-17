@echo off

IF DEFINED CMAKI_PWD (
  set CMAKI_PWD=%CMAKI_PWD%
) else (
  set CMAKI_PWD=%CD%
)

IF DEFINED CMAKI_INSTALL (
  set CMAKI_INSTALL=%CMAKI_INSTALL%
) else (
  set CMAKI_INSTALL=%CMAKI_PWD%/bin
)

IF DEFINED MODE (
  set MODE=%MODE%
) else (
  set MODE=Debug
)

set YMLFILE=%CMAKI_PWD%/cmaki.yml

:: warning, TODO: detectar si hay cambios locales y avisar
git diff %CMAKI_PWD%

cd %CMAKI_PWD%/node_modules/cmaki_generator
curl -s https://raw.githubusercontent.com/makiolo/cmaki_scripts/master/make_artifact.cmd > __make_artifact.cmd
call __make_artifact.cmd
del __make_artifact.cmd
