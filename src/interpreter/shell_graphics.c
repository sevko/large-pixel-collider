/*
 *  shell_graphics.c contains functions to handle the interactive shell's
 *  terminal user-interface.
*/

#include <ncurses.h>
#include <stdlib.h>
#include "src/interpreter/shell_graphics.h"

// used to render a prompt character on the shell command-line
#define LEFT_PADDING 2
#define PROMPT_STRING "> "

extern int g_enteringCommand, g_curX, g_curY;
extern char ** g_buffer;

static char ** g_visualBuffer;  // buffer containing text to appear in the shell
static int g_numVisualLines, g_visualY;

// allocate shell graphics, initialize global variables
void configureGraphicsShell(){
	initscr();
	raw();
	nl();
	scrollok(stdscr, TRUE);
	keypad(stdscr, TRUE);
	g_numVisualLines = 0;
	g_visualY = 0;
}

// draw the shell's text-buffer contents to the screen
void renderShell(){
	clear();

	// draw static contents of the visual buffer
	int line;
	for(line = 0; line < g_numVisualLines; line++)
		printw("%s%s\n", PROMPT_STRING, g_visualBuffer[line]);

	// draw the line being currently entered straight from the g_buffer,
	// as it's constantly changing
	printw("%s%s", PROMPT_STRING, g_buffer[g_curY]);

	move(g_visualY, g_curX + LEFT_PADDING);
}

// add a line to the shell's visual buffer
void addVisualLine(char * line){
	g_visualBuffer = realloc(g_visualBuffer, ++g_numVisualLines *
		sizeof(char *));
	g_visualBuffer[g_numVisualLines - 1] = line;
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
		free(g_visualBuffer[line]);
	free(g_visualBuffer);
}
