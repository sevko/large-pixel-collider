/*
 *  file_parser.c contains functions for reading and evaluating a script file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/screen.h"
#include "src/utils.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/interpreter.h"

typedef struct {
	char ** script;
	int numLines;
} Script_t;

static void evaluateScript(const Script_t * const script);
static void freeScript(Script_t * const script);

// read, and evaluate the contents of filePath in a char **
void readScriptFile(const char * const filePath){
	ScannedFile_t * file = readFile(filePath);
	Script_t * script = malloc(sizeof(Script_t));
	script->script = file->buffer;
	script->numLines = file->numLines;
	free(file);

	evaluateScript(script);
	freeScript(script);
}

// evaluate each of the commands in the Script_t
static void evaluateScript(const Script_t * const script){
	configureScreen();
	Matrix_t * points = createMatrix(), * transform = createIdentity();

	int line;
	for(line = 0; line < script->numLines; line++){
		int status = evaluateCommand(&script->script[line], points, &transform);
		if(status != CMD_VALID_EVAL){
			switch(status){
				case CMD_INVALID_CMD:
					ERROR("Error on line %d:\n\t%sInvalid command.",
						line + 1, script->script[line]);
					break;

				case CMD_INVALID_ARGS:
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

	usleep(5e6);
	freeMatrices(2, points, transform);
	quitScreen();
}

// deallocate a Script_t and all internal pointers
static void freeScript(Script_t * const script){
	int line;
	for(line = 0; line < script->numLines; line++)
		free(script->script[line]);
	free(script->script);
	free(script);
}
