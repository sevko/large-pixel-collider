/*!
 *  @file
 *  @brief Functions for initializing and drawing to the engine's SDL screen.
 */

#pragma once

#include "src/globals.h"
#include "src/graphics/matrix.h"

/*!
 * @brief Draw a pixel at a pair of coordinates with color TEST_COLOR.
 *
 * @param pt See ::drawPixel().
 */
#define drawPixel1(pt) drawPixel(pt, TEST_COLOR)

/*!
 * @brief Draw a pixel at a pair of coordinates with color @p color.
 *
 * @param pt See ::drawPixel().
 * @param color See ::drawPixel().
 */
#define drawPixel2(pt, color) drawPixel(pt, color)

/*!
 *  @brief Helper macro for the overloaded ::drawPixel().
 *
 *  Replaces the ::drawPixel() macro with ::drawPixel1() or ::drawPixel2(),
 *  depending on the number of received arguments.
 *
 *  @param arg1 Placeholder for an argument.
 *  @param arg2 Placeholder for an argument.
 *  @param func_name The macro to replace drawPixel() with.
 *  @param ... Any subsequent arguments.
 */
#define DRAW_PIXEL_VA_MACRO(arg1, arg2, func_name, ...) func_name

/*!
 *  @brief Overloaded drawPixel(), which allows an optional color argument.
 *
 *  Uses ::DRAW_PIXEL_VA_MACRO() to select an appropriate macro (::drawPixel1()
 *  or ::drawPixel2()) for the number of arguments (1, or 2 if a color is
 *  specified).
 *
 *  @code
 *      // The following macro-function calls are both valid.
 *      drawPixel(POINT(10, 30));
 *      drawPixel(POINT(40, 50), 0xFFFFFF);
 *  @code
 */
#define drawPixel(...) \
	DRAW_PIXEL_VA_MACRO(__VA_ARGS__, drawPixel2, drawPixel1)(__VA_ARGS__)

/*!
 *  @brief Initialize the SDL screen.
 */
void configureScreen(void);

/*!
 * @brief Draw a pixel to the SDL screen.
 *
 * @param pt The coordinates of the pixel to draw.
 * @param color The color of the pixel.
 */
void (drawPixel)(Point_t *pt, int color);

/*!
 *  @brief Update the SDL screen to display pixels newly plotted with
 *      ::drawPixel().
 *
 *  ::renderScreen() must be called for the SDL screen to display any newly
 *  rendered pixels.
 */
void renderScreen(void);

/*!
 *  @brief Clear the SDL screen.
 *
 *  Clear the screen by filling it with black pixels.
 */
void clearScreen(void);

/*!
 *  @brief Deinitialize the SDL screen.
 *
 *  ::quitScreen() should be called once the engine finishes using the SDL
 *  screen.
 */
void quitScreen(void);

/*!
 *  @brief Save the currently displayed SDL screen to a BMP file.
 *
 *  @param filename The path of the BMP file to save the screen to.
 */
int writeScreen(const char * const filename);
