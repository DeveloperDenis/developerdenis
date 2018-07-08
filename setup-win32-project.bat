@echo off

if %1.==. goto PROJECT_NAME_ERROR

REM Assumes that this file is being called from the project root directory
mkdir %1
cd %1

mkdir src
mkdir build
mkdir data

copy ..\developerdenis\build.bat src\

git clone https://github.com/DeveloperDenis/developerdenis.git

:PROJECT_NAME_ERROR
echo Error: must specify a project name
goto END

:END