/*
 *  engine.c is the graphic engine's main file; contains functions for testing
 *  functionality and startup.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/graphics.h"
#include "src/matrix.h"
#include "src/screen.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/shell.h"
#include "src/unit_tests.h"

#define TEST_CMD "--test"
#define SCRIPT_CMD "--script"

static void sigHandler(int sig);
static void setup();
static void argumentHandler(int argc, char * argv[]);

static void argumentHandler(int argc, char * argv[]){
	if(1 < argc){
		if(strcmp(TEST_CMD, argv[1]) == 0)
			unitTests();

		else if(strcmp(SCRIPT_CMD, argv[1]) == 0){
			if(argc == 3)
				readScriptFile(argv[2]);

			else
				FATAL("--script flag requires argument.");
		}

		else
			FATAL("Argument not recognized.");
	}

	else
		shell();
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
	argumentHandler(argc, argv);
	return EXIT_SUCCESS;
}
