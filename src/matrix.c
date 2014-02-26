/*
 *  matrix.c contains functions for creating and manipulating Matrix_t structs.
*/

#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "utils.h"

// alloc memory for Matrix_t, set all internal pointers
Matrix_t * createMatrix(){
	Matrix_t * const matrix = malloc(sizeof(Matrix_t));
	matrix->numPoints = 0;

	matrix->points[0] = matrix->points[1] = matrix->points[2] = matrix->points[3] = NULL;

	return matrix;
}

// free alloc'd Matrix_t and all internal pointers
void freeMatrix(Matrix_t * const matrix){
	int varPtr;
	for(varPtr = 0; varPtr < NUM_POINT_VARS; varPtr++)
		free(matrix->points[varPtr]);
	free(matrix);
}

// expand each of a Matrix_t's points rows by 1
void expandMatrix(Matrix_t * const matrix){
	matrix->numPoints++;
	int varPtr;
	for(varPtr = 0; varPtr < NUM_POINT_VARS; varPtr++)
		matrix->points[varPtr] = realloc(matrix->points[varPtr],
			sizeof(double) * matrix->numPoints);
}

// expand a Matrix_t, and add a point (x, y, z, 1) to its last column
void addPoint(Matrix_t * const matrix, int x, int y, int z){
	expandMatrix(matrix);
	matrix->points[0][matrix->numPoints - 1] = x;
	matrix->points[1][matrix->numPoints - 1] = y;
	matrix->points[2][matrix->numPoints - 1] = z;
	matrix->points[3][matrix->numPoints - 1] = 0;
}

// add two points (x1, y1, z1) and (x2, y2, z2) to a Matrix_t
void addEdge(Matrix_t * const matrix, int x1, int y1, int z1, int x2, int y2, int z2){
	addPoint(matrix, x1, y1, z1);
	addPoint(matrix, x2, y2, z2);
}

// iterate over a Matrix_t's points, and draw lines with adjacent pairs of points
void drawMatrixLines(const Matrix_t * const matrix){
	if(matrix->numPoints < 2)
		return;

	int ptPair;
	for(ptPair = 0; ptPair < matrix->numPoints; ptPair += 2)
		drawLine(matrix->points[0][ptPair], matrix->points[1][ptPair],
			matrix->points[0][ptPair + 1], matrix->points[1][ptPair + 1], TEST_COLOR);
}

// print the values of each of a Matrix_t's points
void printMatrix(const Matrix_t * const matrix){
	int point;
	for(point = 0; point < matrix->numPoints; point++)
		printf("#%d\t(%f, %f, %f, %f)\n", point + 1,
			matrix->points[0][point], matrix->points[1][point],
			matrix->points[2][point], matrix->points[3][point]);
}
