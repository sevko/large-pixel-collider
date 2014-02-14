#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "screen.h"
#include "utils.h"

void testRasterizeLine(){
	int round;
	for(round = 0; round < 3000; round++){
		int line;
		for(line = 0; line < IMAGE_HEIGHT; line++)
			drawLine(0, line, IMAGE_WIDTH, line);
	}
}

int main(){
	configureScreen();
	testRasterizeLine();
	renderScreen();
	quitScreen();
	return EXIT_SUCCESS;
}
