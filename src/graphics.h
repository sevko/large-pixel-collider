/*!
 *  @file
 *  @brief Functions that govern graphical rasterization and point-plotting.
 *
 *  graphics.h contains graphics-processing -- ie, line rasterization --
 *  functions, and ties together the engine's ::Matrix_t points with the visual
 *  "front-end".
 */

#pragma once

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
 */
void drawLine(int x1, int y1, int x2, int y2);
