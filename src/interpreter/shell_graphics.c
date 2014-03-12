/*
 *  shell_graphics.c contains functions to handle the interactive shell's
 *  terminal user-interface.
*/

#include <ncurses.h>
#include <stdlib.h>
#include "src/interpreter/shell_graphics.h"

// used to render a prompt character on the shell command-line
#define PROMPT_ARGS_OUTPUT "     "
#define PROMPT_CMD  "CMD: "
#define PROMPT_COLOR_NUM (LINE_TYPE_OUTPUT + 1)
#define PROMPT_PADDING 5

extern int g_currLineType, g_curX, g_curY;
extern char ** g_buffer;

typedef struct {
	char * line;
	int type;
} VisualLine_t;

static VisualLine_t * g_visualBuffer;
static int g_numVisualLines, g_visualY, g_hasColor;

// allocate shell graphics, initialize global variables
void configureGraphicsShell(){
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

// draw the shell's text-buffer contents to the screen
void renderShell(){
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

// print line of type with appropriate prompt and, if possible, colors
void printLine(char * line, int type){
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

// add a VisualLine_t with line of type to the shell's visual buffer
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

// deallocate memory used by shell graphics
void freeGraphicsShell(){
	endwin();

	int line;
	for(line = 0; line < g_numVisualLines; line++)
		free(g_visualBuffer[line].line);
	free(g_visualBuffer);
}
