#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "engine.h"
#include "screen.h"
#include "utils.h"

void testRasterizeLine(){
	int round;
	for(round = 0; round < 1000; round++){
		clearScreen();
		usleep(1e6 / 100);
		int xOffset = IMAGE_WIDTH / 2, yOffset = IMAGE_HEIGHT / 2;
		int polygon, numPolygons = 10;
		for(polygon = 0; polygon < numPolygons; polygon++)
			drawPolygon(3 + polygon, 10 + polygon * 20, xOffset, yOffset, round * 0.5);
		renderScreen();
	}
}

int main(){
	configureScreen();
	testRasterizeLine();
	quitScreen();
	return EXIT_SUCCESS;
}
