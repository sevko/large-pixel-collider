/*!
 *  @file
 *  @brief Functions that govern graphical rasterization and point-plotting.
 *
 *  graphics.h contains graphics-processing -- ie, line rasterization --
 *  functions, and ties together the engine's ::Matrix_t points with the visual
 *  "front-end".
 */

#pragma once

#include "src/graphics/matrix.h"

/*
 * @brief Draw a line with the default color.
 *
 * @param p1 See ::drawPixel().
 * @param p2 See ::drawPixel().
*/
#define drawLine1(p1, p2) drawLine(p1, p2, TEST_COLOR)

/*
 * @brief Draw a line with a specific color.
 *
 * @param p1 See ::drawPixel().
 * @param p2 See ::drawPixel().
 * @param color See ::drawPixel().
*/
#define drawLine2(p1, p2, color) drawLine(p1, p2, color)

/*!
 *  @brief Helper macro for the overloaded ::drawLine().
 *
 *  Replaces the ::drawLine() macro with ::drawLine1() or ::drawLine2(),
 *  depending on the number of received arguments.
 *
 *  @param arg1 Placeholder for an argument.
 *  @param arg2 Placeholder for an argument.
 *  @param arg3 Placeholder for an argument.
 *  @param func_name The macro to replace ::drawLine() with.
 *  @param ... Any subsequent arguments.
 */
#define DRAW_LINE_VA_MACRO(arg1, arg2, arg3, func_name, ...) \
	func_name

/*!
 *  @brief Overloaded ::drawLine(), which allows an optional color argument.
 *
 *  Uses ::DRAW_LINE_VA_MACRO() to select an appropriate macro for the number
 *  of arguments (2, or 3 if a color is specified).
 *
 *  @code
 *      // The following macro-function calls are both valid.
 *      drawLine(POINT(1, 2, 3), POINT(3, 4, 5));
 *      drawLine(POINT(1, 2, 3), POINT(3, 4, 5), 0xFFFFFF);
 *  @code
 */
#define drawLine(...) \
	DRAW_LINE_VA_MACRO(__VA_ARGS__, drawLine2, drawLine1)(__VA_ARGS__)

/*
 * @brief Return an ::RGB_t representation of a color.
 *
 * @param r (int) The red color value.
 * @param g (int) The green color value.
 * @param b (int) The blue color value.
*/
#define RGB(r, g, b) ((int []){r, g, b})

#define R 0 // The index of the red color value in a ::RGB_t *.
#define G 1 // The index of the green color value in a ::RGB_t *.
#define B 2 // The index of the blue color value in a ::RGB_t *.

typedef int RGB_t; // Used to represent RGB color values.

/*!
 *  @brief Rasterize a line.
 *
 *  Rasterize a line with endpoints @a (p1[X], p1[Y]) and @a (p2[X], p2[Y])
 *  using the Bresenham algorithm.
 *
 *  @param p1 The first endpoint.
 *  @param p2 The second endpoint.
 *  @param color The color of the line.
 */
void (drawLine)(Point_t *p1, Point_t *p2, int color);

/*
 * @brief Fill a triangle using scanline-rendering.
 *
 * @param p1 The first vertex of the triangle.
 * @param p2 The second vertex of the triangle.
 * @param p3 The third vertex of the triangle.
*/
void scanlineRender(Point_t *p1, Point_t *p2, Point_t *p3);
