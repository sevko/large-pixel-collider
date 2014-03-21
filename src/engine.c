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

// function to test drawPolygon() and drawCircle()
void testPolygon(){
	configureScreen();
	Matrix_t * points = createMatrix();
	Matrix_t * xRot = createRotation(X_AXIS, 1);
	Matrix_t * yRot = createRotation(Y_AXIS, 1);
	Matrix_t * zRot = createRotation(Z_AXIS, 1);

	addEdge(points, 0, 200, 0, 0, -200, 0);
	addTorus(points, 100, 100, 100, 50);

	int i;
	for(i = 0; i < 1000; i++){
		clearScreen();
		multiplyMatrices(4, xRot, yRot, zRot, points);
		drawMatrixLines(points);
		renderScreen();
		usleep(1e6 / 60);
	}

	usleep(10e6);

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
	// else
		// shell();
	testPolygon();
	return EXIT_SUCCESS;
}
