#ifndef DENIS_WIN32_H_
#define DENIS_WIN32_H_

#include "denis_types.h"

#include "denis_strings.h"

//NOTE(denis): user must free the returned string
static char* showFileDialog(char *descriptionOfFile, char *fileExtension, bool requestSave)
{
    char *result = 0;

    const u32 fileNameSize = 512;
    char *fileNameBuffer = (char*)HEAP_ALLOC(fileNameSize);
    
    OPENFILENAME openFileName = {};
    openFileName.lStructSize = sizeof(OPENFILENAME);

    char filter[fileNameSize] = {};
    u32 stringIndex;
    for (stringIndex = 0; descriptionOfFile[stringIndex] != 0; ++stringIndex)
    {
		filter[stringIndex] = descriptionOfFile[stringIndex];
    }

    ++stringIndex;
    filter[stringIndex++] = '*';
    filter[stringIndex++] = '.';
    for (u32 i = 0; fileExtension[i] != 0; ++i)
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

//NOTE(denis): user must free the returned string
static char* getProgramPathName()
{
    char *result = 0;
	
    TCHAR fileNameBuffer[MAX_PATH+1];
    DWORD getFileNameResult = GetModuleFileName(NULL, fileNameBuffer, MAX_PATH+1);
    if (getFileNameResult != 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
		char filePath[MAX_PATH+1] = {};
		u32 indexOfLastSlash = 0;
		for (int i = 0; i < MAX_PATH && fileNameBuffer[i] != 0; ++i)
		{
			if (fileNameBuffer[i] == '\\')
				indexOfLastSlash = i;
		}
 
		copyIntoString(filePath, fileNameBuffer, 0, indexOfLastSlash);
 
		result = duplicateString(filePath);
    }
    else
    {
		//TODO(denis): try again with a bigger buffer?
    }
 
    return result;
}

#endif
