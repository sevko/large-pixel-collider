#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/graphics/geometry.h"
#include "src/graphics/graphics.h"
#include "src/graphics/matrix.h"

/*!
 *  The ratio between degrees and radians -- faciliates conversion from the
 *  former to the latter.
 */
#define RAD (M_PI / 180)

/*!
 *  @brief Return the dot-product of a row of one ::Matrix_t and a column of
 *      another ::Matrix_t.
 *
 *  @param m1 The first @a n*4 matrix (in practice, should always be @a 4*4).
 *  @param row The row of @p m1 to be multiplied.
 *  @param m2 The second @a 4*n ::Matrix_t.
 *  @param col The col of @p m2 to be multiplied.
 *
 *  @return The dot-product of row @p row of m1::points and column @p col of
 *      m2::points.
 */
static double dotProduct(const Matrix_t * const m1, int row,
	const Matrix_t * const m2, int col);

/*
 *  @brief Indicate whether or not a given triangle is visible to the camera.
 *
 *  Given the @f$(x, y, z)@f$ coordinates of the three vertices of a triangle,
 *  calculate the triangle's surface normal to determine whether it's visible
 *  to the camera.
 *
 *  @param p1 The first vertex.
 *  @param p2 The second vertex.
 *  @param p3 The third vertex.
 *
 *  @return 1 if the triangle specified by the three vertices is visible to
 *      the camera; 0 otherwise.
 */
static int backfaceCull(Point_t *p1, Point_t *p2, Point_t *p3);

Matrix_t * createMatrix(void){
	Matrix_t * const matrix = malloc(sizeof(Matrix_t));
	matrix->numPoints = 0;

	matrix->points = NULL;

	return matrix;
}

void expandMatrix(Matrix_t * const matrix){
	matrix->numPoints++;
	matrix->points = realloc(matrix->points,
		matrix->numPoints * sizeof(Point_t *));
}

void freeMatrices(int numMatrices, ...){
	va_list matrices;
	va_start(matrices, numMatrices);

	int arg;
	for(arg = 0; arg < numMatrices; arg++)
		freeMatrix(va_arg(matrices, Matrix_t *));
	va_end(matrices);
}

void freeMatrix(Matrix_t * matrix){
	int pt;
	for(pt = 0; pt < matrix->numPoints; pt++)
		free(matrix->points[pt]);
	free(matrix->points);
	free(matrix);
}

void freeMatrixFromVoid(void * matrix){
	freeMatrix((Matrix_t *)matrix);
}

void drawMatrix(const Matrix_t * const matrix){
	int color = 0;
	int colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF};
	int ptPair;
	for(ptPair = 0; ptPair < matrix->numPoints - 2; ptPair += 3){
		Point_t *p1 = matrix->points[ptPair],
			*p2 = matrix->points[ptPair + 1],
			*p3 = matrix->points[ptPair + 2];
		color++;
		if(backfaceCull(p1, p2, p3))
			scanlineRender(p1, p2, p3, colors[color % 5]);
	}
}

void multiplyScalar(double scalar, Matrix_t * const matrix){
	int row, col;
	for(row = 0; row < 4; row++)
		for(col = 0; col < matrix->numPoints; col++)
			matrix->points[row][col] *= scalar;
}

void multiplyMatrices(int numMatrices, ...){
	va_list matrices;
	va_start(matrices, numMatrices);

	Matrix_t ** multiplicands = malloc((numMatrices - 1) * sizeof(Matrix_t *));

	int arg;
	for(arg = 0; arg < numMatrices - 1; arg++)
		multiplicands[arg] = va_arg(matrices, Matrix_t *);

	// last matrix, into which all preceding matrices will be multiplied
	Matrix_t * points = va_arg(matrices, Matrix_t *);
	va_end(matrices);

	for(arg = 0; arg < numMatrices - 1; arg++)
		multiplyMatrix(multiplicands[arg], points);
	free(multiplicands);
}

void multiplyMatrix(Matrix_t * const m1, Matrix_t * const m2){
	int col;
	for(col = 0; col < m2->numPoints; col++){
		double dot0 = dotProduct(m1, 0, m2, col);
		double dot1 = dotProduct(m1, 1, m2, col);
		double dot2 = dotProduct(m1, 2, m2, col);
		double dot3 = dotProduct(m1, 3, m2, col);

		m2->points[col][X] = dot0;
		m2->points[col][Y] = dot1;
		m2->points[col][Z] = dot2;
		m2->points[col][W] = dot3;
	}
}

Matrix_t * createIdentity(void){
	Matrix_t * identity = createMatrix();
	addPoint(identity, POINT(1, 0, 0, 0));
	addPoint(identity, POINT(0, 1, 0, 0));
	addPoint(identity, POINT(0, 0, 1, 0));
	addPoint(identity, POINT(0, 0, 0, 1));
	return identity;
}

Matrix_t * createTranslation(Point_t *delta){
	Matrix_t * translation = createIdentity();
	translation->points[W][X] = delta[X];
	translation->points[W][Y] = delta[Y];
	translation->points[W][Z] = delta[Z];
	return translation;
}

Matrix_t * createScale(Point_t *delta){
	Matrix_t * scale = createMatrix();
	addPoint(scale, POINT(delta[X], 0, 0, 0));
	addPoint(scale, POINT(0, delta[Y], 0, 0));
	addPoint(scale, POINT(0, 0, delta[Z], 0));
	addPoint(scale, POINT(0, 0, 0, 1));
	return scale;
}

