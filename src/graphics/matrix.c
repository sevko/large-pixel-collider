#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/graphics/matrix.h"
#include "src/graphics/graphics.h"

/*!
 *  @brief Return the linear interpolation of numeric values @p a and @p b.
 *
 *  @param a The first numeric value.
 *  @param b The second numeric value.
 */
#define INTERPOL(a, b) (a + (b - a) * t1)

//! The angle between the subsequent, rotated circles that compose a sphere.
#define CIRCLE_STEP_SIZE 8 // angle between circles of a sphere/torus

//! The number of steps taken in plotting Bezier and Hermite curves.
#define CURVE_STEP_NUMBER 1e3

/*!
 *  The ratio between degrees and radians -- faciliates conversion from the
 *  former to the latter.
 */
#define RAD (M_PI / 180)

//! The directory to contain all ::Matrix_t points CSV files.
#define TEST_FILE_DIR "test/"

/*!
 *  @brief Return a pointer to a ::Matrix_t containing the points of a sphere.
 *
 *  @param oX The x-coordinate of the sphere's origin.
 *  @param oY The y-coordinate of the sphere's origin.
 *  @param radius The radius of the sphere.
 *
 *  @return A pointer to a ::Matrix_t containing the points of the sphere.
 */
static Matrix_t * generateSphere(double oX, double oY, double radius);

/*!
 *  @brief Return a pointer to a ::Matrix_t containing the points of a torus.
 *
 *  @param oX The x-coordinate of the torus's centroid.
 *  @param oY The y-coordinate of the torus's centroid.
 *  @param radius The radius of the torus.
 *
 *  @return A pointer to a ::Matrix_t containing the points of the torus.
 */
static Matrix_t * generateTorus(double oX, double oY, double rad1, double rad2);

/*!
 *  @brief Expand a ::Matrix_t::points to accomodate an additional point.
 *
 *  Realloc a given ::Matrix_t::points, and increment ::Matrix_t::numPoints.
 *
 *  @param matrix The ::Matrix_t whose ::Matrix_t::points matrix will be
 *      expanded to an additional column
 */
static void expandMatrix(Matrix_t * const matrix);

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
 *  @param x1 The x-coordinate of the triangle's first vertex.
 *  @param y1 The y-coordinate of the triangle's first vertex.
 *  @param z1 The z-coordinate of the triangle's first vertex.
 *  @param x2 The x-coordinate of the triangle's second vertex.
 *  @param y2 The y-coordinate of the triangle's second vertex.
 *  @param z2 The z-coordinate of the triangle's second vertex.
 *  @param x3 The x-coordinate of the triangle's third vertex.
 *  @param y3 The y-coordinate of the triangle's third vertex.
 *  @param z3 The z-coordinate of the triangle's third vertex.
 *
 *  @return 1 if the triangle specified by the three vertices is visible to
 *      the camera; 0 otherwise.
 */
static int backfaceCull(double x1, double y1, double z1, double x2, double y2,
	double z2, double x3, double y3, double z3);

Matrix_t * createMatrix(void){
	Matrix_t * const matrix = malloc(sizeof(Matrix_t));
	matrix->numPoints = 0;

	matrix->points = NULL;

	return matrix;
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

Point_t * createPoint(Point_t * pt){
	Point_t * point = malloc(4 * sizeof(double));
	point[X] = pt[X];
	point[Y] = pt[Y];
	point[Z] = pt[Z];
	point[W] = pt[W];
	return point;
}

void addPoint(Matrix_t * const matrix, Point_t * point){
	expandMatrix(matrix);
	matrix->points[matrix->numPoints - 1] = createPoint(point);
}

void addEdge(Matrix_t * const matrix, Point_t * p1, Point_t * p2){
	addPoint(matrix, POINT(p1[X] + 0.1, p1[Y], p1[Z]));
	addPoint(matrix, p1);
	addPoint(matrix, p2);
}

void addTriangle(Matrix_t * const matrix, Point_t * p1, Point_t * p2,
	Point_t * p3){
	addPoint(matrix, p1);
	addPoint(matrix, p2);
	addPoint(matrix, p3);
}

void addPolygonFull(Matrix_t * points, int oX, int oY, int radius, int
	numSides, double theta){
	double tanFactor = tan(theta), radFactor = cos(theta);
	double x = radius, y = 0;

	int segment;
	for(segment = 0; segment < numSides; segment++){
		addPoint(points, POINT(x + oX, y + oY, 0));
		float tempX = x;
		x = (x - y * tanFactor) * radFactor;
		y = (y + tempX * tanFactor) * radFactor;
	}
	addPoint(points, POINT(x + oX, y + oY, 0));
}

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

		addPoint(points, POINT(INTERPOL(abbcX, bccdX), INTERPOL(abbcY, bccdY), 0));
	}
}

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
		addPoint(points, POINT(x, y, 0));
	}
}

