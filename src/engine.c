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
#include "src/graphics/graphics.h"
#include "src/graphics/matrix.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/shell.h"
#include "src/interpreter/stack/stack.h"

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

void test();

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

static void sigHandler(int sig){
	(void)sig;
	exit(EXIT_SUCCESS);
}

static void setup(void){
	signal(SIGINT, sigHandler);
}

void test(){
	char * s1 = malloc(100); strcpy(s1, "111111111");
	char * s2 = malloc(100); strcpy(s2, "222222222");
	char * s3 = malloc(100); strcpy(s3, "333333333");
	char * s4 = malloc(100); strcpy(s4, "444444444");
	char * s5 = malloc(100); strcpy(s5, "555555555");
	char * s6 = malloc(100); strcpy(s6, "666666666");
	char * s7 = malloc(100); strcpy(s7, "777777777");
	char * s8 = malloc(100); strcpy(s8, "888888888");
	char * s9 = malloc(100); strcpy(s9, "999999999");

	Stack_t * stack = createStack();
	push(stack, s1);
	push(stack, s2);
	push(stack, s3);
	push(stack, s4);
	push(stack, s5);
	push(stack, s6);
	push(stack, s7);
	push(stack, s8);
	push(stack, s9);

	char * popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);
	popped = pop(stack); printf("%s\n", popped); free(popped);

	freeStack(stack);
}

void engineDriver(){
	printf("Hello, world!\n");
}

// int main(int argc, char * argv[]){
	// setup();
	// argumentHandler(argc, argv);
	// return EXIT_SUCCESS;
// }
