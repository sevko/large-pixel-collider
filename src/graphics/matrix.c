#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/graphics/geometry.h"
#include "src/graphics/graphics.h"
#include "src/graphics/matrix.h"

/*
 * @brief Get a ::Point_t representation of a ::Matrix_t row.
 *
 * Values are taken from the first four columns of the matrix, because this
 * macro is only useful in the context of ::multiplyMatrix().
 *
 * @param matrix (::Matrix_t *) A matrix.
 * @param row (int) The row to retrieve values from.
*/
#define GET_HORIZONTAL_POINT(matrix, row) \
	(POINT(matrix->points[0][row], matrix->points[1][row],\
		matrix->points[2][row], matrix->points[3][row]))

/*!
 *  The ratio between degrees and radians -- faciliates conversion from the
 *  former to the latter.
 */
#define RAD (M_PI / 180)

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

/*
 * @brief Wrapper for ::surfaceNormal() that acts on a triangle.
 *
 * @param pts A matrix containing a triangular mesh.
 * @param vertex The first vertex of a triangle in @p pts.
 *
 * @return The surface normal of the triangle depicted by the ::Point_t * at
 *      offset @a vertex in @a pts->points, and the two ::Point_t * after it.
*/
static Point_t *triangleNormal(const Matrix_t *pts, int vertex);

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

void drawMatrix(const Matrix_t *matrix){
	Point_t *surfaceNorms[matrix->numPoints / 3];
	int triangle;
	for(triangle = 0; triangle < matrix->numPoints / 3; triangle++){
		surfaceNorms[triangle] = triangleNormal(matrix, triangle * 3);
		NORMALIZE(surfaceNorms[triangle]);
	}

	int vertex;
	for(vertex = 0; vertex < matrix->numPoints; vertex += 3){
		Point_t *p1 = matrix->points[vertex],
			*p2 = matrix->points[vertex + 1],
			*p3 = matrix->points[vertex + 2];

		Point_t *norm = NORMALIZE(triangleNormal(matrix, vertex));
		RGB_t *color1 = flatShade(p1, norm),
			*color2 = flatShade(p2, norm),
			*color3 = flatShade(p3, norm);

		// RGB_t *color1 = flatShade(p1, vertexNorms[vertex]),
			// *color2 = flatShade(p2, vertexNorms[vertex + 1]),
			// *color3 = flatShade(p3, vertexNorms[vertex + 2]);

		if(backfaceCull(p1, p2, p3))
			scanlineRender(
				&(Light_t){
					.color = color1,
					.pos = p1
				},
				&(Light_t){
					.color = color2,
					.pos = p2
				},
				&(Light_t){
					.color = color3,
					.pos = p3
				}
			);
		free(color1);
		free(color2);
		free(color3);
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

		double dot0 = dotProduct(GET_HORIZONTAL_POINT(m1, 0), m2->points[col]);
		double dot1 = dotProduct(GET_HORIZONTAL_POINT(m1, 1), m2->points[col]);
		double dot2 = dotProduct(GET_HORIZONTAL_POINT(m1, 2), m2->points[col]);
		double dot3 = dotProduct(GET_HORIZONTAL_POINT(m1, 3), m2->points[col]);

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
	if((file = fopen(fullFilename, "r")) == NULL)
		FATAL("Cannot open file %s.", fullFilename);

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

double dotProduct(Point_t *p1, Point_t *p2){
	return p1[X] * p2[X] + p1[Y] * p2[Y] + p1[Z] * p2[Z] + p1[W] * p2[W];
}

Point_t *surfaceNormal(Point_t *p1, Point_t *p2, Point_t *p3){
	Point_t *u = SUB_POINT(p2, p1),
		*v = SUB_POINT(p3, p1);

	return createPoint(POINT(
			(u[Y] * v[Z]) - (u[Z] * v[Y]),
			(u[Z] * v[X]) - (u[X] * v[Z]),
			(u[X] * v[Y]) - (u[Y] * v[X]), 0));
}

static int backfaceCull(Point_t *p1, Point_t *p2, Point_t *p3){
	Point_t *norm = surfaceNormal(p1, p2, p3);
	int culled = -(int)norm[Z] < 0;
	free(norm);
	return culled;
}

static Point_t *triangleNormal(const Matrix_t *pts, int vertex){
	return surfaceNormal(pts->points[vertex], pts->points[vertex + 1],
			pts->points[vertex + 2]);
}