void addRectangularPrism(Matrix_t *pts, Point_t *p1, Point_t *p2){
	Point_t *a = p1; // front, top-left
	Point_t *b = POINT(p1[X], p1[Y] - p2[Y], p1[Z]); // front, bottom-left
	Point_t *c = POINT(p1[X] + p2[X], p1[Y] - p2[Y], p1[Z]); // front, bottom-rt
	Point_t *d = POINT(p1[X] + p2[X], p1[Y], p1[Z]); // front, top-right
	Point_t *e = POINT(a[X], a[Y], a[Z] - p2[Z]); // back, top-left
	Point_t *f = POINT(b[X], b[Y], b[Z] - p2[Z]); // back, bottom-left
	Point_t *g = POINT(c[X], c[Y], c[Z] - p2[Z]); // back, bottom-right
	Point_t *h = POINT(d[X], d[Y], d[Z] - p2[Z]); // back, top-right

	// front xz face
	addTriangle(pts, a, b, c);
	addTriangle(pts, a, c, d);

	// back xz face
	addTriangle(pts, f, e, h);
	addTriangle(pts, f, h, g);

	// top xy face
	addTriangle(pts, e, a, d);
	addTriangle(pts, e, d, h);

	// bottom xy face
	addTriangle(pts, b, f, g);
	addTriangle(pts, b, g, c);

	// left yz face
	addTriangle(pts, e, f, b);
	addTriangle(pts, e, b, a);

	// right yz face
	addTriangle(pts, d, c, g);
	addTriangle(pts, d, g, h);
}

void addSphere(Matrix_t * points, double oX, double oY, double radius){
	Matrix_t * sphere = generateSphere(oX, oY, radius);
	int circlePts = sphere->numPoints / (360 / CIRCLE_STEP_SIZE);

	int circle, point;
	for(circle = 0; circle < 360 / CIRCLE_STEP_SIZE - 1; circle++){
		int circleStart = circle * circlePts;
		for(point = 0; point < circlePts - 1; point++){
			addTriangle(points,
				COPY_POINT(sphere->points[circleStart + point + 1]),
				COPY_POINT(sphere->points[circleStart + point]),
				COPY_POINT(sphere->points[circleStart + circlePts + point + 1])
			);
			addTriangle(points,
				COPY_POINT(sphere->points[circleStart + point]),
				COPY_POINT(sphere->points[circleStart + circlePts + point]),
				COPY_POINT(sphere->points[circleStart + circlePts + point + 1])
			);
		}
	}

	for(point = sphere->numPoints - circlePts; point < sphere->numPoints - 1;
		point++){
		addTriangle(points,
			COPY_POINT(sphere->points[point + 1]),
			COPY_POINT(sphere->points[point]),
			COPY_POINT(sphere->points[(point + 1) % circlePts])
		);
		addTriangle(points,
			COPY_POINT(sphere->points[(point + 1) % circlePts]),
			COPY_POINT(sphere->points[point]),
			COPY_POINT(sphere->points[point % circlePts])
		);
	}

	freeMatrix(sphere);
}

void addTorus(Matrix_t * points, double oX, double oY, double rad1,
	double rad2){
	Matrix_t * torus = generateTorus(oX, oY, rad1, rad2);
	int circlePts = torus->numPoints / (360 / CIRCLE_STEP_SIZE);

	int circle, point;
	for(circle = 0; circle < 360 / CIRCLE_STEP_SIZE - 1; circle++){
		int circleStart = circle * circlePts;
		for(point = 0; point < circlePts - 1; point++)
			addTriangle(points,
				COPY_POINT(torus->points[circleStart + point + 1]),
				COPY_POINT(torus->points[circleStart + point]),
				COPY_POINT(torus->points[circleStart + circlePts + point + 1])
			);
		addTriangle(points,
			COPY_POINT(torus->points[circleStart]),
			COPY_POINT(torus->points[circleStart + point]),
			COPY_POINT(torus->points[circleStart + point + 1])
		);
	}

	int circleStart = torus->numPoints - circlePts;
	for(point = 0; point < circlePts - 1; point++)
		addTriangle(points,
			COPY_POINT(torus->points[circleStart + point + 1]),
			COPY_POINT(torus->points[circleStart + point]),
			COPY_POINT(torus->points[point + 1])
		);
	addTriangle(points,
		COPY_POINT(torus->points[circleStart]),
		COPY_POINT(torus->points[circleStart + point]),
		COPY_POINT(torus->points[0])
	);

	freeMatrix(torus);
}

