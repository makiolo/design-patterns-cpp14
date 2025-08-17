@ECHO OFF
SET DIRWORK=%~dp0

IF EXIST "%PYTHON%" (
	rem ok
) ELSE (
	set PYTHON=python
)

SET PATH=%~dp0\bin;%PATH%
"%PYTHON%" %DIRWORK%\build.py %*
