#include <stdio.h>

#include "lib/xterm_control/xterm_control.h"

static void clearScreen();

void render(char ** buffer, int numlines){
	clearScreen();
	int line;
	for(line = 0; line < numlines; line++)
		printf("\t%s\n", buffer[line]);
}

static void clearScreen(){
	xt_par2(XT_SET_ROW_COL_POS, 0, 0);
	xt_par0(XT_CLEAR_SCREEN);
}
