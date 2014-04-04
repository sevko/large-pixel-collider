/*
 *  globals.h contains macros universally applicable to the graphics engine.
*/

#pragma once

#include <stdlib.h>

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 700

// printf-style substitution macro used to print debug messages
#define ERROR(format, ...) \
	printf("%s: %d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// same as ERROR(), but exits with code 1
#define FATAL(format, ...) \
	do {\
		ERROR(format " Exiting.", __VA_ARGS__);\
		exit(EXIT_FAILURE);\
	} while(0)
