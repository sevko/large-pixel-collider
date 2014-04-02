/*
 *  matrix.c contains functions for creating and manipulating Matrix_t structs.
*/

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "src/matrix.h"
#include "src/graphics.h"

#define INTERPOL(a, b) (a + (b - a) * t1)   // interpolate a point

#define CIRCLE_STEP_SIZE 4 // angle between circles of a sphere/torus
#define CURVE_STEP_NUMBER 1e3 // number of steps used in plotting a curve
#define RAD (M_PI / 180)

struct Matrix {
	double * points[4];
	int numPoints;
};

static void expandMatrix(Matrix_t * const matrix);
static double dotProduct(const Matrix_t * const m1, int row,
	const Matrix_t * const m2, int col);

// alloc memory for Matrix_t, set all internal pointers
Matrix_t * createMatrix(){
	Matrix_t * const matrix = malloc(sizeof(Matrix_t));
	matrix->numPoints = 0;

	matrix->points[0] = matrix->points[1] = NULL;
	matrix->points[2] = matrix->points[3] = NULL;

	return matrix;
}

// call freeMatrix() on vararg Matrix_ts
void freeMatrices(int numArgs, ...){
	va_list matrices;
	va_start(matrices, numArgs);

	int arg;
	for(arg = 0; arg < numArgs; arg++)
		freeMatrix(va_arg(matrices, Matrix_t *));
	va_end(matrices);
}

// free alloc'd Matrix_t and all internal pointers
void freeMatrix(Matrix_t * matrix){
	free(matrix->points[0]);
	free(matrix->points[1]);
	free(matrix->points[2]);
	free(matrix->points[3]);
	free(matrix);
}

// expand a Matrix_t, and add a point (x, y, z, 1) to its last column;
// used to add coordinate points to a matrix, which have a default w value of 1.
void addPoint(Matrix_t * const matrix, double x, double y, double z){
	expandMatrix(matrix);
	matrix->points[0][matrix->numPoints - 1] = x;
	matrix->points[1][matrix->numPoints - 1] = y;
	matrix->points[2][matrix->numPoints - 1] = z;
	matrix->points[3][matrix->numPoints - 1] = 1;
}

// expand a Matrix_t, and add a point (x, y, z, w) to its last column;
// used to add values to a transformation matrix, which has a variable w value.
void addTransformPoint(Matrix_t * const matrix, double x, double y, double z,
	double w){
	expandMatrix(matrix);
	matrix->points[0][matrix->numPoints - 1] = x;
	matrix->points[1][matrix->numPoints - 1] = y;
	matrix->points[2][matrix->numPoints - 1] = z;
	matrix->points[3][matrix->numPoints - 1] = w;
}

// add two points (x1, y1, z1) and (x2, y2, z2) to a Matrix_t
void addEdge(Matrix_t * const matrix, double x1, double y1, double z1,
	double x2, double y2, double z2){
	addPoint(matrix, x1, y1, z1);
	addPoint(matrix, x2, y2, z2);
}

// add a triangle of points (x1, y1, z1), (x2, y2, z2), and (x3, y3, z3) to a
// Matrix_t
void addTriangle(Matrix_t * const matrix,  double x1, double y1, double z1,
	double x2, double y2, double z2, double x3, double y3, double z3){
	addPoint(matrix, x1, y1, z1);
	addPoint(matrix, x2, y2, z2);
	addPoint(matrix, x3, y3, z3);
}

// draw a polygon with origin (oX, oY), radius, and numSides
void addPolygon(Matrix_t * points, int oX, int oY, int radius, int numSides){
	double theta = 2 * M_PI / numSides;
	double tanFactor = tan(theta), radFactor = cos(theta);
	double x = radius, y = 0;

	int segment;
	for(segment = 0; segment < numSides; segment++){
		addPoint(points, x + oX, y + oY, 0);
		addPoint(points, x + oX, y + oY, 0);
		float tempX = x;
		x = (x - y * tanFactor) * radFactor;
		y = (y + tempX * tanFactor) * radFactor;
	}
}

// draw a Bezier curve with control points (x0, y0), (x1, y1), (x2, y2),
// and (x3, y3)
void addBezier(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3){

	int t;
	double t1, abX, bcX, cdX, abbcX, bccdX, abY, bcY, cdY, bccdY, abbcY;
	for(t = 0; t < CURVE_STEP_NUMBER; t++){
		t1 = t / CURVE_STEP_NUMBER;
		abX = INTERPOL(x0, x1);
		bcX = INTERPOL(x1, x2);
		cdX = INTERPOL(x2, x3);
		abbcX = INTERPOL(abX, bcX);
		bccdX = INTERPOL(bcX, cdX);

		abY = INTERPOL(y0, y1);
		bcY = INTERPOL(y1, y2);
		cdY = INTERPOL(y2, y3);
		bccdY = INTERPOL(bcY, cdY);
		abbcY = INTERPOL(abY, bcY);

		addPoint(points, INTERPOL(abbcX, bccdX), INTERPOL(abbcY, bccdY), 0);
	}
}

