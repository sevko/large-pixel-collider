#pragma once

#define LINE_TYPE_ARGS 1
#define LINE_TYPE_CMD 2
#define LINE_TYPE_ERROR 3
#define LINE_TYPE_OUTPUT 4

void configureGraphicsShell();
void renderShell();
void printLine(char * line, int type);
void addVisualLine(char * line, int type);
void freeGraphicsShell();
