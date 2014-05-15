/*!
 * @file
 * @brief Functions for interpreting MDL commands.
 */

#pragma once

#include "src/graphics/matrix.h"
#include "src/interpreter/stack/stack.h"

/*
 * @brief Evaluate the MDL script loaded in memory.
 *
 * Evaluate the MDL script file read and parsed by ::file_parser::readMDLFile(),
 * stored in memory in the @a op commands array.
 *
 * @param points A pointer to a pointer to the points ::Matrix_t.
 * @param coordStack A pointer to the coordinates ::Stack_t coordStack.
 */
void evaluateMDLScript(Matrix_t ** points, Stack_t * coordStack);
