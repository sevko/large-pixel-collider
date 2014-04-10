/*!
 *  @file
 *  @brief Functions for rendering the console shell; a complement to shell.h.
 */

#pragma once

//! @brief Indicates a VisualLine_t in g_visualBuffer contains arguments.
#define LINE_TYPE_ARGS 1

//! @brief Indicates a VisualLine_t in g_visualBuffer contains a command.
#define LINE_TYPE_CMD 2

//! @brief Indicates a VisualLine_t in g_visualBuffer contains an error message.
#define LINE_TYPE_ERROR 3

//! @brief Indicates a VisualLine_t in g_visualBuffer contains shell output.
#define LINE_TYPE_OUTPUT 4

/*!
 *  @brief Initialize all global variables, and allocate memory required by
 *      the shell's rendering functions. Must be called before other functions
 *      other shell_graphics functions are used.
 */
void configureGraphicsShell(void);

/*!
 *  @brief Render the contents of g_visualBuffer to the console.
 */
void renderShell(void);

/*!
 *  @brief Add a line of text to g_visualBuffer.
 *
 *  @param line The char buffer to be added.
 *  @param type The type of the line (see LINE_TYPE_ARGS, LINE_TYPE_CMD,
 *      LINE_TYPE_ERROR, LINE_TYPE_OUTPUT).
 */
void addVisualLine(char * line, int type);

/*!
 *  @brief Deallocate all memory used by the shell.
 */
void freeGraphicsShell(void);
