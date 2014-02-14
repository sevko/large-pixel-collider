#include <stdio.h>
#include <stdlib.h>

#include "screen.h"

int main(){
	configureScreen();
	drawPixel(10, 10, 0x00FF0000);
	renderScreen();
	quitScreen();
	return EXIT_SUCCESS;
}