// draw a Hermite curve with start and endpoints (x0, y0) and (x2, y2), and
// rates of change as calculated using (x1, y1) and (x3, y3).
void addHermite(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3){

	// calculate rates of change
	int r0X = x1 - x0, r0Y = y1 - y0;
	int r1X = x3 - x2, r1Y = y3 - y2;

	// calculate coefficients
	float aX = 2 * x0 - 2 * x2 + r0X + r1X;
	float aY = 2 * y0 - 2 * y2 + r0Y + r1Y;

	float bX = -3 * x0 + 3 * x2 - 2 * r0X - r1X;
	float bY = -3 * y0 + 3 * y2 - 2 * r0Y - r1Y;

	float cX = r0X;
	float cY = r0Y;

	float dX = x0;
	float dY = y0;

	float t1, t2, t3, x, y;

	int t;
	for(t = 0; t < CURVE_STEP_NUMBER; t++){
		t1 = t / CURVE_STEP_NUMBER;
		t3 = t1 * t1 * t1;
		t2 = t1 * t1;

		x = aX * t3 + bX * t2 + cX * t1 + dX;
		y = aY * t3 + bY * t2 + cY * t1 + dY;
		addPoint(points, x, y, 0);
	}
}

// add a rectangular prism to points, with upper-left corner (x, y, z), and the
// given width, height, and depth
void addRectangularPrism(Matrix_t * points, double x, double y, double z,
	double width, double height, double depth){

	// front, top-left
	double aX = x;
	double aY = y;
	double aZ = z;

	// front, bottom-left
	double bX = x;
	double bY = y - height;
	double bZ = z;

	// front, bottom-right
	double cX = x + width;
	double cY = y - height;
	double cZ = z;

	// front, top-right
	double dX = x + width;
	double dY = y;
	double dZ = z;

	// back, top-left
	double eX = aX;
	double eY = aY;
	double eZ = aZ - depth;

	// back, bottom-left
	double fX = bX;
	double fY = bY;
	double fZ = bZ - depth;

	// back, bottom-right
	double gX = cX;
	double gY = cY;
	double gZ = cZ - depth;

	// back, top-right
	double hX = dX;
	double hY = dY;
	double hZ = dZ - depth;

	// front xz face
	addTriangle(points, aX, aY, aZ, bX, bY, bZ, cX, cY, cZ);
	addTriangle(points, aX, aY, aZ, cX, cY, cZ, dX, dY, dZ);

	// back xz face
	addTriangle(points, fX, fY, fZ, eX, eY, eZ, hX, hY, hZ);
	addTriangle(points, fX, fY, fZ, hX, hY, hZ, gX, gY, gZ);

	// top xy face
	addTriangle(points, eX, eY, eZ, aX, aY, aZ, dX, dY, dZ);
	addTriangle(points, eX, eY, eZ, dX, dY, dZ, hX, hY, hZ);

	// bottom xy face
	addTriangle(points, bX, bY, bZ, fX, fY, fZ, gX, gY, gZ);
	addTriangle(points, bX, bY, bZ, gX, gY, gZ, cX, cY, cZ);

	// left yz face
	addTriangle(points, eX, eY, eZ, fX, fY, fZ, bX, bY, bZ);
	addTriangle(points, eX, eY, eZ, bX, bY, bZ, aX, aY, aZ);

	// right yz face
	addTriangle(points, dX, dY, dZ, cX, cY, cZ, gX, gY, gZ);
	addTriangle(points, dX, dY, dZ, gX, gY, gZ, hX, hY, hZ);
}

// add the points of a sphere to points, centered on (oX, oY) with the given
// radius to the argument Matrix_t.
void addSphere(Matrix_t * points, double oX, double oY, double radius){
	Matrix_t * sphere = generateSphere(oX, oY, radius);
	int circlePts = sphere->numPoints / (180 / CIRCLE_STEP_SIZE);

	int point;
	for(point = 0; point < sphere->numPoints - circlePts - 1; point++)
		addTriangle(points,
			sphere->points[0][point],
			sphere->points[1][point],
			sphere->points[2][point],
			sphere->points[0][point + 1],
			sphere->points[1][point + 1],
			sphere->points[2][point + 1],
			sphere->points[0][circlePts + point + 1],
			sphere->points[1][circlePts + point + 1],
			sphere->points[2][circlePts + point + 1]
		);

	for(; point < sphere->numPoints - 1; point++)
		addTriangle(points,
			sphere->points[0][point],
			sphere->points[1][point],
			sphere->points[2][point],
			sphere->points[0][point + 1],
			sphere->points[1][point + 1],
			sphere->points[2][point + 1],
			sphere->points[0][sphere->numPoints - point - 1],
			sphere->points[1][sphere->numPoints - point - 1],
			sphere->points[2][sphere->numPoints - point - 1]
		);

	freeMatrix(sphere);
}

