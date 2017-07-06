//----------------------------------------------------
//    denis_strings.h
// Written by Denis Levesque
//----------------------------------------------------

#ifndef DENIS_STRINGS_H_
#define DENIS_STRINGS_H_

#include "denis_types.h"

#define LOWER_CASE(c) ((c) >= 'a' && (c) <= 'z')
#define UPPER_CASE(c) ((c) >= 'A' && (c) <= 'Z')

inline bool charInString(char c, char* string)
{
    bool result = false;

    if (string)
    {
		for (int i = 0; string[i] != 0 && !result; ++i)
		{
			if (string[i] == c)
				result = true;
		}
    }
    
    return result;
}

//NOTE(denis): can be used if the strings are not valid C strings
static inline bool charArraysEqual(char* array1, int size1, char* array2, int size2)
{
    bool areEqual = TRUE;
    int arraySize = size1 < size2 ? size1 : size2;

    int i;
    for (i = 0; i < arraySize && areEqual; ++i)
    {
		if (array1[i] != array2[i])
			areEqual = FALSE;
    }

    return areEqual;
}

//NOTE(denis): assumes that string1 and string2 are valid C strings
static inline bool stringsEqual(char* string1, char* string2)
{
    bool areEqual = TRUE;
    
    int i = 0;
    while (string1[i] != 0 && string2[i] != 0 && areEqual)
    {
		if (string1[i] != string2[i])
			areEqual = FALSE;

		++i;
    }

    if (string1[i] != string2[i])
		areEqual = FALSE;
    
    return areEqual;
}

static inline bool stringsEqualIgnoreCase(char* string1, char* string2)
{
    bool areEqual = TRUE;

    
    int i = 0;
    while (string1[i] != 0 && string2[i] != 0 && areEqual)
    {
		if (string1[i] != string2[i])
		{
			if (LOWER_CASE(string1[i]) && UPPER_CASE(string2[i]))
			{
				areEqual = (string1[i] - 'a') == (string2[i] - 'A');
			}
			else if (UPPER_CASE(string1[i]) && LOWER_CASE(string2[i]))
			{
				areEqual = (string1[i] - 'A') == (string2[i] - 'a');
			}
			else
				areEqual = FALSE;
		}

		++i;
    }

    if ((string1[i] == 0 && string2[i] != 0) ||
		(string1[i] != 0 && string2[i] == 0))
    {
		areEqual = FALSE;
    }
    
    return areEqual;
}

//NOTE(denis): returns a pointer that is equal to string, > than string
// by the number non-blank spaces in the beginning, or NULL if the string contains
// only blanks
static char* trimString(char* string)
{
    char* result = string;
    
    if (string)
    {
		int i = 0;
		while (string[i] != 0 && (string[i] == ' ' || string[i] == '\n'))
			++i;
	
		if (string[i] != 0)
		{
			result = string + i;

			int lastNonBlankIndex = i;
			while (string[i] != 0)
			{
				if (string[i] != ' ' && string[i] != '\n')
					lastNonBlankIndex = i;

				++i;
			}
	    
			if (lastNonBlankIndex < i - 1)
				string[lastNonBlankIndex+1] = 0;
		}
		else
			result = NULL;
    }

    return result;
}

static inline int getStringSize(char* string)
{
    int result = 0;

    while (string && string[result] != 0)
    {
		++result;
    }
    
    return result;
}

//NOTE(denis): string returned from this function must be freed by the user
static inline char* createStringFromArray(char* array, int size)
{
    char* result = NULL;
 
    if (array)
    {
		result = (char*)HEAP_ALLOC(size+1);
	
		int i;
		for (i = 0; i < size; ++i)
		{
			result[i] = array[i];
		}
		result[size] = 0;
    }
    return result;
}

static inline char* duplicateString(char *string)
{
    char *result = 0;
    
    if (string)
    {
		uint32 numChars = getStringSize(string);

		result = (char*)HEAP_ALLOC(numChars+1);

		for (uint32 i = 0; i < numChars; ++i)
		{
			result[i] = string[i];
		}
    }

    return result;
}

//NOTE(denis): user must free result of this function
static char** tokenizeStringInPlace(char* string, int maxTokens, char separator)
{
    char** tokenArray = (char**)HEAP_ALLOC(maxTokens*sizeof(char*));
    
    int i;
    for (i = 0; i < maxTokens; ++i)
		tokenArray[i] = 0;

    int charIndex = 0;
    for (i = 0; i < maxTokens; ++i)
    {
		while(string[charIndex] != 0 && string[charIndex] == separator)
			++charIndex;

		if (string[charIndex] == 0)
			break;

		tokenArray[i] = string + charIndex;
		++charIndex;

		while(string[charIndex] != 0 && string[charIndex] != separator)
			++charIndex;

		if (string[charIndex] != 0)
			string[charIndex++] = 0;
		else
			break;
    }

    return tokenArray;
}

 
//NOTE(denis): assumes that destination is big enough to hold all of source's
// characters
static inline void copyIntoString(char *destination, char *source)
{
    if (destination && source)
    {
		for (uint32 i = 0; source[i] != 0; ++i)
		{
			destination[i] = source[i];
		}
    }
}
 
//NOTE(denis): copies a portion of source into destination,
// the first character copied is index = beginning
// and the last character copied is at index = end
static inline void copyIntoString(char *destination, char *source,
								  uint32 beginning, uint32 end)
{
    uint32 destinationIndex = 0;
     
    if (destination && source && beginning < end)
    {
		for (uint32 i = beginning; i <= end; ++i)
		{
			destination[destinationIndex++] = source[i];
		}
    }
}
 
//NOTE(denis): returns a new string which is a+b
static char* concatStrings(char *a, char *b)
{
    uint32 sizeOfA = 0;
    uint32 sizeOfB = 0;
    char *result = 0;
 
    if (a && b)
    {
		for (uint32 i = 0; a[i] != 0; ++i)
		{
			++sizeOfA;
		}
 
		for (uint32 i = 0; b[i] != 0; ++i)
		{
			++sizeOfB;
		}
 
		result = (char*)HEAP_ALLOC(sizeOfA+sizeOfB+1);
 
		copyIntoString(result, a);
		copyIntoString(result+sizeOfA, b);
    }
     
    return result;
}

#endif
