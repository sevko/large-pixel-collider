#pragma once

#include "src/matrix.h"

// status of evaluation of a single command
#define CMD_SPECIAL 0
#define CMD_VALID_EVAL 1
#define CMD_INVALID_CMD 2
#define CMD_INVALID_ARGS 3

#define HELP_CMD 'h'
#define EXIT_CMD 'e'

#define HELP_FILE_PATH "doc/shell_help.txt"

int evaluateCommand(char ** const command, Matrix_t ** points,
	Matrix_t ** transform);
int argsRequired(char cmd);