// add the points of a torus to points centered on (oX, oY), with a ring of
// radius rad1, the constituent circles of which are centered along a circle of
// rad2 centered on the torus's origin.
void addTorus(Matrix_t * points, double oX, double oY, double rad1,
	double rad2){
	Matrix_t * torus = generateTorus(oX, oY, rad1, rad2);
	int torusPts = torus->numPoints / (360 / CIRCLE_STEP_SIZE);

	int point;
	for(point = 0; point < torus->numPoints - torusPts - 1; point++)
		addTriangle(points,
			torus->points[0][point],
			torus->points[1][point],
			torus->points[2][point],
			torus->points[0][point + 1],
			torus->points[1][point + 1],
			torus->points[2][point + 1],
			torus->points[0][torusPts + point + 1],
			torus->points[1][torusPts + point + 1],
			torus->points[2][torusPts + point + 1]
		);

	for(; point < torus->numPoints - 1; point++)
		addTriangle(points,
			torus->points[0][point],
			torus->points[1][point],
			torus->points[2][point],
			torus->points[0][point + 1],
			torus->points[1][point + 1],
			torus->points[2][point + 1],
			torus->points[0][(point + 1) % torusPts],
			torus->points[1][(point + 1) % torusPts],
			torus->points[2][(point + 1) % torusPts]
		);

	freeMatrix(torus);
}

// return a Matrix_t with the points of a sphere centered on (oX, oY) with the
// given radius
Matrix_t * generateSphere(double oX, double oY, double radius){
	Matrix_t * sphere = createMatrix(),
		* xRot = createRotation(X_AXIS, CIRCLE_STEP_SIZE);

	int degree;
	for(degree = 0; degree < 180; degree += CIRCLE_STEP_SIZE){
		addCircle(sphere, 0, 0, radius);
		multiplyMatrix(xRot, sphere);
	}

	Matrix_t * translation = createTranslation(oX, oY, 0);
	multiplyMatrix(translation, sphere);

	freeMatrices(2, xRot, translation);
	return sphere;
}

// return a Matrix_t with the points of a torus centered on (oX, oY), with a
// ring of radius rad1, the constituent circles of which are centered along a
// circle of rad2 centered on the torus's origin.
Matrix_t * generateTorus(double oX, double oY, double rad1, double rad2){
	Matrix_t * torus = createMatrix();
	Matrix_t * yRot = createRotation(Y_AXIS, CIRCLE_STEP_SIZE);

	int degree;
	for(degree = 0; degree < 360; degree += 4){
		addCircle(torus, rad2, 0, rad1);
		multiplyMatrix(yRot, torus);
	}

	Matrix_t * translation = createTranslation(oX, oY, 0);
	multiplyMatrix(translation, torus);

	freeMatrices(2, yRot, translation);
	return torus;
}

// iterate over a Matrix_t's points, and draw lines with adjacent point pairs
void drawMatrix(const Matrix_t * const matrix){
	if(matrix->numPoints < 3)
		return;

	int ptPair;
	for(ptPair = 0; ptPair < matrix->numPoints; ptPair += 3){
		drawLine(matrix->points[0][ptPair], matrix->points[1][ptPair],
			matrix->points[0][ptPair + 1], matrix->points[1][ptPair + 1]);
		drawLine(matrix->points[0][ptPair], matrix->points[1][ptPair],
			matrix->points[0][ptPair + 2], matrix->points[1][ptPair + 2]);
		drawLine(matrix->points[0][ptPair + 1], matrix->points[1][ptPair + 1],
			matrix->points[0][ptPair + 2], matrix->points[1][ptPair + 2]);
	}
}

// multiply a Matrix_t by a scalar value
void multiplyScalar(double scalar, Matrix_t * const matrix){
	int row, col;
	for(row = 0; row < 4; row++)
		for(col = 0; col < matrix->numPoints; col++)
			matrix->points[row][col] *= scalar;
}

// multiply the first (numArgs - 1) argument Matrix_ts into the last Matrix_t
void multiplyMatrices(int numArgs, ...){
	va_list matrices;
	va_start(matrices, numArgs);

	Matrix_t ** multiplicands = malloc((numArgs - 1) * sizeof(Matrix_t *));

	int arg;
	for(arg = 0; arg < numArgs - 1; arg++)
		multiplicands[arg] = va_arg(matrices, Matrix_t *);

	// last matrix, into which all preceding matrices will be multiplied
	Matrix_t * points = va_arg(matrices, Matrix_t *);
	va_end(matrices);

	for(arg = 0; arg < numArgs - 1; arg++)
		multiplyMatrix(multiplicands[arg], points);
	free(multiplicands);
}

