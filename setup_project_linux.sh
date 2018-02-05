#!/bin/bash

echo "Creating project directories..."
mkdir ../code/ 2> /dev/null
mkdir ../build/ 2> /dev/null
mkdir ../data/ 2> /dev/null

echo "Copying build script..."
cp build.sh ../code/

echo "Creating main.cpp and main.h files..."

echo -e "#include \"platform_layer.h\"

exportDLL MAIN_UPDATE_CALL(mainUpdateCall)
{
\t
}" > ../code/main.cpp

echo -e "#if !defined(MAIN_H_)
#define MAIN_H_

struct STATIC_SETTINGS
{
\tstatic const char* WINDOW_TITLE;
\tstatic const uint32 WINDOW_WIDTH;
\tstatic const uint32 WINDOW_HEIGHT;
\tstatic const bool WINDOW_RESIZABLE;

\tstatic const char* DLL_FILE_NAME;
\tstatic const uint32 FPS_TARGET;
};

const char* STATIC_SETTINGS::WINDOW_TITLE = 0;
const uint32 STATIC_SETTINGS::WINDOW_WIDTH = 0;
const uint32 STATIC_SETTINGS::WINDOW_HEIGHT = 0;
const bool STATIC_SETTINGS::WINDOW_RESIZABLE = false;

const char* STATIC_SETTINGS::DLL_FILE_NAME = 0;
const uint32 STATIC_SETTINGS::FPS_TARGET = 60;

struct Memory
{
\tbool isInit;
};

#endif" > ../code/main.h

echo "Set up complete!"
