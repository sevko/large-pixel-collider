#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/keyboard/keyboard.h"

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
static void evaluateNewline();
static void scrollUp();
static void scrollDown();
static void backspace();

int g_curX, g_curY;     // cursor position in the buffer
char ** g_buffer;       // shell text-buffer

static int g_running,   // indicates whether shell is still running
	g_enteringCommand;  // whether user is entering a command, or arguments

static int g_virtualY;  // when scrolling, line number of line last accessed

static Matrix_t * points, * transform;

// main shell loop
void shell(){
	configureShell();

	char key;
	while(g_running){
		if((key = getkey()) != KEY_NOTHING){
			if(PRINTABLE(key))
				insertChar(key);

			else
				switch(key){
					case KEY_ENTER:
						evaluateNewline();
						break;

					case KEY_UP:
						scrollUp();
						break;

					case KEY_DOWN:
						scrollDown();
						break;

					case KEY_BACKSPACE:
						backspace();
						break;

					case KEY_END:
						g_running = 0;
						break;
				}

			renderShell();
		}

		usleep(TICK_PAUSE);
	}

	freeShell();
}

// initialize all global variables, allocate g_buffer memory
static void configureShell(){
	configureScreen();
	g_running = g_enteringCommand = 1;
	g_curX = g_curY = 0;
	g_virtualY = -1;

	g_buffer = malloc(sizeof(char *));
	g_buffer[0] = malloc(1);
	g_buffer[0][0] = '\0';

	points = createMatrix();
	transform = createIdentity();
	renderShell();
}

// deallocate all memory used by shell
static void freeShell(){
	int line;
	for(line = 0; line <= g_curY; line++)
		free(g_buffer[line]);
	free(g_buffer);
	freeMatrices(2, points, transform);
	quitScreen();
}

// insert char key at the current cursor position
static void insertChar(char key){
	g_buffer[g_curY] = realloc(g_buffer[g_curY], g_curX + 2);
	g_buffer[g_curY][g_curX++] = key;
	g_buffer[g_curY][g_curX] = '\0';
}

// evaluate a line entered in the shell: if the line is a command and arguments
// are necessary, set the appropriate flags; otherwise, evaluate the command,
// with arguments if necessary.
static void evaluateNewline(){
	if(g_enteringCommand && argsRequired(g_buffer[g_curY][0]))
		g_enteringCommand = 0;
	else {
		int lnWithCmd;
		if(g_enteringCommand)
			lnWithCmd = g_curY;
		else
			lnWithCmd = g_curY - 1;

		int status = evaluateCommand(&g_buffer[lnWithCmd], points, &transform);
		if(status != VALID_EVAL){
			switch(status){
				case INVALID_CMD:
					ERROR("Error, invalid command: %c.",
						g_buffer[lnWithCmd][0]);
					break;

				case INVALID_ARGS:
					ERROR("Error, invalid arguments: %s",
						g_buffer[g_curY]);
					break;
			}
			ERROR("Exiting.");
		}
		g_enteringCommand = 1;
	}

	g_curY++;
	g_curX = 0;
	g_buffer = realloc(g_buffer, (g_curY + 1) * sizeof(char *));
	g_buffer[g_curY] = malloc(1);
	g_buffer[g_curY][0] = '\0';
}

// scroll back through the shell's command history.
static void scrollUp(){
	// if the user hasn't scrolled yet
	if(g_virtualY == -1){
		g_virtualY = g_curY;
		return;
	}

	if(g_virtualY <= 0)
		return;

	g_virtualY--;
	// g_virtualChanged = 1;
	int virtualLen = strlen(g_buffer[g_virtualY]);
	g_curX = virtualLen;
	g_buffer[g_curY] = realloc(g_buffer[g_curY], virtualLen + 1);
	strcpy(g_buffer[g_curY], g_buffer[g_virtualY]);
}

// scroll forward through the shell's command history
static void scrollDown(){
	// if the user hasn't scrolled yet
	if(g_virtualY == -1){
		g_virtualY = g_curY;
		return;
	}

	if(g_curY <= g_virtualY)
		return;

	g_virtualY++;
	// g_virtualChanged = 1;
	int virtualLen = strlen(g_buffer[g_virtualY]);
	g_curX = virtualLen;
	g_buffer[g_curY] = realloc(g_buffer[g_curY], virtualLen + 1);
	strcpy(g_buffer[g_curY], g_buffer[g_virtualY]);
}

// delete the preceding character
static void backspace(){
	if(g_curX > 0)
		g_buffer[g_curY][--g_curX] = '\0';
}
