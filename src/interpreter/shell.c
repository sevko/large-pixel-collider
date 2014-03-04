#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/keyboard/keyboard.h"

#include "src/globals.h"
#include "src/globals.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/shell_graphics.h"

#define TICK_PAUSE (1e6 / 60)
#define PRINTABLE(key) (32 <= key && key <= 126)

void shell(){
	// configureScreen();

	int running = 1, enteringCommand = 1;
	int curX = 0, curY = 0;

	char key, ** buffer = malloc(sizeof(char *));
	buffer[0] = malloc(1);
	buffer[0][0] = '\0';

	Matrix_t * points = createMatrix(), * transform = createIdentity();

	while(running){
		if((key = getkey()) != KEY_NOTHING){
			if(key == KEY_ENTER){
				if(enteringCommand && argsRequired(buffer[curY][0]))
					enteringCommand = 0;
				else {
					int lnWithCmd;
					if(enteringCommand)
						lnWithCmd = curY;
					else
						lnWithCmd = curY - 1;

					int status = evaluateCommand(&buffer[lnWithCmd], points, &transform);
					if(status != VALID_EVAL){
						switch(status){
							case INVALID_CMD:
								ERROR("Error, invalid command: %c.",
									buffer[lnWithCmd][0]);
								break;

							case INVALID_ARGS:
								ERROR("Error, invalid arguments: %s",
									buffer[curY]);
								break;
						}
						ERROR("Exiting.");
						break;
					}
					enteringCommand = 1;
				}

				curY++;
				curX = 0;
				buffer = realloc(buffer, (curY + 1) * sizeof(char *));
				buffer[curY] = malloc(1);
				buffer[curY][0] = '\0';
			}

			else if(PRINTABLE(key)){
				buffer[curY] = realloc(buffer[curY], curX + 2);
				buffer[curY][curX++] = key;
				buffer[curY][curX] = '\0';
			}
			render(buffer, curY + 1);
		}
		usleep(TICK_PAUSE);
	}
	// quitScreen();
}
