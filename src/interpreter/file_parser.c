#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/utils.h"
#include "src/interpreter/stack/stack.h"

/*!
 * Microseconds to wait before the file parser exits after successful script
 * evaluation.
 */
#define PARSER_EXIT_PAUSE 5e6

void readScriptFile(const char * const filePath){
	ScannedFile_t * script = readFile(filePath);
	configureScreen();
	Matrix_t * points = createMatrix(), * transform = createIdentity();
	Stack_t * coordStack = createStack();

	int line;
	for(line = 0; line < script->numLines; line++){
		int status = evaluateCommand(&script->buffer[line], &points,
			&transform, coordStack);
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

	usleep(PARSER_EXIT_PAUSE);
	freeMatrices(2, points, transform);
	quitScreen();
	freeFile(script);
}
