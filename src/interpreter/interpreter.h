#pragma once

#include "src/matrix.h"

// status of evaluation of a single command
#define VALID_EVAL 0
#define INVALID_CMD 1
#define INVALID_ARGS 2

int evaluateCommand(char ** const command, Matrix_t * const points,
	Matrix_t ** transform);
int argsRequired(char cmd);
