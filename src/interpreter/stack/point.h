/*!
 *  @file
 *  @brief A collection of wrapper macros for ::Matrix_t to emulate single
 *      points.
*/

#pragma once

#include "src/graphics/matrix.h"

/*
 * @param point (Matrix_t *) A matrix with a single point.
 *
 * @return The x-coordinate of the first point in @p point.
 */
#define GET_X(origin) origin->points[X][0]

/*
 * @param point (Matrix_t *) A matrix with a single point.
 *
 * @return The x-coordinate of the first point in @p point.
 */
#define GET_Y(origin) origin->points[Y][0]

/*
 * @param point (Matrix_t *) A matrix with a single point.
 *
 * @return The x-coordinate of the first point in @p point.
 */
#define GET_Z(origin) origin->points[Z][0]
