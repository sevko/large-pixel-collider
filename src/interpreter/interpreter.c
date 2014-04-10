#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/matrix.h"
#include "src/screen.h"
#include "src/interpreter/interpreter.h"

/*!
 *  @brief Lines beginning with the COMMENT_CHAR char are ignored.
 */
#define COMMENT_CHAR '#'

/*!
 *  @brief Command for adding a Bezier curve to the points ::Matrix_t.
 */
#define ADD_BEZIER_CMD 'b'

/*!
 *  @brief Command for adding a circle to the points ::Matrix_t.
 */
#define ADD_CIRCLE_CMD 'c'

/*!
 *  @brief Command for adding a hermite curve to the points ::Matrix_t.
 */
#define ADD_HERMITE_CMD 'h'

/*!
 *  @brief Command for adding a line segment to the points ::Matrix_t.
 */
#define ADD_LINE_CMD 'l'

/*!
 *  @brief Command for adding a rectangular prism to the points ::Matrix_t.
 */
#define ADD_RECT_PRISM_CMD 'p'

/*!
 *  @brief Command for adding a sphere to the points ::Matrix_t.
 */
#define ADD_SPHERE_CMD 'm'

/*!
 *  @brief Command for adding a torus to the points ::Matrix_t.
 */
#define ADD_TORUS_CMD 'd'

/*!
 *  @brief Command for applying the current transformation ::Matrix_t to the
 *      current points ::Matrix_t.
 */
#define APPLY_TRANSFORM_CMD 'a'

/*!
 *  @brief Command for clearing the current points ::Matrix_t.
 */
#define CLEAR_POINTS_CMD 'w'

/*!
 *  @brief Command for adding a scale matrix to the current transformation
 *  ::Matrix_t.
 */
#define CREATE_SCALE_CMD 's'

/*!
 *  @brief Command for adding a translation matrix to the current transformation
 *      ::Matrix_t.
 */
#define CREATE_TRANSLATION_CMD 't'

/*!
 *  @brief Command for adding a rotation (through the x-axis) matrix to the
 *      current transformation ::Matrix_t.
 */
#define CREATE_ROT_X_CMD 'x'

/*!
 *  @brief Command for adding a rotation (through the y-axis) matrix to the
 *      current transformation ::Matrix_t.
 */
#define CREATE_ROT_Y_CMD 'y'

/*!
 *  @brief Command for adding a rotation (through the z-axis) matrix to the
 *      current transformation ::Matrix_t.
 */
#define CREATE_ROT_Z_CMD 'z'

/*!
 *  @brief Command for drawing the current points ::Matrix_t to the screen.
 */
#define DRAW_FRAME_CMD 'v'

/*!
 *  @brief Command for drawing the current points ::Matrix_t to the screen,
 *      and saving the frame to a file.
 */
#define SAVE_FRAME_CMD 'g'

/*!
 *  @brief Command for setting the current transformation ::Matrix_t to an
 *      identity matrix.
 */
#define SET_IDENTITY_CMD 'i'

int evaluateCommand(char ** const command, Matrix_t ** points,
	Matrix_t ** transform){
	char cmdChar = command[0][0];

	int len = strlen(command[0]);
	if(cmdChar == '\n' || len == 0 || cmdChar == COMMENT_CHAR)
		return CMD_VALID_EVAL;

	else if(2 < len)
		return CMD_INVALID_CMD ;

	else if(cmdChar == ADD_BEZIER_CMD){
		double x0, y0, x1, y1, x2, y2, x3, y3;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf %lf %lf",
			&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3) < 8)
			return CMD_INVALID_ARGS;
		addBezier(*points, x0, y0, x1, y1, x2, y2, x3, y3);
	}

	else if(cmdChar == ADD_CIRCLE_CMD){
		double oX, oY, radius;
		if(sscanf(command[1], "%lf %lf %lf", &oX, &oY, &radius) < 3)
			return CMD_INVALID_ARGS;
		addCircle(*points, oX, oY, radius);
	}

	else if(cmdChar == ADD_HERMITE_CMD){
		double x0, y0, x1, y1, x2, y2, x3, y3;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf %lf %lf",
			&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3) < 8)
			return CMD_INVALID_ARGS;
		addHermite(*points, x0, y0, x1, y1, x2, y2, x3, y3);
	}

	else if(cmdChar == ADD_LINE_CMD){
		double x1, y1, z1, x2, y2, z2;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf",
			&x1, &y1, &z1, &x2, &y2, &z2) < 6)
			return CMD_INVALID_ARGS;
		addEdge(*points, x1, y1, z1, x2, y2, z2);
	}

	else if(cmdChar == ADD_RECT_PRISM_CMD){
		double x, y, z, width, height, depth;
		if(sscanf(command[1], "%lf %lf %lf %lf %lf %lf",
			&x, &y, &z, &width, &height, &depth) < 6)
			return CMD_INVALID_ARGS;
		addRectangularPrism(*points, x, y, z, width, height, depth);
	}

	else if(cmdChar == ADD_SPHERE_CMD){
		double x, y, radius;
		if(sscanf(command[1], "%lf %lf %lf", &x, &y, &radius) < 3)
			return CMD_INVALID_ARGS;
		addSphere(*points, x, y, radius);
	}

	else if(cmdChar == ADD_TORUS_CMD){
		double x, y, rad1, rad2;
		if(sscanf(command[1], "%lf %lf %lf %lf", &x, &y, &rad1, &rad2) < 4)
			return CMD_INVALID_ARGS;
		addTorus(*points, x, y, rad1, rad2);
	}

	else if(cmdChar == APPLY_TRANSFORM_CMD)
		multiplyMatrix(*transform, *points);

	else if(cmdChar == CLEAR_POINTS_CMD){
		freeMatrix(*points);
		*points = createMatrix();
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

	else if(cmdChar == DRAW_FRAME_CMD){
		clearScreen();
		drawMatrix(*points);
		renderScreen();
	}

	else if(cmdChar == HELP_CMD || cmdChar == EXIT_CMD )
		return CMD_SPECIAL;

	else if(cmdChar == SAVE_FRAME_CMD){
		clearScreen();
		drawMatrix(*points);
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

	else if(cmdChar == SET_IDENTITY_CMD){
		freeMatrix(*transform);
		*transform = createIdentity();
	}

	else
		return CMD_INVALID_CMD ;

	return CMD_VALID_EVAL;
}

int argsRequired(char cmd){
	return
		cmd == ADD_BEZIER_CMD ||
		cmd == ADD_CIRCLE_CMD ||
		cmd == ADD_LINE_CMD ||
		cmd == ADD_RECT_PRISM_CMD ||
		cmd == ADD_SPHERE_CMD ||
		cmd == ADD_TORUS_CMD ||
		cmd == ADD_HERMITE_CMD ||
		cmd == CREATE_SCALE_CMD ||
		cmd == CREATE_TRANSLATION_CMD ||
		cmd == CREATE_ROT_X_CMD ||
		cmd == CREATE_ROT_Y_CMD ||
		cmd == CREATE_ROT_Z_CMD ||
		cmd == SAVE_FRAME_CMD;
}
