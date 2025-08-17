@echo off
if exist "boostorg_predef" (
	rmdir /s /q boostorg_predef
)
git clone -q https://github.com/boostorg/predef.git boostorg_predef

..\cmaki_scripts\setup.cmd
