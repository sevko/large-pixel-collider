#include <stdlib.h>
#include <stdio.h>

#include "src/graphics/geometry.h"
#include "src/graphics/matrix.h"

/*!
 *  @brief Return the linear interpolation of numeric values @p a and @p b.
 *
 *  @param a The first numeric value.
 *  @param b The second numeric value.
 */
#define INTERPOL(a, b) (a + (b - a) * t1)

// The angle between the subsequent, rotated circles that compose a sphere.
#define CIRCLE_STEP_SIZE 8

//! The number of steps taken in plotting Bezier and Hermite curves.
#define CURVE_STEP_NUMBER 1e3

/*!
 *  @brief Return a pointer to a ::Matrix_t containing the points of a sphere.
 *
 *  @param origin The origin of the sphere.
 *  @param radius The radius of the sphere.
 *
 *  @return A pointer to a ::Matrix_t containing the points of the sphere.
 */
static Matrix_t * generateSphere(Point_t *origin, double radius);

/*!
 *  @brief Return a pointer to a ::Matrix_t containing the points of a torus.
 *
 *  @param origin The origin of the torus.
 *  @param radius The radius of the torus.
 *
 *  @return A pointer to a ::Matrix_t containing the points of the torus.
 */
static Matrix_t * generateTorus(Point_t *origin, double rad1, double rad2);

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

void addPolygonFull(Matrix_t * points, Point_t *origin, int radius, int
	numSides, double theta){
	double tanFactor = tan(theta), radFactor = cos(theta);
	double x = radius, y = 0;

	int segment;
	for(segment = 0; segment < numSides; segment++){
		addPoint(points, POINT(x + origin[X], y + origin[Y], 0));
		float tempX = x;
		x = (x - y * tanFactor) * radFactor;
		y = (y + tempX * tanFactor) * radFactor;
	}
	addPoint(points, POINT(x + origin[X], y + origin[Y], 0));
}

void addBezier(Matrix_t * points, Point_t *p1, Point_t *p2, Point_t *p3,
	Point_t *p4){

	int t;
	double t1, abX, bcX, cdX, abbcX, bccdX, abY, bcY, cdY, bccdY, abbcY;
	for(t = 0; t < CURVE_STEP_NUMBER; t++){
		t1 = t / CURVE_STEP_NUMBER;
		abX = INTERPOL(p1[X], p2[X]);
		bcX = INTERPOL(p2[X], p3[X]);
		cdX = INTERPOL(p3[X], p4[X]);
		abbcX = INTERPOL(abX, bcX);
		bccdX = INTERPOL(bcX, cdX);

		abY = INTERPOL(p1[Y], p2[Y]);
		bcY = INTERPOL(p2[Y], p3[Y]);
		cdY = INTERPOL(p3[Y], p4[Y]);
		bccdY = INTERPOL(bcY, cdY);
		abbcY = INTERPOL(abY, bcY);

		addPoint(points, POINT(INTERPOL(abbcX, bccdX), INTERPOL(abbcY, bccdY), 0));
	}
}

void addHermite(Matrix_t * points, Point_t *p1, Point_t *p2, Point_t *p3,
	Point_t *p4){

	// calculate rates of change
	int r0X = p2[X] - p1[X], r0Y = p2[Y] - p1[Y];
	int r1X = p4[X] - p3[X], r1Y = p4[Y] - p3[Y];

	// calculate coefficients
	float aX = 2 * p1[X] - 2 * p3[X] + r0X + r1X;
	float aY = 2 * p1[Y] - 2 * p3[Y] + r0Y + r1Y;

	float bX = -3 * p1[X] + 3 * p3[X] - 2 * r0X - r1X;
	float bY = -3 * p1[Y] + 3 * p3[Y] - 2 * r0Y - r1Y;

	float cX = r0X;
	float cY = r0Y;

	float dX = p1[X];
	float dY = p1[Y];

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

void addSphere(Matrix_t * points, Point_t *origin, double radius){
	Matrix_t * sphere = generateSphere(origin, radius);
	int circlePts = sphere->numPoints / (360 / CIRCLE_STEP_SIZE);

	int circle, point;
	for(circle = 0; circle < 360 / CIRCLE_STEP_SIZE - 1; circle++){
		int circleStart = circle * circlePts;
		for(point = 0; point < circlePts - 2; point++){
			addTriangle(points,
				(sphere->points[circleStart + point + 1]),
				(sphere->points[circleStart + point]),
				(sphere->points[circleStart + circlePts + point + 1])
			);
			addTriangle(points,
				(sphere->points[circleStart + point]),
				(sphere->points[circleStart + circlePts + point]),
				(sphere->points[circleStart + circlePts + point + 1])
			);
		}
	}

	for(point = sphere->numPoints - circlePts; point < sphere->numPoints - 2;
		point++){
		addTriangle(points,
			(sphere->points[point + 1]),
			(sphere->points[point]),
			(sphere->points[(point + 1) % circlePts])
		);
		addTriangle(points,
			(sphere->points[(point + 1) % circlePts]),
			(sphere->points[point]),
			(sphere->points[point % circlePts])
		);
	}

	freeMatrix(sphere);
}

void addTorus(Matrix_t * points, Point_t *origin, double rad1, double rad2){
	Matrix_t * torus = generateTorus(origin, rad1, rad2);
	int circlePts = torus->numPoints / (360 / CIRCLE_STEP_SIZE);

	int circle, point;
	for(circle = 0; circle < 360 / CIRCLE_STEP_SIZE - 1; circle++){
		int circleStart = circle * circlePts;
		for(point = 0; point < circlePts - 1; point++){
			addTriangle(points,
				torus->points[circleStart + point + 1],
				torus->points[circleStart + point],
				torus->points[circleStart + circlePts + point + 1]
			);
			addTriangle(points,
				torus->points[circleStart + point],
				torus->points[circleStart + circlePts + point],
				torus->points[circleStart + circlePts + point + 1]
			);
		}
	}

	int circleStart = torus->numPoints - circlePts;
	for(point = 0; point < circlePts - 1; point++){
		addTriangle(points,
			torus->points[circleStart + point + 1],
			torus->points[circleStart + point],
			torus->points[point + 1]
		);
		addTriangle(points,
			torus->points[point + 1],
			torus->points[circleStart + point],
			torus->points[point]
		);
	}

	freeMatrix(torus);
}

static Matrix_t * generateSphere(Point_t *origin, double radius){
	Matrix_t * sphere = createMatrix(),
		* xRot = createRotation(X_AXIS, CIRCLE_STEP_SIZE);

	int degree;
	for(degree = 0; degree < 360; degree += CIRCLE_STEP_SIZE){
		addHalfCircle(sphere, POINT(0, 0), radius);
		multiplyMatrix(xRot, sphere);
	}

	Matrix_t * translation = createTranslation(origin);
	multiplyMatrix(translation, sphere);

	freeMatrices(2, xRot, translation);
	return sphere;
}

static Matrix_t * generateTorus(Point_t *origin, double rad1, double rad2){
	Matrix_t * torus = createMatrix();
	Matrix_t * yRot = createRotation(Y_AXIS, CIRCLE_STEP_SIZE);

	int degree;
	for(degree = 0; degree < 360; degree += CIRCLE_STEP_SIZE){
		addCircle(torus, POINT(rad2, 0), rad1);
		multiplyMatrix(yRot, torus);
	}

	Matrix_t * translation = createTranslation(origin);
	multiplyMatrix(translation, torus);

	freeMatrices(2, yRot, translation);
	return torus;
}
