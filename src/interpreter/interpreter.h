/*!
 * @file
 * @brief Functions for interpreting MDL commands.
 */

#pragma once

#include "src/graphics/matrix.h"
#include "src/interpreter/stack/stack.h"

/*
 * @brief Initialize an MDL script's constants and variables.
 *
 * Register the number of frames, and create a ::VariableGradient_t array
 * for all of the script's knob values. If any `vary` commands are called before
 * the `frame` command, initialization fails.
 *
 * @return 1 on initialization success; 0 on failure.
 */
int initializeVariables(void);

/*
 * @brief Evaluate the MDL script loaded in memory.
 *
 * Evaluate the MDL script file read and parsed by ::file_parser::readMDLFile(),
 * stored in memory in the @a op commands array.
 */
void evaluateMDLScript(void);
