/*!
 *  @file globals.h
 *  @brief A collection of macros and declarations universal to the graphics
 *      engine.
 *
 *  globals.h contains object-like and function-like macros, such as the
 *  dimensions of the screen and those to facilitate debugging, used universally
 *  throughout the engine's source code.
*/

#pragma once

#include <stdlib.h>

/*!
 *  @brief The width of the engine's screen.
 */
#define IMAGE_WIDTH 800

/*!
 *  @brief The height of the engine's screen.
 */
#define IMAGE_HEIGHT 700

/*!
 *  @brief Printf-style function macro with helpful debugging information.
 *
 *  A wrapper macro for printf that prints formatted arguments along with
 *  the filename and line number of the call's origin.
 *
 *  @param format Type "const char *" A format string.
 *  @param ... The format string's corresponding arguments.
 */
#define ERROR(format, ...) \
	printf("%s: %d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/*!
 *  @brief A wrapper macro for the ERROR() macro, that additionally exits.
 *
 *  FATAL() is a wrapper for the ERROR() macro, which additionally exits with an
 *  error code of 1 -- this distinguishes fatal errors from their recoverable
 *  counterparts.
 *
 *  @param format Type "const char *" A format string.
 *  @param ... The format string's corresponding arguments.
 */
#define FATAL(format, ...) \
	do {\
		ERROR(format " Exiting.", ##__VA_ARGS__);\
		exit(EXIT_FAILURE);\
	} while(0)
