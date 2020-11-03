@echo on

if %1.==. goto PROJECT_NAME_ERROR

REM Assumes that this file is being called from the project root directory
mkdir %1
cd %1

mkdir src
mkdir build
mkdir data

copy ..\developerdenis\build_template.bat src\build.bat

git clone https://github.com/DeveloperDenis/developerdenis.git

goto END

:PROJECT_NAME_ERROR
echo Error: must specify a project name
goto END

:END
