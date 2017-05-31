#ifndef DENIS_WIN32_H_
#define DENIS_WIN32_H_

#include "denis_types.h"

#define NOMINMAX
#define Rectangle _Rectangle
#include <windows.h>
#undef Rectangle

#define HEAP_ALLOC(size) VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE)
#define HEAP_FREE(pointer) VirtualFree(pointer, 0, MEM_RELEASE);

static char* showFileDialog(char *descriptionOfFile, char *fileExtension, bool requestSave)
{
    char *result = 0;

    const uint32 fileNameSize = 512;
    char *fileNameBuffer = (char*)HEAP_ALLOC(fileNameSize);
    
    OPENFILENAME openFileName = {};
    openFileName.lStructSize = sizeof(OPENFILENAME);

    char filter[fileNameSize] = {};
    uint32 stringIndex;
    for (stringIndex = 0; descriptionOfFile[stringIndex] != 0; ++stringIndex)
    {
	filter[stringIndex] = descriptionOfFile[stringIndex];
    }

    ++stringIndex;
    filter[stringIndex++] = '*';
    filter[stringIndex++] = '.';
    for (uint32 i = 0; fileExtension[i] != 0; ++i)
    {
	filter[stringIndex++] = fileExtension[i];
    }
    
    openFileName.lpstrFilter = filter;
    openFileName.lpstrFile = fileNameBuffer;
    openFileName.nMaxFile = fileNameSize;
    openFileName.lpstrDefExt = fileExtension;

    if (requestSave)
    {
	openFileName.Flags = OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&openFileName) != 0)
	{
	    result = fileNameBuffer;
	}
	else
	    HEAP_FREE(fileNameBuffer);
    }
    else
    {
	openFileName.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&openFileName) != 0)
	{
	    result = fileNameBuffer;
	}
	else
	    HEAP_FREE(fileNameBuffer);
    }
    
    return result;
}

#endif
