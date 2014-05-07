#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/interpreter/utils.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/shell.h"
#include "src/interpreter/shell_graphics.h"
#include "src/interpreter/stack/stack.h"

/*!
 *  @brief Indicate whether char is printable (alphanumeric, a symbol, etc).
 *
 *  @param key The char to check.
 *
 *  @return If @p key is printable, return 1; otherwise, return 0.
 */
#define PRINTABLE(key) (32 <= key && key <= 126)

/*!
 *  @brief Initialize all global variables, allocate memory for buffers. Must be
 *      called before shell usage.
 */
static void configureShell(void);

/*!
 *  @brief Deallocate all the memory belonging to the shell. Should be called
 *      after shell use, or memory leaks will occur.
 */
static void freeShell(void);

/*!
 *  @brief Insert a character at the current cursor position.
 *
 *  @param key The character to insert.
 */
static void insertChar(char key);

/*!
 *  @brief If possible (ie, the cursor is not at the beginning of the line),
 *      delete the character before the current cursor position, and move the
 *      cursor back.
 */
static void backspace(void);

/*!
 *  @brief Delete the character at the current cursor position.
 */
static void deleteChar(void);

/*!
 *  @brief Evaluate the text buffer after the user enters a new line.
 *
 *  1. If the line contains a command that requires no arguments, execute
 *      the command.
 *  2. If the line contains a command that requires arguments, set the flags
 *      that indicate expected arguments.
 *  3. If arguments are expected and the current line contains valid
 *      arguments, evaluate the previously entered command.
 */
static void evaluateNewline(void);

/*!
 *  @brief Scroll up through the command line history, loading the currently
 *      visible past entry into the user's command-line.
 */
static void scrollUp(void);

/*!
 *  @brief Scroll down through the command line history, loading the currently
 *      visible, past entry into the user's command line.
 */
static void scrollDown(void);

/*!
 *  @brief If the cursor isn't at the beginning of the current line, move it
 *      left.
 *
 *  @return 1 if the cursor was successfully moved (ie, wasn't at the beginning
 *      of the line); 0 otherwise.
 */
static int moveLeft(void);

/*!
 *  @brief If the cursor isn't at the end of the current line, move it right.
 */
static void moveRight(void);

/*!
 *  @brief Add help information (from the file named HELP_FILE_PATH) to the
 *      shell's visual buffer.
 */
static void renderHelp(void);

/*!
 *  @brief The x and y coordinates of the cursor relative to the g_buffer.
 */
int g_curX, g_curY;

/*!
 *  @brief Indicates the contents -- a command or arguments -- of the line
 *      currently being written by the user.
 */
int g_currLineType;

/*!
 *  @brief Buffer for all commands entered by user.
 */
char ** g_buffer;

/*!
 *  @brief 1 if the shell is still running, 0 if not.
 */
static int g_running;

/*!
 *  @brief The last line scrolled to in the shell's command-line history.
 */
static int g_virtualY;

/*!
 *  @brief Buffer for the line being currently written, so as to preserve it
 *      when scrolling through the command-line history.
 */
static char * g_tempCurrLine;

/*!
 *  @brief The two matrices that the user manipulates from the shell.
 *
 *  @a points contains the current points ::Matrix_t, while @a transform
 *      contains the current transformation ::Matrix_t.
 */
static Matrix_t * points, * transform;

static Stack_t * g_coordStack;

void shell(void){
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
	}

	freeShell();
}

static void configureShell(void){
	g_running = 1;
	g_currLineType = LINE_TYPE_CMD;
	g_curX = g_curY = 0;
	g_virtualY = -1;

	g_buffer = malloc(sizeof(char *));
	g_buffer[0] = malloc(1);
	g_buffer[0][0] = '\0';
	g_tempCurrLine = NULL;

	points = createMatrix();
	transform = createIdentity();

	g_coordStack = createStack();

	configureGraphicsShell();
	renderShell();
	configureScreen();
}

static void freeShell(void){
	int line;
	for(line = 0; line <= g_curY; line++)
		free(g_buffer[line]);
	free(g_buffer);

	if(g_virtualY != -1)
		free(g_tempCurrLine);
	freeMatrices(2, points, transform);

	freeGraphicsShell();
	freeStack(g_coordStack);
	quitScreen();
}

static void insertChar(char key){
	int len = strlen(g_buffer[g_curY]);
	g_buffer[g_curY] = realloc(g_buffer[g_curY], len + 2);

	int ind;
	for(ind = len + 1; g_curX < ind; ind--)
		g_buffer[g_curY][ind] = g_buffer[g_curY][ind - 1];

	g_buffer[g_curY][g_curX++] = key;
}

static void backspace(void){
	if(moveLeft())
		deleteChar();
}

static void deleteChar(void){
	int len = strlen(g_buffer[g_curY]);

	int ind;
	for(ind = g_curX; ind < len; ind++)
		g_buffer[g_curY][ind] = g_buffer[g_curY][ind + 1];
}

static void evaluateNewline(void){
	char * visualLine = malloc(strlen(g_buffer[g_curY]) + 1);
	strcpy(visualLine, g_buffer[g_curY]);
	addVisualLine(visualLine, g_currLineType);

	if(g_currLineType == LINE_TYPE_CMD && argsRequired(g_buffer[g_curY][0]))
		g_currLineType = LINE_TYPE_ARGS;

	else {
		int lnWithCmd;
		if(g_currLineType == LINE_TYPE_CMD)
			lnWithCmd = g_curY;
		else
			lnWithCmd = g_curY - 1;

		int status = evaluateCommand(&g_buffer[lnWithCmd], &points, &transform,
				g_coordStack);
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
					sprintf(errMsg, "Invalid command: %c. Enter 'h' for help.",
						g_buffer[lnWithCmd][0]);
				else
					sprintf(errMsg, "Invalid arguments: %s. Enter 'h' for help.",
						g_buffer[g_curY]);

				addVisualLine(errMsg, LINE_TYPE_ERROR);
			}
		}
		g_currLineType = LINE_TYPE_CMD;
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

static void scrollUp(void){
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

static void scrollDown(void){
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

static int moveLeft(void){
	if(0 < g_curX){
		g_curX--;
		return 1;
	}
	else
		return 0;
}

static void moveRight(void){
	if(g_curX < (int)strlen(g_buffer[g_curY]))
		g_curX++;
}

static void renderHelp(void){
	ScannedFile_t * file = readFile(HELP_FILE_PATH);
	int line;
	for(line = 0; line < file->numLines; line++){
		file->buffer[line][strlen(file->buffer[line]) - 1] = '\0';
		addVisualLine(file->buffer[line], LINE_TYPE_OUTPUT);
	}
	free(file);
}