// multiply Matrix_ts m1 and m2 in place, storing the result in m2
void multiplyMatrix(Matrix_t * const m1, Matrix_t * const m2){
	int col;
	for(col = 0; col < m2->numPoints; col++){
		double dot0 = dotProduct(m1, 0, m2, col);
		double dot1 = dotProduct(m1, 1, m2, col);
		double dot2 = dotProduct(m1, 2, m2, col);
		double dot3 = dotProduct(m1, 3, m2, col);

		m2->points[0][col] = dot0;
		m2->points[1][col] = dot1;
		m2->points[2][col] = dot2;
		m2->points[3][col] = dot3;
	}
}

// return a 4x4 identity matrix
Matrix_t * createIdentity(){
	Matrix_t * identity = createMatrix();
	addTransformPoint(identity, 1, 0, 0, 0);
	addTransformPoint(identity, 0, 1, 0, 0);
	addTransformPoint(identity, 0, 0, 1, 0);
	addTransformPoint(identity, 0, 0, 0, 1);
	return identity;
}

// return a 4x4 matrix to translate a set of points by dx, dy, and dz
Matrix_t * createTranslation(double dx, double dy, double dz){
	Matrix_t * translation = createIdentity();
	translation->points[0][3] = dx;
	translation->points[1][3] = dy;
	translation->points[2][3] = dz;
	return translation;
}

// return a 4x4 matrix to scale a set of points by dx, dy, and dz
Matrix_t * createScale(double dx, double dy, double dz){
	Matrix_t * scale = createMatrix();
	addTransformPoint(scale, dx, 0, 0, 0);
	addTransformPoint(scale, 0, dy, 0, 0);
	addTransformPoint(scale, 0, 0, dz, 0);
	addTransformPoint(scale, 0, 0, 0, 1);
	return scale;
}

// return a 4x4 matrix to rotate a set of points angle degrees across an axis
Matrix_t * createRotation(int axis, double angle){
	Matrix_t * rotation = createMatrix();

	double radAngle = angle * RAD;
	double sinA = sin(radAngle), cosA = cos(radAngle);
	if(axis == X_AXIS){
		addTransformPoint(rotation, 1, 0, 0, 0);
		addTransformPoint(rotation, 0, cosA, sinA, 0);
		addTransformPoint(rotation, 0, -sinA, cosA, 0);
	}

	else if(axis == Y_AXIS){
		addTransformPoint(rotation, cosA, 0, sinA, 0);
		addTransformPoint(rotation, 0, 1, 0, 0);
		addTransformPoint(rotation, -sinA, 0, cosA, 0);
	}

	else if(axis == Z_AXIS){
		addTransformPoint(rotation, cosA, sinA, 0, 0);
		addTransformPoint(rotation, -sinA, cosA, 0, 0);
		addTransformPoint(rotation, 0, 0, 1, 0);
	}

	addTransformPoint(rotation, 0, 0, 0, 1);
	return rotation;
}

// print the formatted values of each of a Matrix_t's points
void printPointMatrix(const Matrix_t * const matrix){
	puts("Printing points.");

	int point;
	for(point = 0; point < matrix->numPoints; point++)
		printf("#%d\t(%d, %d, %d, %d)\n", point + 1,
			(int)matrix->points[0][point], (int)matrix->points[1][point],
			(int)matrix->points[2][point], (int)matrix->points[3][point]);
}

// print the Matrix_t in a matrix format
void printMatrix(const Matrix_t * const matrix){
	puts("\nPrinting matrix.");

	int row, col;
	for(row = 0; row < 4; row++){
		printf("|");
		for(col = 0; col < matrix->numPoints; col++)
			printf("\t%d,", (int)matrix->points[row][col]);
		puts("\t|");
	}
}

// expand each of a Matrix_t's points rows by 1
static void expandMatrix(Matrix_t * const matrix){
	matrix->numPoints++;
	int varPtr;
	for(varPtr = 0; varPtr < 4; varPtr++)
		matrix->points[varPtr] = realloc(matrix->points[varPtr],
			sizeof(double) * matrix->numPoints);
}

// return the dot-product of a row in Matrix_t m1 and a column in Matrix_t m2
static double dotProduct(const Matrix_t * const m1, int row,
	const Matrix_t * const m2, int col){
	return m1->points[row][0] * m2->points[0][col] +
		m1->points[row][1] * m2->points[1][col] +
		m1->points[row][2] * m2->points[2][col] +
		m1->points[row][3] * m2->points[3][col];
}
