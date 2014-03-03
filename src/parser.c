#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "parser.h"

#define MAX_SCRIPT_LINE_LENGTH 100      // hard-coded, but a reasonable limit

// read, and return the contents of filePath in a char **
Script_t * readScriptFile(char * filePath){
	char ** fileBuffer = malloc(sizeof(char *));
	fileBuffer[0] = NULL;
	int line = 0;

	FILE * file = fopen(filePath, "r");
	if(file == NULL){
		ERROR("Failed to open file \"%s\"", filePath);
		exit(EXIT_FAILURE);
	}

	char * lineBuffer = malloc(MAX_SCRIPT_LINE_LENGTH);
	while(fgets(lineBuffer, MAX_SCRIPT_LINE_LENGTH, file) != NULL){
		fileBuffer[line++] = lineBuffer;
		fileBuffer = realloc(fileBuffer, (line + 1) * sizeof(char *));
		lineBuffer = malloc(MAX_SCRIPT_LINE_LENGTH);
	}
	free(lineBuffer);

	Script_t * script = malloc(sizeof(Script_t));
	script->script = fileBuffer;
	script->numLines = line;

	fclose(file);
	return script;
}

void freeScript(Script_t * script){
	int line;
	for(line = 0; line < script->numLines; line++)
		free(script->script[line]);
	free(script->script);
	free(script);
}
