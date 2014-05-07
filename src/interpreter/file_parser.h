/*!
 *  @file
 *  @brief Functions for reading and evaluating the commands contained in a
 *      file of the engine's script flavor.
 */

#pragma once

/*
 * @brief Read and evaluate the commands inside an MDL script file.
 *
 * The commands are evaluated against an internal points ::Matrix_t, which is
 * rendered to an SDL screen.
 *
 * @param filePath Path of the script file to be read and evaluated.
 */
void readMDLFile(const char * const filePath);

/*!
 *  @brief Read and evaluate the commands inside a script file.
 *
 * The commands are evaluated against an internal points ::Matrix_t, which is
 * rendered to an SDL screen.
 *
 *  @param filePath Path of the script file to be read and evaluated.
 */
void readScriptFile(const char * const filePath);
