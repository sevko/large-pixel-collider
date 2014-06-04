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
 * @param pt See ::plotPixel().
 */
#define plotPixel1(pt) plotPixel(pt, TEST_COLOR)

/*!
 * @brief Draw a pixel at a pair of coordinates with color @p color.
 *
 * @param pt See ::plotPixel().
 * @param color See ::plotPixel().
 */
#define plotPixel2(pt, color) plotPixel(pt, color)

/*!
 *  @brief Helper macro for the overloaded ::plotPixel().
 *
 *  Replaces the ::plotPixel() macro with ::plotPixel1() or ::plotPixel2(),
 *  depending on the number of received arguments.
 *
 *  @param arg1 Placeholder for an argument.
 *  @param arg2 Placeholder for an argument.
 *  @param func_name The macro to replace plotPixel() with.
 *  @param ... Any subsequent arguments.
 */
#define DRAW_PIXEL_VA_MACRO(arg1, arg2, func_name, ...) func_name

/*!
 *  @brief Overloaded plotPixel(), which allows an optional color argument.
 *
 *  Uses ::DRAW_PIXEL_VA_MACRO() to select an appropriate macro (::plotPixel1()
 *  or ::plotPixel2()) for the number of arguments (1, or 2 if a color is
 *  specified).
 *
 *  @code
 *      // The following macro-function calls are both valid.
 *      plotPixel(POINT(10, 30));
 *      plotPixel(POINT(40, 50), 0xFFFFFF);
 *  @code
 */
#define plotPixel(...) \
	DRAW_PIXEL_VA_MACRO(__VA_ARGS__, plotPixel2, plotPixel1)(__VA_ARGS__)

typedef struct {
	// 3D representation of each pixel's current height/color.
	double buf[IMAGE_HEIGHT][IMAGE_WIDTH][2];
} ZBuffer_t;

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
void (plotPixel)(Point_t *pt, int color);

/*!
 *  @brief Update the SDL screen to display pixels newly plotted with
 *      ::plotPixel().
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

/*
 * @brief Create a ::ZBuffer_t.
 *
 * @return The new ::ZBuffer_t.
*/
ZBuffer_t *createZBuffer(void);

/*
 * @brief Zero out a ::ZBuffer_t::buf.
 *
 * @param zBuf The ::ZBuffer_t to clear.
*/
void clearZBuffer(ZBuffer_t *zBuf);
/*
 * @brief Recreate a ::ZBuffer_t from a file written with
 *      ::writeZBufferToFile().
 *
 * @note Used only by ::unit_tests.
 * @param filePath The path to the file to read.
 *
 * @return A pointer to the newly created ::ZBuffer_t.
*/
ZBuffer_t *readZBufferFromFile(const char *filePath);
/*
 * @brief Write a ::ZBuffer_t to a file.
 *
 * The ::ZBuffer_t::buf is written to a file named @p filePath in the following
 * format:
 *
 *      %(d1):%(f1),%(f2),...
 *
 *      %(d1) : The number of points in ::ZBuffer_t::buf.
 *      %(f1) : The z-coordinate of the first pixel.
 *      %(f2) : The color of the first pixel.
 *      ... : The pattern "%(f1),%(f2)," for every other pixel.
 *
 * @param zBuf The ::ZBuffer_t to write.
 * @param filePath The path of the file to write.
*/
void writeZBufferToFile(ZBuffer_t *zBuf, const char *filePath);

/*
 * @brief Determine whether two ::ZBuffer_t are identical.
 *
 * The contents of ::ZBuffer_t::buf are inspected for equality. Note that both
 * ::ZBuffer_t::buf are expected to contain ::IMAGE_HEIGHT * ::IMAGE_WIDTH
 * points; if one does not, undefined behavior ensues.
 *
 * @param zBuf1 The first ::ZBuffer_t.
 * @param zBuf2 The second ::ZBuffer_t.
 *
 * @return 1 if the ::ZBuffer_t::buf are equal; 0, otherwise.
*/
int equalZBuffers(ZBuffer_t *zBuf1, ZBuffer_t *zBuf2);
