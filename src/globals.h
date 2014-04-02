/*
 *  globals.h contains macros universally applicable to the graphics engine.
*/

#pragma once

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 700

// printf-style substitution macro used to print debug messages
#define ERROR(format, ...) \
	printf("%s: %d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