Matrix_t * createRotation(int axis, double angle){
	Matrix_t * rotation = createMatrix();

	double radAngle = angle * RAD;
	double sinA = sin(radAngle), cosA = cos(radAngle);
	if(axis == X_AXIS){
		addPoint(rotation, POINT(1, 0, 0, 0));
		addPoint(rotation, POINT(0, cosA, sinA, 0));
		addPoint(rotation, POINT(0, -sinA, cosA, 0));
	}

	else if(axis == Y_AXIS){
		addPoint(rotation, POINT(cosA, 0, sinA, 0));
		addPoint(rotation, POINT(0, 1, 0, 0));
		addPoint(rotation, POINT(-sinA, 0, cosA, 0));
	}

	else if(axis == Z_AXIS){
		addPoint(rotation, POINT(cosA, sinA, 0, 0));
		addPoint(rotation, POINT(-sinA, cosA, 0, 0));
		addPoint(rotation, POINT(0, 0, 1, 0));
	}

	addPoint(rotation, POINT(0, 0, 0, 1));
	return rotation;
}

void printPointMatrix(const Matrix_t * const matrix){
	puts("Printing points.");

	int point;
	for(point = 0; point < matrix->numPoints; point++)
		printf("#%d\t(%d, %d, %d, %d)\n", point + 1,
			(int)matrix->points[point][X], (int)matrix->points[point][Y],
			(int)matrix->points[point][Z], (int)matrix->points[point][W]);
}

void printMatrix(const Matrix_t * const matrix){
	puts("\nPrinting matrix.");

	int row, col;
	for(col = 0; col < matrix->numPoints; col++){
		printf("|");
		for(row = 0; row < 4; row++)
			printf("\t%d,", (int)matrix->points[row][col]);
		puts("\t|");
	}
}

int equalMatrix(Matrix_t * m1, Matrix_t * m2){
	if(m1->numPoints != m2->numPoints)
		return 0;

	int point;
	for(point = 0; point < m1->numPoints; point++)
		if((int)m1->points[point][X] != (int)m2->points[point][X] ||
			(int)m1->points[point][Y] != (int)m2->points[point][Y] ||
			(int)m1->points[point][Z] != (int)m2->points[point][Z] ||
			(int)m1->points[point][W] != (int)m2->points[point][W])
			return 0;

	return 1;
}

Matrix_t * copyMatrix(const Matrix_t * const matrix){
	Matrix_t * copyMatrix = createMatrix();

	int point;
	for(point = 0; point < matrix->numPoints; point++)
		addPoint(copyMatrix, POINT(matrix->points[point][X],
			matrix->points[point][Y], matrix->points[point][Z],
			matrix->points[point][W]));

	return copyMatrix;
}

Matrix_t * readPointsFromFile(char * filename){
	char * fullFilename = malloc(strlen(filename) + strlen(TEST_FILE_DIR) + 1);
	strcpy(fullFilename, TEST_FILE_DIR);
	strcat(fullFilename, filename);

	FILE * file;
	if((file = fopen(fullFilename, "r")) == NULL){
		FATAL("Cannot open file %s.", fullFilename);
	}

	free(fullFilename);
	Matrix_t * points = createMatrix();

	// read number of points contained in file
	int numPoints;
	if(fscanf(file, "%d:", &numPoints) < 1)
		FATAL("Reading %s. Failed to read number of points.", fullFilename);

	// read points from file; add them to the points matrix
	int point;
	for(point = 0; point < numPoints; point++){
		int x, y, z;
		if(fscanf(file, "%d,%d,%d,", &x, &y, &z) < 3)
			FATAL("Reading %s. Failed to read point #%d.", fullFilename, point);
		addPoint(points, POINT(x, y, z));
	}

	fclose(file);
	return points;
}

void writePointsToFile(Matrix_t * points, char * filename){
	// prepend filename with test-files directory name
	char * fullFilename = malloc(strlen(filename) + strlen(TEST_FILE_DIR) + 1);
	strcpy(fullFilename, TEST_FILE_DIR);
	strcat(fullFilename, filename);

	FILE * file;
	if((file = fopen(fullFilename, "w")) == NULL)
		FATAL("Cannot open file %s.", fullFilename);
	free(fullFilename);

	fprintf(file, "%d:", points->numPoints);

	// write points to file
	int point;
	for(point = 0; point < points->numPoints; point++)
		fprintf(file, "%d,%d,%d,",
			(int)points->points[point][X],
			(int)points->points[point][Y],
			(int)points->points[point][Z]);
	fclose(file);
}

static double dotProduct(const Matrix_t * const m1, int row,
	const Matrix_t * const m2, int col){
	return m1->points[0][row] * m2->points[col][X] +
		m1->points[1][row] * m2->points[col][Y] +
		m1->points[2][row] * m2->points[col][Z] +
		m1->points[3][row] * m2->points[col][W];
}

static int backfaceCull(Point_t *p1, Point_t *p2, Point_t *p3){
	Point_t * u = POINT(p2[X] - p1[X], p2[Y] - p1[Y], p2[Z] - p1[Z]);
	Point_t * v = POINT(p3[X] - p1[X], p3[Y] - p1[Y], p3[Z] - p1[Z]);

	double normalX = (u[Y] * v[Z]) - (u[Z] * v[Y]);
	double normalY = (u[Z] * v[X]) - (u[X] * v[Z]);
	double normalZ = (u[X] * v[Y]) - (u[Y] * v[X]);

	int dotProduct = normalX * 0 + normalY * 0 + normalZ * -1;
	return dotProduct < 0;
}
