/*!
 *  @file
 *  @brief Functions that govern graphical rasterization and point-plotting.
 *
 *  graphics.h contains graphics-processing -- ie, line rasterization --
 *  functions, and ties together the engine's ::Matrix_t points with the visual
 *  "front-end".
 */

#pragma once

/*
 * @brief Draw a line with the default color.
 *
 * @param x1 See ::drawPixel().
 * @param y1 See ::drawPixel().
 * @param x2 See ::drawPixel().
 * @param y2 See ::drawPixel().
*/
#define drawLine1(x1, y1, x2, y2) drawLine(x1, y1, x2, y2, TEST_COLOR)

/*
 * @brief Draw a line with a specific color.
 *
 * @param x1 See ::drawPixel().
 * @param y1 See ::drawPixel().
 * @param x2 See ::drawPixel().
 * @param y2 See ::drawPixel().
 * @param color See ::drawPixel().
*/
#define drawLine2(x1, y1, x2, y2, color) drawLine(x1, y1, x2, y2, color)

/*!
 *  @brief Helper macro for the overloaded drawLine().
 *
 *  Replaces the drawLine() macro with drawLine1() or drawLine2(), depending
 *  on the number of received arguments.
 *
 *  @param arg1 Placeholder for an argument.
 *  @param arg2 Placeholder for an argument.
 *  @param arg3 Placeholder for an argument.
 *  @param arg4 Placeholder for an argument.
 *  @param arg5 Placeholder for an argument.
 *  @param func_name The macro to replace drawLine() with.
 *  @param ... Any subsequent arguments.
 */
#define DRAW_LINE_VA_MACRO(arg1, arg2, arg3, arg4, arg5, func_name, ...) \
	func_name

/*!
 *  @brief Overloaded drawLine(), which allows an optional color argument.
 *
 *  Uses DRAW_LINE_VA_MACRO to select an appropriate macro (drawLine1 or
 *  drawLine2) for the number of arguments (5, or 5 if a color is specified).
 *
 *  @code
 *      // The following macro-function calls are both valid.
 *      drawLine(10, 30, 50, 70);
 *      drawLine(40, 50, 60, 70, 0xFFFFFF);
 *  @code
 */
#define drawLine(...) \
	DRAW_LINE_VA_MACRO(__VA_ARGS__, drawLine2, drawLine1)(__VA_ARGS__)

/*!
 *  @brief Rasterize a line.
 *
 *  Rasterize a line with endpoints @f$(x_1, y_1)@f$ and @f$(x_2, y_2)@f$ using
 *  a fairly optimized Bresenham algorithm.
 *
 *  @param x1 The x-coordinate of the first endpoint.
 *  @param y1 The y-coordinate of the first endpoint.
 *  @param x2 The x-coordinate of the second endpoint.
 *  @param y2 The y-coordinate of the second endpoint.
 *  @param color The color of the line.
 */
void (drawLine)(int x1, int y1, int x2, int y2, int color);

/*
 * @brief Fill a triangle using scanline-rendering.
 *
 * @param x1 The x-coordinate of the first vertex.
 * @param y1 The y-coordinate of the first vertex.
 * @param x2 The x-coordinate of the second vertex.
 * @param y2 The y-coordinate of the second vertex.
 * @param x3 The x-coordinate of the third vertex.
 * @param y3 The y-coordinate of the third vertex.
 * @param color The color of the filled triangle.
*/
void scanlineRender(double x1, double y1, double x2, double y2, double x3,
	double y3, int color);
