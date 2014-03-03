#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "matrix.h"
#include "parser.h"
#include "screen.h"

#define MAX_SCRIPT_LINE_LENGTH 100      // hard-coded, but a reasonable limit

// status of evaluation of a single command
#define VALID_EVAL 0
#define INVALID_CMD 1
#define INVALID_ARGS 2

// chars indicating commands in a script file
#define COMMENT_CHAR '#'
#define ADD_LINE 'l'
#define SET_IDENTITY 'i'
#define CREATE_SCALE 's'
#define CREATE_TRANSLATION 't'
#define CREATE_ROT_X 'x'
#define CREATE_ROT_Y 'y'
#define CREATE_ROT_Z 'z'
#define APPLY_TRANSFORM 'a'
#define DRAW_FRAME 'v'
#define SAVE_FRAME 'g'

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
		if(lineBuffer[0] != COMMENT_CHAR)
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

// evaluate the command located at command[0], accounting for the arguments,
// if any are required, at command[1], and manipulate the Matrix_ts points and
// transform accordingly; report back status of command evaluation.
int evaluateCommand(char ** command, Matrix_t * points, Matrix_t ** transform){
	if(2 < strlen(command[0]))
		return INVALID_CMD;

	char cmdChar = command[0][0];

	if(cmdChar == ADD_LINE){
		double x1, y1, z1, x2, y2, z2;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf",
			&x1, &y1, &z1, &x2, &y2, &z2) == EOF)
			return INVALID_ARGS;
		addEdge(points, x1, y1, z1, x2, y2, z2);
	}

	else if(cmdChar == SET_IDENTITY){
		freeMatrix(*transform);
		*transform = createIdentity();
	}

	else if(cmdChar == CREATE_SCALE){
		double sx, sy, sz;
		if(sscanf(command[1], "%lf %lf %lf", &sx, &sy, &sz) == EOF)
			return INVALID_ARGS;
		Matrix_t * scale = createScale(sx, sy, sz);
		multiplyMatrix(scale, *transform);
		freeMatrix(scale);
	}

	else if(cmdChar == CREATE_TRANSLATION){
		double tx, ty, tz;
		if(sscanf(command[1], "%lf %lf %lf", &tx, &ty, &tz) == EOF)
			return INVALID_ARGS;
		Matrix_t * translation = createTranslation(tx, ty, tz);
		multiplyMatrix(translation, *transform);
		freeMatrix(translation);
	}

	else if(cmdChar == CREATE_ROT_X || cmdChar == CREATE_ROT_Y ||
		cmdChar == CREATE_ROT_Z ){

		double theta;
		if(sscanf(command[1], "%lf", &theta) == EOF)
			return INVALID_ARGS;

		Matrix_t * rotation;
		switch(cmdChar){
			case CREATE_ROT_X:
				rotation = createRotation(X_AXIS, theta);
				break;

			case CREATE_ROT_Y:
				rotation = createRotation(Y_AXIS, theta);
				break;

			default:
				rotation = createRotation(Z_AXIS, theta);
				break;
		}

		multiplyMatrix(rotation, *transform);
		freeMatrix(rotation);
	}

	else if(cmdChar == APPLY_TRANSFORM){
		multiplyMatrix(*transform, points);
	}

	else if(cmdChar == DRAW_FRAME){
		clearScreen();
		drawMatrixLines(points);
		renderScreen();
	}

	else if(cmdChar == SAVE_FRAME){
		clearScreen();
		drawMatrixLines(points);
		renderScreen();
		command[1][strlen(command[1]) - 1] = '\0';  // remove newline
		if(writeScreen(command[1]) == -1){
			ERROR("Failed to write file: %s.", command[1]);
			return INVALID_ARGS;
		}
	}

	else
		return INVALID_CMD;

	return VALID_EVAL;
}

// indicate whether or not a char's corresponding command requires arguments
int argsRequired(char cmd){
	return cmd == ADD_LINE || cmd == CREATE_SCALE ||
		cmd == CREATE_TRANSLATION || cmd == CREATE_ROT_X ||
		cmd == CREATE_ROT_Y || cmd == CREATE_ROT_Z || cmd == SAVE_FRAME;
}

// deallocate a Script_t and all internal pointers
void freeScript(Script_t * script){
	int line;
	for(line = 0; line < script->numLines; line++)
		free(script->script[line]);
	free(script->script);
	free(script);
}
