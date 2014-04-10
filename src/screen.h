/*!
 *  @file
 *  @brief Functions for initializing and drawing to the engine's SDL screen.
 */

#pragma once

//! The default color of pixels plotted with drawPixel().
#define TEST_COLOR 0x00FF0000

/*!
 *  @brief Draw a pixel at @a (@p x, @p y) with color TEST_COLOR.
 *
 *  @param x The x-coordinate of the pixel to draw.
 *  @param y The y-coordinate of the pixel to draw.
 */
#define drawPixel1(x, y) drawPixel(x, y, TEST_COLOR)

/*!
 *  @brief Draw a pixel at @a (@p x, @p y) with color @p color.
 *
 *  @param x (int) The x-coordinate of the pixel to draw.
 *  @param y (int) The y-coordinate of the pixel to draw.
 *  @param color (int) The color of the pixel.
 */
#define drawPixel2(x, y, color) drawPixel(x, y, color)

/*!
 *  @brief Helper macro for the overloaded drawPixel().
 *
 *  Replaces the drawPixel() macro with drawPixel1() or drawPixel2(), depending
 *  on the number of received arguments.
 *
 *  @param _1 Placeholder for an argument.
 *  @param _2 Placeholder for an argument.
 *  @param _3 Placeholder for an argument.
 *  @param NAME The macro to replace drawPixel() with.
 *  @param ... Any subsequent arguments.
 */
#define VA_MACRO(_1, _2, _3, NAME, ...) NAME

/*!
 *  @brief Overloaded drawPixel(), which allows an optional color argument.
 *
 *  Uses VA_MACRO to select an appropriate macro (drawPixel or drawPixel2) for
 *  the number of arguments (2, or 3 if a color is specified).
 *
 *  @code
 *      // the following macro-function calls are both valid.
 *      drawPixel(10, 30);
 *      drawPixel(40, 50, 0xFFFFFF);
 *  @code
 */
#define drawPixel(...) \
	VA_MACRO(__VA_ARGS__, drawPixel2, drawPixel1)(__VA_ARGS__)

/*!
 *  @brief Initialize the SDL screen.
 */
void configureScreen(void);

/*!
 *  @brief Draw a pixel to the SDL screen.
 *
 *  @param x The x-coordinate of the pixel.
 *  @param y The y-coordinate of the pixel.
 */
void (drawPixel)(int x, int y, int color);

/*!
 *  @brief Update the SDL screen to display pixels newly plotted with
 *      drawPixel().
 *
 *  renderScreen() must be called for the SDL screen to display any newly
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
 *  quitScreen() should be called once the engine finishes using the SDL screen.
 */
void quitScreen(void);

/*!
 *  @brief Save the currently displayed SDL screen to a BMP file.
 *
 *  @param filename The path of the BMP file to save the screen to.
 */
int writeScreen(const char * const filename);
