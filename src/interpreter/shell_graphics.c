#include <stdio.h>

#include "lib/xterm_control/xterm_control.h"
#include "src/interpreter/shell_graphics.h"

#define COLOR(color, string)(color string XT_CH_NORMAL)
#define LEFT_PADDING 0

static void clearShellScreen();

extern int g_enteringCommand, g_curX, g_curY;
extern char ** g_buffer;

void renderShell(){
	clearShellScreen();

	int line;
	for(line = 0; line <= g_curY; line++){
		printf("%s\n", g_buffer[line]);
	}
	xt_par2(XT_SET_ROW_COL_POS, g_curY + 1, g_curX + LEFT_PADDING + 1);
}

static void clearShellScreen(){
	xt_par2(XT_SET_ROW_COL_POS, 0, 0);
	xt_par0(XT_CLEAR_SCREEN);
}
