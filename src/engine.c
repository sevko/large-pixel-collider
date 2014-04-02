/*
 *  engine.c is the graphic engine's main file; contains functions for testing
 *  functionality and startup.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/graphics.h"
#include "src/matrix.h"
#include "src/screen.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/shell.h"

static void sigHandler(int sig);
static void setup();

static void test(){
	configureScreen();
	Matrix_t * points = createMatrix(),
		* xRot = createRotation(X_AXIS, 1.0),
		* yRot = createRotation(Y_AXIS, 1.0),
		* zRot = createRotation(Z_AXIS, 1.0);
	addTorus(points, 0, 0, 20, 270);
	multiplyMatrix(createRotation(Z_AXIS, 90), points);
	addTorus(points, 0, 0, 20, 270);
	multiplyMatrix(createRotation(X_AXIS, 120), points);
	addTorus(points, 0, 0, 20, 270);
	multiplyMatrix(createRotation(X_AXIS, 120), points);
	addTorus(points, 0, 0, 20, 270);
	addSphere(points, 0, 0, 200);

	char * filename = malloc(20);

	int tick;
	for(tick = 0; tick < 139; tick++){
		printf("Tick: %d.\n", tick + 1);
		sprintf(filename, "tmp/img%04d", tick);
		clearScreen();
		multiplyMatrices(4, xRot, yRot, zRot, points);
		drawMatrix(points);
		writeScreen(filename);
	}

	freeMatrices(4, xRot, yRot, zRot, points);
	quitScreen();
}

// establish signal handler for SIGINT
static void sigHandler(int sig){
	(void)sig;
	exit(EXIT_SUCCESS);
}

// perform any necessary setup, before the engine runs
static void setup(){
	signal(SIGINT, sigHandler);
}

int main(int argc, char * argv[]){
	setup();
	if(1 < argc)
		readScriptFile(argv[1]);
	else
		test();
		// shell();
	return EXIT_SUCCESS;
}
