#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "screen.h"
#include "utils.h"

void testRasterizeLine(){
	drawLine(0, 0, IMAGE_HEIGHT - 1, IMAGE_WIDTH - 1);
}

int main(){
	configureScreen();
	testRasterizeLine();
	renderScreen();
	quitScreen();
	return EXIT_SUCCESS;
}
