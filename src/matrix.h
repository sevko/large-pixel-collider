#pragma once

// rasterize a polygon with a large number of sides
#define addCircle(points, oX, oY, radius) \
	addPolygon(points, oX, oY, radius, (radius / 2 ))

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define X 0
#define Y 1
#define Z 2
#define W 3

typedef struct {
	double * points[4];
	int numPoints;
} Matrix_t;

Matrix_t * createMatrix();
void freeMatrices(int numArgs, ...);
void freeMatrix(Matrix_t * matrix);
void addPoint(Matrix_t * const matrix, double x, double y, double z);
void addTransformPoint(Matrix_t * const matrix, double x, double y, double z,
	double w);
void addEdge(Matrix_t * const matrix, double x1, double y1, double z1,
	double x2, double y2, double z2);
void addTriangle(Matrix_t * const matrix,  double x1, double y1, double z1,
	double x2, double y2, double z2, double x3, double y3, double z3);
void addPolygon(Matrix_t * points, int oX, int oY, int radius, int numSides);
void addBezier(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3);
void addHermite(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3);
void addRectangularPrism(Matrix_t * points, double x, double y, double z,
	double width, double height, double depth);
void addSphere(Matrix_t * points, double oX, double oY, double radius);
void addTorus(Matrix_t * points, double oX, double oY, double rad1,
	double rad2);
Matrix_t * generateSphere(double oX, double oY, double radius);
Matrix_t * generateTorus(double oX, double oY, double rad1, double rad2);
void drawMatrix(const Matrix_t * const matrix);
void drawMatrixLines(const Matrix_t * const matrix);
void multiplyScalar(double scalar, Matrix_t * const matrix);
void multiplyMatrices(int numArgs, ...);
void multiplyMatrix(Matrix_t * const m1, Matrix_t * const m2);
Matrix_t * createIdentity();
Matrix_t * createTranslation(double dx, double dy, double dz);
Matrix_t * createScale(double dx, double dy, double dz);
Matrix_t * createRotation(int axis, double angle);
void printPointMatrix(const Matrix_t * const matrix);
void printMatrix(const Matrix_t * const matrix);
int equalMatrix(Matrix_t * m1, Matrix_t * m2);
