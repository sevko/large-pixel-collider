/*
 *  shell_graphics.c contains functions to handle the interactive shell's
 *  terminal user-interface.
*/

#include <stdio.h>

#include "lib/xterm_control/xterm_control.h"
#include "src/interpreter/shell_graphics.h"

#define COLOR(color, string)(color string XT_CH_NORMAL)
#define LEFT_PADDING 2
#define PROMPT_STRING COLOR(XT_CH_RED, ">")

static void clearShellScreen();

extern int g_enteringCommand, g_curX, g_curY;
extern char ** g_buffer;

// draw the shell's text-buffer contents to the screen
void renderShell(){
	clearShellScreen();

	int line;
	for(line = 0; line <= g_curY; line++)
		printf("%s %s\n", PROMPT_STRING, g_buffer[line]);
	xt_par2(XT_SET_ROW_COL_POS, g_curY + 1, g_curX + LEFT_PADDING + 1);
}

// clear the terminal screen
static void clearShellScreen(){
	xt_par2(XT_SET_ROW_COL_POS, 0, 0);
	xt_par0(XT_CLEAR_SCREEN);
}
