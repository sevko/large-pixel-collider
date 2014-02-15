#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "utils.h"

Matrix_t * createMatrix(){
	Matrix_t * matrix = malloc(sizeof(Matrix_t));
	matrix->numPoints = 0;

	matrix->points[0] = matrix->points[1] = matrix->points[2] = matrix->points[3] = NULL;

	return matrix;
}

void freeMatrix(Matrix_t * matrix){
	int varPtr;
	for(varPtr = 0; varPtr < NUM_POINT_VARS; varPtr++)
		free(matrix->points[varPtr]);
	free(matrix);
}

void expandMatrix(Matrix_t * matrix){
	matrix->numPoints++;
	int varPtr;
	for(varPtr = 0; varPtr < NUM_POINT_VARS; varPtr++)
		matrix->points[varPtr] = realloc(matrix->points[varPtr],
			sizeof(double) * matrix->numPoints);
}

void addPoint(Matrix_t * matrix, int x, int y, int z){
	expandMatrix(matrix);
	matrix->points[0][matrix->numPoints - 1] = x;
	matrix->points[1][matrix->numPoints - 1] = y;
	matrix->points[2][matrix->numPoints - 1] = z;
	matrix->points[3][matrix->numPoints - 1] = 0;
}

void addEdge(Matrix_t * matrix, int x1, int y1, int z1, int x2, int y2, int z2){
	addPoint(matrix, x1, y1, z1);
	addPoint(matrix, x2, y2, z2);
}

void drawMatrixLines(Matrix_t * matrix){
	if(matrix->numPoints < 2)
		return;

	int ptPair;
	for(ptPair = 0; ptPair < matrix->numPoints; ptPair += 2)
		drawLine(matrix->points[0][ptPair], matrix->points[1][ptPair],
			matrix->points[0][ptPair + 1], matrix->points[1][ptPair + 1], TEST_COLOR);
}

void printMatrix(Matrix_t * matrix){
	int point;
	for(point = 0; point < matrix->numPoints; point++)
		printf("#%d\t(%f, %f, %f, %f)\n", point + 1,
			matrix->points[0][point], matrix->points[1][point],
			matrix->points[2][point], matrix->points[3][point]);
}
