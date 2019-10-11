@echo off

echo Building Module.
echo Dependencies: GIT, PREMAKE5, VISUAL STUDIO 2015/2013.
echo.

:: Hacky way to ensure we've got a git repository & submodules at our disposal
git init
git submodule update --init --recursive
echo.

:: Fire up a Visual Studio command-line environment and generate our solution
IF DEFINED VS140COMNTOOLS (
	echo Preparing for build with Visual Studio 2015.
	premake5 vs2015
	CALL "%VS140COMNTOOLS%VSDevCmd.bat"
) ELSE IF DEFINED VS120COMNTOOLS (
	echo Preparing for build with Visual Studio 2013.
	premake5 vs2013
	CALL "%VS120COMNTOOLS%VSDevCmd.bat"
) ELSE (
	echo Failed to detect Visual Studio installation.
	timeout 5
	exit /B 1
)

echo.

for /R project %%X in (*.sln) do (
	msbuild %%X
)

:: We're done!
timeout 5

exit /B %ERRORLEVEL%
