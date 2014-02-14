#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "screen.h"
#include "utils.h"

void testRasterizeLine(){
	int round, numRounds = 3000;
	for(round = 0; round < numRounds; round++){
		int line;
		for(line = 0; line < IMAGE_HEIGHT; line++)
			drawLine(0, line, IMAGE_WIDTH - 1, line);
	}
}

int main(){
	configureScreen();
	testRasterizeLine();
	renderScreen();
	quitScreen();
	return EXIT_SUCCESS;
}
