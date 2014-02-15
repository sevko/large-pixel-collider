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

	Matrix_t * matrix = createMatrix();
	printMatrix(matrix);

	addPoint(matrix, 0, 0, 0);
	addPoint(matrix, 100, 20, 0);
	addPoint(matrix, 100, 20, 0);
	addPoint(matrix, 50, 60, 0);
	addEdge(matrix, 50, 60, 0, 0, 0, 0);

	printMatrix(matrix);
	drawMatrixLines(matrix);
	freeMatrix(matrix);

	renderScreen();
	quitScreen();
}

int main(){
	testMatrix();
	testRasterizeLine();
	return EXIT_SUCCESS;
}
