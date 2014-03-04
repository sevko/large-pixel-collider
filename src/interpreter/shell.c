#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/keyboard/keyboard.h"

#include "src/globals.h"
#include "src/screen.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/shell_graphics.h"

#define TICK_PAUSE (1e6 / 60)
#define PRINTABLE(key) (32 <= key && key <= 126)

int g_running, g_enteringCommand, g_curX, g_curY;
char ** g_buffer;

void configureShell(){
	g_running = g_enteringCommand = 1;
	g_curX = g_curY = 0;

	g_buffer = malloc(sizeof(char *));
	g_buffer[0] = malloc(1);
	g_buffer[0][0] = '\0';
}

void shell(){
	configureShell();
	configureScreen();
	renderShell();
	Matrix_t * points = createMatrix(), * transform = createIdentity();

	int virtualY = 0,  // line number when scrolling through previous commands
		virtualChanged = 0; // whether user has scrolled

	char key;
	while(g_running){
		if((key = getkey()) != KEY_NOTHING){
			if(key == KEY_ENTER){
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
						break;
					}
					g_enteringCommand = 1;
				}

				g_curY++;
				g_curX = 0;
				g_buffer = realloc(g_buffer, (g_curY + 1) * sizeof(char *));
				g_buffer[g_curY] = malloc(1);
				g_buffer[g_curY][0] = '\0';
			}

			else if(PRINTABLE(key)){
				g_buffer[g_curY] = realloc(g_buffer[g_curY], g_curX + 2);
				g_buffer[g_curY][g_curX++] = key;
				g_buffer[g_curY][g_curX] = '\0';
			}

			else if(key == KEY_BACKSPACE){
				if(g_curX > 0)
					g_buffer[g_curY][--g_curX] = '\0';
			}

			else if(key == KEY_UP && 0 < virtualY){
				virtualY--;
				virtualChanged = 1;
				int virtualLen = strlen(g_buffer[virtualY]);
				g_curX = virtualLen;
				g_buffer[g_curY] = realloc(g_buffer[g_curY], virtualLen + 1);
				strcpy(g_buffer[g_curY], g_buffer[virtualY]);
			}

			else if(key == KEY_DOWN && virtualY < g_curY){
				virtualY++;
				virtualChanged = 1;
				int virtualLen = strlen(g_buffer[virtualY]);
				g_curX = virtualLen;
				g_buffer[g_curY] = realloc(g_buffer[g_curY], virtualLen + 1);
				strcpy(g_buffer[g_curY], g_buffer[virtualY]);
			}

			renderShell();
		}
		else if(!virtualChanged)
			virtualY = g_curY;

		usleep(TICK_PAUSE);
	}

	quitScreen();
}
