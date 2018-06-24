#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"

#include "denis_strings.h"

//TODO(denis): Have this able to create the default project_settings.h if a settings file is not given

/* Usage: read_project_settings <settings file name>

   [INSERT DESCRIPTION OF PROGRAM HERE]
*/

#define DEFAULT_TITLE "Default Title"
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_RESIZABLE false
#define DEFAULT_DLL_NAME "main.dll"
#define DEFAULT_FPS_TARGET 60

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Error: must pass a project settings file to read_project_settings\n");
		return 1;
	}
	
	char* filename = argv[1];
	FILE* settingsFile = fopen(filename, "r");
	if (!settingsFile)
	{
		printf("Error: could not open %s\n", filename);
		return 1;
	}

	char* windowTitle = 0;
    u32 windowWidth = 0;
	u32 windowHeight = 0;
	bool windowResizable = false;
	char* dllFileName = 0;
	u32 fpsTarget = 0;

	char line[500] = {};
	while (fgets(line, 500, settingsFile))
	{
		char** tokens = tokenizeStringInPlace(line, 2, '=');
		
		if (stringsEqualIgnoreCase(tokens[0], "WINDOW_TITLE"))
		{
			windowTitle = duplicateString(tokens[1]);
			windowTitle = trimString(windowTitle);
		}
		else if (stringsEqualIgnoreCase(tokens[0], "WINDOW_WIDTH"))
		{
			windowWidth = parseU32String(tokens[1]);
		}
		else if (stringsEqualIgnoreCase(tokens[0], "WINDOW_HEIGHT"))
		{
			windowHeight = parseU32String(tokens[1]);
		}
		else if (stringsEqualIgnoreCase(tokens[0], "WINDOW_RESIZABLE"))
		{
			windowResizable = stringsEqualIgnoreCase(tokens[1], "true") ? true : false;
		}
		else if (stringsEqualIgnoreCase(tokens[0], "DLL_FILE_NAME"))
		{
			dllFileName = duplicateString(tokens[1]);
			dllFileName = trimString(dllFileName);
		}
		else if (stringsEqualIgnoreCase(tokens[0], "FPS_TARGET"))
		{
			fpsTarget = parseU32String(tokens[1]);
		}
		
		free(tokens);
	}

	if (!windowTitle)
		windowTitle = DEFAULT_TITLE;
	if (!windowWidth)
		windowWidth = DEFAULT_WIDTH;
	if (!windowHeight)
		windowHeight = DEFAULT_HEIGHT;
	if (!dllFileName)
		dllFileName = DEFAULT_DLL_NAME;
	if (!fpsTarget)
		fpsTarget = DEFAULT_FPS_TARGET;
	
	fclose(settingsFile);
	settingsFile = 0;
	
	FILE* outputFile = fopen("project_settings.h", "w");
	if (!outputFile)
	{
		printf("Error: cannot create/open %s\n", "project_settings.h");
		return 1;
	}

	fprintf(outputFile, "// THIS IS AN AUTO-GENERATED FILE, MODIFICATIONS IN THIS FILE WILL NOT BE PRESERVED\n\n");
	fprintf(outputFile, "#if !defined(PROJECT_SETTINGS_H_)\n");
	fprintf(outputFile, "#define PROJECT_SETTINGS_H_\n\n");

	fprintf(outputFile, "char* WINDOW_TITLE = \"%s\";\n", windowTitle);
	fprintf(outputFile, "u32 WINDOW_WIDTH = %d;\n", windowWidth);
	fprintf(outputFile, "u32 WINDOW_HEIGHT = %d;\n", windowHeight);
	fprintf(outputFile, "bool WINDOW_RESIZABLE = %d;\n", windowResizable);
	fprintf(outputFile, "char* DLL_FILE_NAME = \"%s\";\n", dllFileName);
	fprintf(outputFile, "u32 FPS_TARGET = %d;\n", fpsTarget);

	fprintf(outputFile, "\n#endif\n");

	fclose(outputFile);
	
	return 0;
}
