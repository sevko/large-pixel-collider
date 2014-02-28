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
	Matrix_t * m1 = createMatrix();
	addTransformPoint(m1, 11, 12, 13, 92);
	addTransformPoint(m1, 41, 12, 63, 12);
	addTransformPoint(m1, 18, 52, 93, 58);
	addTransformPoint(m1, 71, 22, 13, 94);

	Matrix_t * m2 = createMatrix();
	addTransformPoint(m2, 11, 12, 13, 92);
	addTransformPoint(m2, 18, 52, 93, 58);
	addTransformPoint(m2, 71, 22, 13, 94);
	addTransformPoint(m2, 41, 12, 63, 12);

	multiplyMatrix(m1, m2);
	printMatrix(m2);
}

int main(){
	testMatrix();
	// testRasterizeLine();
	return EXIT_SUCCESS;
}
