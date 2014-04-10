/*!
 *  @file interpreter.h
 *  @brief Functions for interpreting commands in the graphics engine's
 *      scripting quasi-language.
 */

#pragma once

#include "src/matrix.h"

/*!
 *  @brief Returned by evaluateCommand() if a command was "special," or specific
 *      to the shell (see HELP_CMD and EXIT_CMD).
 */
#define CMD_SPECIAL 0

/*!
 *  @brief Returned by evaluateCommand() to indicate successfull evaluation of
 *      a command.
 */
#define CMD_VALID_EVAL 1

/*!
 *  @brief Returned by evaluateCommand() to indicate failed evaluation of a
 *  command due to an invalid command.
 */
#define CMD_INVALID_CMD 2

/*!
 *  @brief Returned by evaluateCommand() to indicate failed evaluation of a
 *  command due to invalid arguments.
 */
#define CMD_INVALID_ARGS 3

/*!
 *  @brief Command to print help information in the shell.
 */
#define HELP_CMD 'H'

/*!
 *  @brief Command to exit the shell.
 */
#define EXIT_CMD 'e'

/*!
 *  @brief Path of the file whose contents will be printed to the shell in
 *      response to receipt of the HELP_CMD command.
 */
#define HELP_FILE_PATH "doc/shell_help.txt"

/*!
 *  @brief Evaluate a command, modifying a points ::Matrix_t as necessary, and
 *      return an indication of successful or failed execution.
 *
 *  Check the command at @a command[0] -- if arguments are necessary, pull them
 *  from @a command[1] (evaluateCommand() is called only after a command with
 *  any required arguments is entered). If the command acts on the ::Matrix_t
 *  @p points or ::Matrix_t @p transform, manipulate them accordingly.
 *
 *  @param command A pointer to a pointer to the line containing the command
 *      char, which is necessary in case arguments are required and the
 *      subsequent line in the script's line buffer consequently needs be
 *      accessed.
 *  @param points A pointer to a pointer to the points ::Matrix_t, which may or
 *      may not be modified by the command.
 *  @param transform A pointer to a pointer to the transform ::Matrix_t, which
 *      may or may not be modified by the command.
 *
 *  @return Return CMD_SPECIAL, CMD_VALID_EVAL, CMD_INVALID_CMD, or
 *      CMD_INVALID_ARGS to indicate the status of the command evaluation.
 */
int evaluateCommand(char ** const command, Matrix_t ** points,
	Matrix_t ** transform);

/*!
 *  @brief Indicate whether the command requires arguments.
 *
 *  @param cmd The command char to check.
 *
 *  @return If the command specified by @p cmd requires arguments, return 1;
 *      otherwise, return 0.
 */
int argsRequired(char cmd);
