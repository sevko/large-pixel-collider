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
	addPoint(m1, 100, 100, 0);
	addPoint(m1, 100, 130, 0);

	addPoint(m1, 100, 130, 0);
	addPoint(m1, 130, 130, 0);

	addPoint(m1, 130, 130, 0);
	addPoint(m1, 130, 100, 0);

	addPoint(m1, 130, 100, 0);
	addPoint(m1, 100, 100, 0);



	addPoint(m1, 100, 100, 30);
	addPoint(m1, 100, 130, 30);

	addPoint(m1, 100, 130, 30);
	addPoint(m1, 130, 130, 30);

	addPoint(m1, 130, 130, 30);
	addPoint(m1, 130, 100, 30);

	addPoint(m1, 130, 100, 30);
	addPoint(m1, 100, 100, 30);




	addPoint(m1, 100, 100, 0);
	addPoint(m1, 100, 100, 30);

	addPoint(m1, 100, 130, 0);
	addPoint(m1, 100, 130, 30);

	addPoint(m1, 130, 130, 0);
	addPoint(m1, 130, 130, 30);

	addPoint(m1, 130, 100, 0);
	addPoint(m1, 130, 100, 30);

	Matrix_t * scale = createScale(1.5, 1.5, 1.5);
	multiplyMatrix(scale, m1);
	freeMatrix(scale);

	int tick;
	for(tick = 0; tick < 400; tick++){
		clearScreen();
		Matrix_t * xRot = createRotation(X_AXIS, 0.2);
		Matrix_t * yRot = createRotation(Y_AXIS, 0.2);
		Matrix_t * zRot = createRotation(Z_AXIS, 0.2);
		multiplyMatrix(xRot, zRot);
		multiplyMatrix(yRot, zRot);
		multiplyMatrix(zRot, m1);
		drawMatrixLines(m1);

		freeMatrix(xRot);
		freeMatrix(yRot);
		freeMatrix(zRot);

		renderScreen();
		usleep(1e6 / 200);
	}

	freeMatrix(m1);
	quitScreen();
}

int main(){
	setup();
	testMatrix();
	testRasterizeLine();
	return EXIT_SUCCESS;
}
