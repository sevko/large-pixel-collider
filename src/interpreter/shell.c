/*
 *  shell.c contains functions for initializing and running an interactive shell
 *  that allows the user to manipulate the graphics engine with commands.
*/

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/screen.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/shell.h"
#include "src/interpreter/shell_graphics.h"

#define TICK_PAUSE (1e6 / 60)
#define PRINTABLE(key) (32 <= key && key <= 126)

static void configureShell();
static void freeShell();
static void insertChar(char key);
static void backspace();
static void deleteChar();
static void evaluateNewline();
static void scrollUp();
static void scrollDown();
static int moveLeft();
static void moveRight();
static void renderHelp();

int g_curX, g_curY,     // cursor position in the buffer
	g_enteringCommand;  // whether user is entering a command, or arguments
char ** g_buffer;       // buffer for text entered by user

static int g_running;   // whether the shell is still running

static int g_virtualY;  // last line scrolled to in the command-line history
static char * g_tempCurrLine;   // store current (last) line when scrolling

static Matrix_t * points, * transform;

// main shell loop
void shell(){
	configureShell();

	int key;
	while(g_running){
		key = getch();

		if(PRINTABLE(key))
			insertChar(key);

		else
			switch(key){
				case KEY_ENTER: case '\n':
					evaluateNewline();
					break;

				case KEY_UP:
					scrollUp();
					break;

				case KEY_DOWN:
					scrollDown();
					break;

				case KEY_LEFT:
					moveLeft();
					break;

				case KEY_RIGHT:
					moveRight();
					break;

				case KEY_BACKSPACE:
					backspace();
					break;

				case KEY_END:
					g_running = 0;
					break;
			}

		renderShell();
		usleep(TICK_PAUSE);
	}

	freeShell();
}

// initialize all global variables, allocate g_buffer memory
static void configureShell(){
	g_running = g_enteringCommand = 1;
	g_curX = g_curY = 0;
	g_virtualY = -1;

	g_buffer = malloc(sizeof(char *));
	g_buffer[0] = malloc(1);
	g_buffer[0][0] = '\0';
	g_tempCurrLine = NULL;

	points = createMatrix();
	transform = createIdentity();

	configureGraphicsShell();
	renderShell();
	// configureScreen();
}

// deallocate all memory used by shell
static void freeShell(){
	int line;
	for(line = 0; line <= g_curY; line++)
		free(g_buffer[line]);
	free(g_buffer);

	if(g_virtualY != -1)
		free(g_tempCurrLine);
	freeMatrices(2, points, transform);

	freeGraphicsShell();
	// quitScreen();
}

// insert char key at the current cursor position
static void insertChar(char key){
	int len = strlen(g_buffer[g_curY]);
	g_buffer[g_curY] = realloc(g_buffer[g_curY], len + 2);

	int ind;
	for(ind = len + 1; g_curX < ind; ind--)
		g_buffer[g_curY][ind] = g_buffer[g_curY][ind - 1];

	g_buffer[g_curY][g_curX++] = key;
}

// delete the preceding character
static void backspace(){
	if(moveLeft())
		deleteChar();
}

// deletes the character at the current cursor position
static void deleteChar(){
	int len = strlen(g_buffer[g_curY]);

	int ind;
	for(ind = g_curX; ind < len; ind++)
		g_buffer[g_curY][ind] = g_buffer[g_curY][ind + 1];
}

