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
	drawCircle(points, 0, 0, 100);
	multiplyMatrix(createScale(2, 2, 2), points);
	drawMatrixLines(points);
	renderScreen();
	usleep(10e6);
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
