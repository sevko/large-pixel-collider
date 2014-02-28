/*
 *  engine.c is the graphic engine's main file; contains functions for testing
 *  functionality and startup.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "engine.h"
#include "matrix.h"
#include "screen.h"
#include "utils.h"

// function to test drawPolygon() and drawLine() functionality; renders
// concentric, rotating polygons with increasing radii and number of sides.
void testRasterizeLine(){
	configureScreen();
	int round;
	for(round = 0; round < 100; round++){
		clearScreen();
		usleep(1e6 / 100);
		int xOffset = IMAGE_WIDTH / 2, yOffset = IMAGE_HEIGHT / 2;
		int polygon, numPolygons = 20;
		for(polygon = 0; polygon < numPolygons; polygon++)
			drawPolygon(3 + polygon, 10 + polygon * 10, xOffset, yOffset, round * 0.4);
		renderScreen();
	}

	quitScreen();
}

// function to test matrix.c functions
void testMatrix(){
	configureScreen();

	Matrix_t * m1 = createMatrix();
	addPoint(m1, 10, 10, 20);
	addPoint(m1, 10, 90, 20);

	int tick;
	for(tick = 0; tick < 100; tick++){
		clearScreen();
		multiplyMatrix(createTranslation(0, 2, 2), m1);
		drawMatrixLines(m1);
		renderScreen();
		usleep(1e6 / 60);
	}

	freeMatrix(m1);
	quitScreen();
}

int main(){
	testMatrix();
	// testRasterizeLine();
	return EXIT_SUCCESS;
}
