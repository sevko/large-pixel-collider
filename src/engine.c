/*!
 *  @file
 *  @brief The engine's main module.
 *
 *  engine.c is the graphics engine's main module, and contains functions for
 *  project initialization and setup.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/unit_tests.h"
#include "src/graphics/geometry.h"
#include "src/graphics/graphics.h"
#include "src/graphics/matrix.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/stack/stack.h"

#include "lib/parser.h"

#define TEST_CMD "--test"
#define SCRIPT_CMD "--script"

/*
 *  @brief Establish a signal handler for the argument signal.
 *
 *  Establish a signal handler, or custom response to, a given signal. The only
 *  current handler exits the engine successfully (with an error code of 0) on
 *  a SIGINT.
 *
 *  @param sig The integer value of the signal.
 */
static void sigHandler(int sig);

/*
 *  @brief Establish signal handlers and perform setup before the engine starts.
 */
static void setup(void);

/*
 *  @brief Handle command-line arguments.
 *
 *  Respond to any command-line arguments:
 *      1. if no arguments are passed, start the engine's shell.
 *      2. if the command is TEST_CMD, run all unit tests.
 *      3. if the command is SCRIPT_CMD, evaluate the script file at the
 *          location specified by the subsequent argument.
 *
 *  Exit with an error code of 1 should unrecognized or insufficient arguments
 *  be received.
 *
 *  @param argc The number of command-line arguments, as seen by main().
 *  @param argv The command-line argument strings, as seen by main().
 */
static void argumentHandler(int argc, char * argv[]);

void test(){
	// printf("%X\n", rgbToInt(flatShade((int []){0xAB, 0xCD, 0xEF})));
	configureScreen();
	Matrix_t *pts = createMatrix();
	addTorus(pts, POINT(0, 0), 100, 200);
	Matrix_t *rX = createRotation(X_AXIS, 1),
		*rY = createRotation(Y_AXIS, 1),
		*rZ = createRotation(Z_AXIS, 1);

	int tick;
	for(tick = 0; tick < 1000; tick++){
		clearScreen();
		multiplyMatrices(4, rX, rY, rZ, pts);
		drawMatrix(pts);
		renderScreen();
		usleep(1e6 / 140);
	}

	renderScreen();
	usleep(4e6);
	quitScreen();
}

static void argumentHandler(int argc, char * argv[]){
	if(1 < argc){
		if(strcmp(TEST_CMD, argv[1]) == 0)
			unitTests();

		else if(strcmp(SCRIPT_CMD, argv[1]) == 0){
			if(argc == 3)
				readMDLFile(argv[2]);

			else
				FATAL("--script flag requires argument.");
		}

		else
			FATAL("Argument not recognized.");
	}
	else
		// puts("No arguments given. Exiting.");
		test();
}

static void sigHandler(int sig){
	(void)sig;
	exit(EXIT_SUCCESS);
}

static void setup(void){
	signal(SIGINT, sigHandler);
}

int main(int argc, char * argv[]){
	setup();
	argumentHandler(argc, argv);
	return EXIT_SUCCESS;
}
