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

/*!
 *  @brief Evaluate the commands contained in a ::ScannedFile_t.
 *
 *  @param script The ::ScannedFile_t whose commands will be executed.
 */
static void evaluateScript(const ScannedFile_t * const script);

void readScriptFile(const char * const filePath){
	ScannedFile_t * script = readFile(filePath);
	evaluateScript(script);
	freeFile(script);
}

static void evaluateScript(const ScannedFile_t * const script){
	configureScreen();
	Matrix_t * points = createMatrix(), * transform = createIdentity();

	int line;
	for(line = 0; line < script->numLines; line++){
		int status = evaluateCommand(&script->buffer[line], &points,
			&transform);
		if(status != CMD_VALID_EVAL){
			switch(status){
				case CMD_INVALID_CMD:
					ERROR("Error on line %d:\n\t%sInvalid command.",
						line + 1, script->buffer[line]);
					break;

				case CMD_INVALID_ARGS:
					ERROR("Error on line %d:\n\t%sInvalid argument.",\
						line + 2, script->buffer[line + 1]);
					break;
			}
			ERROR("Exiting.");
			break;
		}

		if(argsRequired(script->buffer[line][0]))
			line++;
	}

	usleep(5e6);
	freeMatrices(2, points, transform);
	quitScreen();
}
