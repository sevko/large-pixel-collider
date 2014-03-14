/*
 *  interpreter.c contains functions for evaluating strings containing commands
 *  and arguments in the graphics engine's scripting language.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/graphics.h"
#include "src/matrix.h"
#include "src/screen.h"
#include "src/interpreter/interpreter.h"

#define COMMENT_CHAR '#'                // ignore lines beginning with this

// chars indicating commands in a script file
#define ADD_LINE_CMD 'l'
#define ADD_CIRCLE_CMD 'c'
#define APPLY_TRANSFORM_CMD 'a'
#define CREATE_SCALE_CMD 's'
#define CREATE_TRANSLATION_CMD 't'
#define CREATE_ROT_X_CMD 'x'
#define CREATE_ROT_Y_CMD 'y'
#define CREATE_ROT_Z_CMD 'z'
#define DRAW_FRAME_CMD 'v'
#define SAVE_FRAME_CMD 'g'
#define SET_IDENTITY_CMD 'i'

// evaluate the command located at command[0], accounting for the arguments,
// if any are required, at command[1], and manipulate the Matrix_ts points and
// transform accordingly; report back status of command evaluation.
int evaluateCommand(char ** const command, Matrix_t * const points,
	Matrix_t ** transform){
	char cmdChar = command[0][0];

	int len = strlen(command[0]);
	if(cmdChar == '\n' || len == 0 || cmdChar == COMMENT_CHAR)
		return CMD_VALID_EVAL;

	else if(2 < len)
		return CMD_INVALID_CMD ;

	else if(cmdChar == ADD_LINE_CMD){
		double x1, y1, z1, x2, y2, z2;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf",
			&x1, &y1, &z1, &x2, &y2, &z2) < 6)
			return CMD_INVALID_ARGS;
		addEdge(points, x1, y1, z1, x2, y2, z2);
	}

	else if(cmdChar == ADD_CIRCLE_CMD){
		double oX, oY, radius;
		if(sscanf(command[1], "%lf %lf %lf", &oX, &oY, &radius) < 3)
			return CMD_INVALID_ARGS;
		drawCircle(points, oX, oY, radius);
	}

	else if(cmdChar == SET_IDENTITY_CMD){
		freeMatrix(*transform);
		*transform = createIdentity();
	}

	else if(cmdChar == CREATE_SCALE_CMD){
		double sx, sy, sz;
		if(sscanf(command[1], "%lf %lf %lf", &sx, &sy, &sz) < 3)
			return CMD_INVALID_ARGS;
		Matrix_t * scale = createScale(sx, sy, sz);
		multiplyMatrix(scale, *transform);
		freeMatrix(scale);
	}

	else if(cmdChar == CREATE_TRANSLATION_CMD){
		double tx, ty, tz;
		if(sscanf(command[1], "%lf %lf %lf", &tx, &ty, &tz) < 3)
			return CMD_INVALID_ARGS;
		Matrix_t * translation = createTranslation(tx, ty, tz);
		multiplyMatrix(translation, *transform);
		freeMatrix(translation);
	}

	else if(cmdChar == CREATE_ROT_X_CMD || cmdChar == CREATE_ROT_Y_CMD ||
		cmdChar == CREATE_ROT_Z_CMD ){

		double theta;
		if(sscanf(command[1], "%lf", &theta) < 1)
			return CMD_INVALID_ARGS;

		Matrix_t * rotation;
		switch(cmdChar){
			case CREATE_ROT_X_CMD:
				rotation = createRotation(X_AXIS, theta);
				break;

			case CREATE_ROT_Y_CMD:
				rotation = createRotation(Y_AXIS, theta);
				break;

			default:
				rotation = createRotation(Z_AXIS, theta);
				break;
		}

		multiplyMatrix(rotation, *transform);
		freeMatrix(rotation);
	}

	else if(cmdChar == APPLY_TRANSFORM_CMD){
		multiplyMatrix(*transform, points);
	}

	else if(cmdChar == DRAW_FRAME_CMD){
		clearScreen();
		drawMatrixLines(points);
		renderScreen();
	}

	else if(cmdChar == SAVE_FRAME_CMD){
		clearScreen();
		drawMatrixLines(points);
		renderScreen();

		int lenName = strlen(command[1]);
		char * filename = malloc(lenName + 1);
		strcpy(filename, command[1]);

		if(filename[lenName - 1] == '\n')
			filename[lenName - 1] = '\0';

		if(writeScreen(filename) == -1){
			ERROR("Failed to write file: %s.", filename);
			return CMD_INVALID_ARGS;
		}

		free(filename);
	}

	else if(cmdChar == HELP_CMD || cmdChar == EXIT_CMD )
		return CMD_SPECIAL;

	else
		return CMD_INVALID_CMD ;

	return CMD_VALID_EVAL;
}

// indicate whether or not a char's corresponding command requires arguments
int argsRequired(char cmd){
	return cmd == ADD_LINE_CMD || cmd == ADD_CIRCLE_CMD || cmd ==
		CREATE_SCALE_CMD || cmd == CREATE_TRANSLATION_CMD || cmd ==
		CREATE_ROT_X_CMD || cmd == CREATE_ROT_Y_CMD || cmd == CREATE_ROT_Z_CMD
		|| cmd == SAVE_FRAME_CMD;
}
