/*!
 * @file
 * @brief Functions for evaluating the commands contained in an MDL script.
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
