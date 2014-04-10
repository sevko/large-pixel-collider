#include <ncurses.h>
#include <stdlib.h>
#include "src/interpreter/shell_graphics.h"

//! Prompt displayed for lines containing arguments.
#define PROMPT_ARGS_OUTPUT "     "

//! Prompt displayed for lines containing a command.
#define PROMPT_CMD  "CMD: "

//! The color of the prompt.
#define PROMPT_COLOR_NUM (LINE_TYPE_OUTPUT + 1)

//! The character width of the prompt.
#define PROMPT_PADDING 5

extern int g_currLineType, g_curX, g_curY;
extern char ** g_buffer;

//! @brief A single line of text in the shell's visual buffer.
typedef struct {
	char * line; //!< The text contained in the line.
	/*!
	 * The type of line (see LINE_TYPE_ARGS, LINE_TYPE_CMD, LINE_TYPE_ERROR,
	 * LINE_TYPE_OUTPUT).
	*/
	int type;
} VisualLine_t;

/*!
 *  @brief Print a line to the console with, if possible, color appropriate for
 *      its type.
 *
 *  @param line The char buffer to print.
 *  @param type The type of the line (see LINE_TYPE_ARGS, LINE_TYPE_CMD,
 *      LINE_TYPE_ERROR, LINE_TYPE_OUTPUT).
 */
static void printLine(char * line, int type);

/*!
 *  @brief The shell's visual buffer -- contains everything in g_buffer, in
 *      addition to any lines output by the shell (ie, error or help messages).
 */
static VisualLine_t * g_visualBuffer;

//! The number of ::VisualLine_t in g_visualBuffer.
static int g_numVisualLines;

//! The line number from which to begin rendering g_visualBuffer.
static int g_visualY;

//! 1 if the terminal supports color; 0 otherwise.
static int g_hasColor;

void configureGraphicsShell(void){
	initscr();
	raw();
	nl();
	scrollok(stdscr, TRUE);
	keypad(stdscr, TRUE);

	if((g_hasColor = has_colors()) == TRUE){
		start_color();
		init_pair(LINE_TYPE_ARGS, COLOR_CYAN, COLOR_BLACK);
		init_pair(LINE_TYPE_CMD, COLOR_BLUE, COLOR_BLACK);
		init_pair(LINE_TYPE_ERROR, COLOR_RED, COLOR_BLACK);
		init_pair(LINE_TYPE_OUTPUT, COLOR_WHITE, COLOR_BLACK);
		init_pair(PROMPT_COLOR_NUM, COLOR_GREEN, COLOR_BLACK);
	}

	g_numVisualLines = 0;
	g_visualY = 0;
}

void renderShell(void){
	clear();

	// draw static contents of the visual buffer
	int line;
	for(line = 0; line < g_numVisualLines; line++){
		// print prompt with color
		printLine(g_visualBuffer[line].line, g_visualBuffer[line].type);
		printw("\n");
	}

	// draw the line being currently entered straight from the g_buffer,
	// as it's constantly changing
	printLine(g_buffer[g_curY], g_currLineType);

	move(g_visualY, g_curX + PROMPT_PADDING);
}

void addVisualLine(char * line, int type){
	g_visualBuffer = realloc(g_visualBuffer, ++g_numVisualLines *
		sizeof(VisualLine_t));

	g_visualBuffer[g_numVisualLines - 1] = (VisualLine_t){
		.line = line,
		.type = type
	};
	g_visualY++;

	int ind = 0;
	while(line[ind])
		if(line[ind++] == '\n')
			g_visualY++;
}

void freeGraphicsShell(void){
	endwin();

	int line;
	for(line = 0; line < g_numVisualLines; line++)
		free(g_visualBuffer[line].line);
	free(g_visualBuffer);
}

static void printLine(char * line, int type){
	if(g_hasColor){
		attron(COLOR_PAIR(PROMPT_COLOR_NUM));
		printw("%s", (type == LINE_TYPE_CMD)?PROMPT_CMD:PROMPT_ARGS_OUTPUT);
		attroff(COLOR_PAIR(PROMPT_COLOR_NUM));

		attron(COLOR_PAIR(type));
		printw("%s", line);
		attroff(COLOR_PAIR(type));
	}
	else
		printw("%s%s", (type == LINE_TYPE_CMD)?PROMPT_CMD:PROMPT_ARGS_OUTPUT,
			line);
}