Matrix_t * generateSphere(double oX, double oY, double radius){
	Matrix_t * sphere = createMatrix(),
		* xRot = createRotation(X_AXIS, CIRCLE_STEP_SIZE);

	int degree;
	for(degree = 0; degree < 360; degree += CIRCLE_STEP_SIZE){
		addHalfCircle(sphere, 0, 0, radius);
		multiplyMatrix(xRot, sphere);
	}

	Matrix_t * translation = createTranslation(oX, oY, 0);
	multiplyMatrix(translation, sphere);

	freeMatrices(2, xRot, translation);
	return sphere;
}

Matrix_t * generateTorus(double oX, double oY, double rad1, double rad2){
	Matrix_t * torus = createMatrix();
	Matrix_t * yRot = createRotation(Y_AXIS, CIRCLE_STEP_SIZE);

	int degree;
	for(degree = 0; degree < 360; degree += CIRCLE_STEP_SIZE){
		addCircle(torus, rad2, 0, rad1);
		multiplyMatrix(yRot, torus);
	}

	Matrix_t * translation = createTranslation(oX, oY, 0);
	multiplyMatrix(translation, torus);

	freeMatrices(2, yRot, translation);
	return torus;
}

void drawMatrix(const Matrix_t * const matrix){
	int color = 0;
	int colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF};
	int ptPair;
	for(ptPair = 0; ptPair < matrix->numPoints - 2; ptPair += 3){
		double x1 = matrix->points[ptPair][X],
			y1 = matrix->points[ptPair][Y],
			z1 = matrix->points[ptPair][Z],
			x2 = matrix->points[ptPair + 1][X],
			y2 = matrix->points[ptPair + 1][Y],
			z2 = matrix->points[ptPair + 1][Z],
			x3 = matrix->points[ptPair + 2][X],
			y3 = matrix->points[ptPair + 2][Y],
			z3 = matrix->points[ptPair + 2][Z];
		color++;
		if(backfaceCull(x1, y1, z1, x2, y2, z2, x3, y3, z3))
			scanlineRender(x1, y1, x2, y2, x3, y3, colors[color % 5]);
	}
}

void drawMatrixLines(const Matrix_t * const matrix){
	int ptPair;
	for(ptPair = 0; ptPair < matrix->numPoints - 1; ptPair += 2)
		drawLine(matrix->points[ptPair][X], matrix->points[ptPair][Y],
			matrix->points[ptPair + 1][X], matrix->points[ptPair + 1][Y]);
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

Matrix_t * createTranslation(double dx, double dy, double dz){
	Matrix_t * translation = createIdentity();
	translation->points[W][X] = dx;
	translation->points[W][Y] = dy;
	translation->points[W][Z] = dz;
	return translation;
}

Matrix_t * createScale(double dx, double dy, double dz){
	Matrix_t * scale = createMatrix();
	addPoint(scale, POINT(dx, 0, 0, 0));
	addPoint(scale, POINT(0, dy, 0, 0));
	addPoint(scale, POINT(0, 0, dz, 0));
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

static void expandMatrix(Matrix_t * const matrix){
	matrix->numPoints++;
	matrix->points = realloc(matrix->points,
		matrix->numPoints * sizeof(Point_t *));
	// int varPtr;
	// for(varPtr = 0; varPtr < 4; varPtr++)
		// matrix->points[varPtr] = realloc(matrix->points[varPtr],
			// sizeof(double) * matrix->numPoints);
}

static double dotProduct(const Matrix_t * const m1, int row,
	const Matrix_t * const m2, int col){
	return m1->points[0][row] * m2->points[col][X] +
		m1->points[1][row] * m2->points[col][Y] +
		m1->points[2][row] * m2->points[col][Z] +
		m1->points[3][row] * m2->points[col][W];
}

static int backfaceCull(double x1, double y1, double z1, double x2, double y2,
	double z2, double x3, double y3, double z3){
	double uX = x2 - x1;
	double uY = y2 - y1;
	double uZ = z2 - z1;

	double vX = x3 - x1;
	double vY = y3 - y1;
	double vZ = z3 - z1;

	double normalX = (uY * vZ) - (uZ * vY);
	double normalY = (uZ * vX) - (uX * vZ);
	double normalZ = (uX * vY) - (uY * vX);

	int dotProduct = normalX * 0 + normalY * 0 + normalZ * -1;
	return dotProduct < 0;
}
