@echo off

set dll_file_name="test.dll"
set exe_file_name="test.exe"
set pdb_file_name="test"

set hh=%time:~0,2%
if "%time:~0,1%"==" " set hh=0%hh:~1,1%

set dt=%date:~7,2%-%date:~4,2%-%date:~10,4%_%hh%_%time:~3,2%_%time:~6,2%

set denis_library=..\developerdenis\

set flags=/nologo /DDEBUG /DDENIS_WIN32 /Gm- /GR- /EHa- /Zi /FC /W4 /WX /wd4100 /wd4505 /wd4189 /wd4201
set linker_flags=/incremental:no
set includes=/I %denis_library% /I ../code/
set files=..\code\main.cpp

pushd ..\build\

del *.pdb > NUL 2> NUL

REM build main exe
cl %flags% %includes% %denis_library%\win32_layer.cpp /Fe%exe_file_name% /link %linker_flags% user32.lib Gdi32.lib Shcore.lib

REM build DLL
echo FOO > pdb.lock
cl %flags% %includes% /LD %files% /Fe%dll_file_name% /link /PDB:%pdb_file_name%_%dt%.pdb %linker_flags%
del pdb.lock

popd