// evaluate a line entered in the shell: if the line is a command and arguments
// are necessary, set the appropriate flags; otherwise, evaluate the command,
// with arguments if necessary.
static void evaluateNewline(){
	char * visualLine = malloc(strlen(g_buffer[g_curY]) + 1);
	strcpy(visualLine, g_buffer[g_curY]);
	addVisualLine(visualLine, (g_enteringCommand)?LINE_TYPE_CMD:LINE_TYPE_ARGS);

	if(g_enteringCommand && argsRequired(g_buffer[g_curY][0]))
		g_enteringCommand = 0;

	else {
		int lnWithCmd;
		if(g_enteringCommand)
			lnWithCmd = g_curY;
		else
			lnWithCmd = g_curY - 1;

		int status = evaluateCommand(&g_buffer[lnWithCmd], points, &transform);
		if(status != CMD_VALID_EVAL){
			if(status == CMD_SPECIAL){
				if(g_buffer[lnWithCmd][0] == EXIT_CMD)
					g_running = 0;
				else
					renderHelp();
			}

			else {
				char * errMsg = malloc(1000);

				if(status == CMD_INVALID_CMD)
					sprintf(errMsg, "Invalid command: %c.",
						g_buffer[lnWithCmd][0]);
				else
					sprintf(errMsg, "Invalid arguments: %s", g_buffer[g_curY]);

				addVisualLine(errMsg, LINE_TYPE_ERROR);
			}
		}
		g_enteringCommand = 1;
	}

	g_curY++;
	g_curX = 0;

	if(g_virtualY != -1){
		g_virtualY = -1;
		free(g_tempCurrLine);
		g_tempCurrLine = NULL;
	}

	g_buffer = realloc(g_buffer, (g_curY + 1) * sizeof(char *));
	g_buffer[g_curY] = malloc(1);
	g_buffer[g_curY][0] = '\0';
}

// scroll back through the shell's command history.
static void scrollUp(){
	// if the user hasn't scrolled yet
	if(g_virtualY == -1){
		g_virtualY = g_curY;
		g_tempCurrLine = malloc(strlen(g_buffer[g_curY]) + 2);
		strcpy(g_tempCurrLine, g_buffer[g_curY]);
	}

	if(g_virtualY <= 0)
		return;

	g_virtualY--;
	int virtualLen = strlen(g_buffer[g_virtualY]);
	g_curX = virtualLen;
	g_buffer[g_curY] = realloc(g_buffer[g_curY], virtualLen + 1);
	strcpy(g_buffer[g_curY], g_buffer[g_virtualY]);
}

// scroll forward through the shell's command history
static void scrollDown(){
	// if the user hasn't scrolled yet
	if(g_virtualY == -1)
		return;

	if(g_curY <= g_virtualY)
		return;

	g_virtualY++;
	if(g_virtualY == g_curY){
		int lenTempLine = strlen(g_tempCurrLine);
		g_buffer[g_curY] = realloc(g_buffer[g_curY], lenTempLine + 1);
		strcpy(g_buffer[g_curY], g_tempCurrLine);

		g_curX = lenTempLine;
		free(g_tempCurrLine);
		g_tempCurrLine = NULL;
		g_virtualY = -1;
	}
	else {
		int virtualLen = strlen(g_buffer[g_virtualY]);
		g_curX = virtualLen;
		g_buffer[g_curY] = realloc(g_buffer[g_curY], virtualLen + 1);
		strcpy(g_buffer[g_curY], g_buffer[g_virtualY]);
	}
}

// move cursor left if possible, and return int indicating success or failure
// (mainly for use by delete())
static int moveLeft(){
	if(0 < g_curX){
		g_curX--;
		return 1;
	}
	else
		return 0;
}

// move cursor right, if possible
static void moveRight(){
	if(g_curX < (int)strlen(g_buffer[g_curY]))
		g_curX++;
}

static void renderHelp(){
	const int maxLineLen = 1000;

	FILE * file = fopen(HELP_FILE_PATH, "r");
	if(file == NULL){
		ERROR("Failed to open file \"%s\"", HELP_FILE_PATH);
		exit(EXIT_FAILURE);
	}

	char * line = malloc(maxLineLen);
	while(fgets(line, maxLineLen, file) != NULL){
		line[strlen(line) - 1] = '\0';  // remove newline
		addVisualLine(line, LINE_TYPE_OUTPUT);
		line = malloc(maxLineLen);
	}

	fclose(file);
}
