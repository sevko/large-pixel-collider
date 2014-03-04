#include <stdio.h>
#include <stdlib.h>

#include "src/globals.h"
#include "src/matrix.h"
#include "src/screen.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/interpreter.h"

#define MAX_SCRIPT_LINE_LENGTH 100      // hard-coded, but a reasonable limit
#define COMMENT_CHAR '#'

// read, and return the contents of filePath in a char **
Script_t * readScriptFile(char * filePath){
	FILE * file = fopen(filePath, "r");
	if(file == NULL){
		ERROR("Failed to open file \"%s\"", filePath);
		exit(EXIT_FAILURE);
	}

	char ** fileBuffer = malloc(sizeof(char *));
	fileBuffer[0] = NULL;
	int line = 0;

	char * lineBuffer = malloc(MAX_SCRIPT_LINE_LENGTH);
	while(fgets(lineBuffer, MAX_SCRIPT_LINE_LENGTH, file) != NULL){
		if(!(lineBuffer[0] == COMMENT_CHAR || lineBuffer[0] == '\n')){
			fileBuffer[line++] = lineBuffer;
			fileBuffer = realloc(fileBuffer, (line + 1) * sizeof(char *));
			lineBuffer = malloc(MAX_SCRIPT_LINE_LENGTH);
		}
	}
	free(lineBuffer);

	Script_t * script = malloc(sizeof(Script_t));
	script->script = fileBuffer;
	script->numLines = line;

	fclose(file);
	return script;
}

// evaluate each of the commands in the Script_t
void evaluateScript(Script_t * script){
	configureScreen();
	Matrix_t * points = createMatrix(), * transform = createIdentity();

	int line;
	for(line = 0; line < script->numLines; line++){
		int status = evaluateCommand(&script->script[line], points, &transform);
		if(status != VALID_EVAL){
			switch(status){
				case INVALID_CMD:
					ERROR("Error on line %d:\n\t%sInvalid command.",
						line + 1, script->script[line]);
					break;

				case INVALID_ARGS:
					ERROR("Error on line %d:\n\t%sInvalid argument.",\
						line + 2, script->script[line + 1]);
					break;
			}
			ERROR("Exiting.");
			break;
		}

		if(argsRequired(script->script[line][0]))
			line++;
	}

	freeScript(script);
	freeMatrices(2, points, transform);
	quitScreen();
}

// deallocate a Script_t and all internal pointers
void freeScript(Script_t * script){
	int line;
	for(line = 0; line < script->numLines; line++)
		free(script->script[line]);
	free(script->script);
	free(script);
}
