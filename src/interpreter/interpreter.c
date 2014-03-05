/*
 *  interpreter.c contains functions for evaluating strings containing commands
 *  and arguments in the graphics engine's scripting language.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/matrix.h"
#include "src/screen.h"
#include "src/interpreter/interpreter.h"

// chars indicating commands in a script file
#define ADD_LINE 'l'
#define APPLY_TRANSFORM 'a'
#define CREATE_SCALE 's'
#define CREATE_TRANSLATION 't'
#define CREATE_ROT_X 'x'
#define CREATE_ROT_Y 'y'
#define CREATE_ROT_Z 'z'
#define DRAW_FRAME 'v'
#define SAVE_FRAME 'g'
#define SET_IDENTITY 'i'

// evaluate the command located at command[0], accounting for the arguments,
// if any are required, at command[1], and manipulate the Matrix_ts points and
// transform accordingly; report back status of command evaluation.
int evaluateCommand(char ** const command, Matrix_t * const points,
	Matrix_t ** transform){
	if(2 < strlen(command[0]))
		return INVALID_CMD;

	char cmdChar = command[0][0];

	if(cmdChar == ADD_LINE){
		double x1, y1, z1, x2, y2, z2;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf",
			&x1, &y1, &z1, &x2, &y2, &z2) < 6)
			return INVALID_ARGS;
		addEdge(points, x1, y1, z1, x2, y2, z2);
	}

	else if(cmdChar == SET_IDENTITY){
		freeMatrix(*transform);
		*transform = createIdentity();
	}

	else if(cmdChar == CREATE_SCALE){
		double sx, sy, sz;
		if(sscanf(command[1], "%lf %lf %lf", &sx, &sy, &sz) < 3)
			return INVALID_ARGS;
		Matrix_t * scale = createScale(sx, sy, sz);
		multiplyMatrix(scale, *transform);
		freeMatrix(scale);
	}

	else if(cmdChar == CREATE_TRANSLATION){
		double tx, ty, tz;
		if(sscanf(command[1], "%lf %lf %lf", &tx, &ty, &tz) < 3)
			return INVALID_ARGS;
		Matrix_t * translation = createTranslation(tx, ty, tz);
		multiplyMatrix(translation, *transform);
		freeMatrix(translation);
	}

	else if(cmdChar == CREATE_ROT_X || cmdChar == CREATE_ROT_Y ||
		cmdChar == CREATE_ROT_Z ){

		double theta;
		if(sscanf(command[1], "%lf", &theta) < 1)
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
