#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "globals.h"
#include "matrix.h"
#include "screen.h"
#include "unit_tests.h"

#define TEST(func) \
	do {\
		int success = func;\
		printf("Testing %-30s %s.\n", #func ":", success?"Success":"Failure");\
		if(!success)\
			FATAL("Unit test %s failed.", #func);\
	} while(0)

#define SAVE(matrix, filename, ...) \
	configureScreen();\
	__VA_ARGS__\
	drawMatrix(matrix);\
	renderScreen();\
	usleep(4e6);\
	writePointsToFile(matrix, filename);\
	quitScreen()

#define ASSERT_EQUAL(matrix, filename) \
	do {\
		Matrix_t * filePoints = readPointsFromFile(filename);\
		int result = equalMatrix(matrix, filePoints);\
		freeMatrices(2, matrix, filePoints);\
		return result;\
	} while(0)

#define TEST_FILE_DIR "test/"

static int testMultiplyMatrices();
static int testAddPoint();
static int testAddPolygons();
static int testPointsFileIO();
static Matrix_t * readPointsFromFile(char * filename);
static void writePointsToFile(Matrix_t * points, char * filename);

static int testMultiplyScalar(){
	Matrix_t * points = createMatrix();
	addPoint(points, 11, 22, 33);
	addPoint(points, 22, 33, 44);
	addPoint(points, 33, 44, 55);
	addPoint(points, 44, 55, 66);

	Matrix_t * product = createMatrix();
	addTransformPoint(product, 33, 66, 99, 3);
	addTransformPoint(product, 66, 99, 132, 3);
	addTransformPoint(product, 99, 132, 165, 3);
	addTransformPoint(product, 132, 165, 198, 3);

	multiplyScalar(3, points);
	int result = equalMatrix(points, product);
	freeMatrices(2, points, product);
	return result;
}

// test multiplyMatrix() and multiplyMatrices()
static int testMultiplyMatrices(){
	// matrix1, to be multiplied into matrix2
	Matrix_t * m1 = createMatrix();
	addPoint(m1, 11, 22, 33);
	addPoint(m1, 22, 33, 44);
	addPoint(m1, 33, 44, 55);
	addPoint(m1, 44, 55, 66);

	// matrix2, to be multiplied into and store multiplication result
	Matrix_t * m2 = createMatrix();
	addPoint(m2, 44, 55, 66);
	addPoint(m2, 55, 66, 77);
	addPoint(m2, 66, 77, 88);
	addPoint(m2, 77, 88, 99);

	// resultant matrix
	Matrix_t * product1 = createMatrix();
	addTransformPoint(product1, 3916, 5742, 7568, 166);
	addTransformPoint(product1, 4642, 6831, 9020, 199);
	addTransformPoint(product1, 5368, 7920, 10472, 232);
	addTransformPoint(product1, 6094, 9009, 11924, 265);

	multiplyMatrix(m1, m2);
	int result1 = equalMatrix(m2, product1);
	freeMatrices(3, m1, m2, product1);

	// matrix1, to be multiplied into matrix3
	Matrix_t * m3 = createMatrix();
	addPoint(m3, 11, 22, 33);
	addPoint(m3, 22, 33, 44);
	addPoint(m3, 33, 44, 55);
	addPoint(m3, 44, 55, 66);

	// matrix2, to be multiplied into matrix3
	Matrix_t * m4 = createMatrix();
	addPoint(m4, 44, 55, 66);
	addPoint(m4, 55, 66, 77);
	addPoint(m4, 66, 77, 88);
	addPoint(m4, 77, 88, 99);

	// matrix3, to be multiplied into and store multiplication result
	Matrix_t * m5 = createMatrix();
	addPoint(m5, 11, 22, 33);
	addPoint(m5, 22, 33, 44);
	addPoint(m5, 33, 44, 55);
	addPoint(m5, 44, 55, 66);

	Matrix_t * product2 = createMatrix();
	addTransformPoint(product2, 429748, 512160, 594572, 7492);
	addTransformPoint(product2, 619960, 738672, 857384, 10792);
	addTransformPoint(product2, 810172, 965184, 1120196, 14092);
	addTransformPoint(product2, 1000384, 1191696, 1383008, 17392);

	multiplyMatrices(3, m3, m4, m5);
	int result2 = equalMatrix(m5, product2);
	freeMatrices(4, m3, m4, m5, product2);

	return result1 && result2;
}

static int testEqualMatrix(){
	Matrix_t * m1 = createMatrix();
	addPoint(m1, 11, 22, 33);
	addPoint(m1, 22, 33, 44);
	addPoint(m1, 33, 44, 55);
	addPoint(m1, 44, 55, 66);

	Matrix_t * m2 = createMatrix();
	addPoint(m2, 11, 22, 33);
	addPoint(m2, 22, 33, 44);
	addPoint(m2, 33, 44, 55);
	addPoint(m2, 44, 55, 66);

	int result = equalMatrix(m1, m2);
	freeMatrices(2, m1, m2);
	return result;
}

// test addPoint()
static int testAddPoint(){
	Matrix_t * points = createMatrix();

	addPoint(points, 0, 0, 0);
	addPoint(points, 100, 100, 100);
	addPoint(points, 100, 200, 200);

	addPoint(points, 0, 0, 0);
	addPoint(points, -100, -100, -100);
	addPoint(points, -100, -200, -200);

	ASSERT_EQUAL(points, "testAddPoint.csv");
}

static int testAddEdge(){
	Matrix_t * points = createMatrix();
	addEdge(points, 0, 0, 0, 100, 0, 100);
	addEdge(points, 100, 0, 100, 100, 100, 100);
	addEdge(points, 100, 100, 100, 0, 0, 0);
	ASSERT_EQUAL(points, "testAddEdge");
}

// test addPolygon(), addCircle()
static int testAddPolygons(){
	Matrix_t * points = createMatrix();
	addCircle(points, 0, 0, 200);
	addPolygon(points, 0, 0, 100, 8);

	ASSERT_EQUAL(points, "testAddPolygons.csv");
}

// test readPointsFromFile(), writePointsToFile()
static int testPointsFileIO(){
	Matrix_t * points = createMatrix();
	addPoint(points, 11, 22, 33);
	addPoint(points, 22, 33, 44);
	addPoint(points, 33, 44, 55);
	addPoint(points, 44, 55, 66);

	writePointsToFile(points, "testPointsFileIO.csv");
	ASSERT_EQUAL(points, "testPointsFileIO.csv");
}

static int testAddBezier(){
	Matrix_t * points = createMatrix();
	addBezier(points, 200, 250, 150, 50, 300, 250, 300, 250);
	ASSERT_EQUAL(points, "testAddBezier.csv");
}

static int testAddHermite(){
	Matrix_t * points = createMatrix();
	addHermite(points, 150, 150, 150, 50, 350, 150, 350, 300);
	ASSERT_EQUAL(points, "testAddHermite.csv");
}

static int testAddRectangularPrism(){
	Matrix_t * points = createMatrix();
	addRectangularPrism(points, 0, 0, 0, 100, 200, 300);
	ASSERT_EQUAL(points, "testAddRectangularPrism.csv");
}

static int testAddSphere(){
	Matrix_t * points = createMatrix();
	addSphere(points, 0, 0, 100);
	addSphere(points, 100, 0, 50);
	addSphere(points, 100, 100, 50);
	ASSERT_EQUAL(points, "testAddSphere.csv");
}

static int testAddTorus(){
	Matrix_t * points = createMatrix();
	addTorus(points, 0, 0, 50, 200);
	addTorus(points, 0, 0, 20, 100);
	ASSERT_EQUAL(points, "testAddTorus.csv");
}

static int testCreateTranslation(){
	Matrix_t * points = createMatrix(),
		* translation = createTranslation(-50, -100, 0);
	addTriangle(points, 0, 0, 0, 100, 0, 100, 100, 100, 200);
	multiplyMatrix(translation, points);
	freeMatrix(translation);
	ASSERT_EQUAL(points, "testCreateTranslation.csv");
}

static int testCreateScale(){
	Matrix_t * points = createMatrix(),
		* scale = createScale(3, 3, 3);
	addRectangularPrism(points, 0, 0, 0, 100, 100, 100);
	multiplyMatrix(scale, points);
	freeMatrix(scale);
	ASSERT_EQUAL(points, "testCreateScale.csv");
}

static int testCreateRotation(){
	Matrix_t * points = createMatrix(),
		* rotX = createRotation(X_AXIS, 270),
		* rotY = createRotation(Y_AXIS, 30),
		* rotZ = createRotation(Z_AXIS, 45);
	addRectangularPrism(points, 0, 0, 0, 20, 100, 300);
	multiplyMatrices(4, rotX, rotY, rotZ, points);
	freeMatrices(3, rotX, rotY, rotZ);
	ASSERT_EQUAL(points, "testCreateRotation.csv");
}

static int testCreateIdentity(){
	Matrix_t * identity = createIdentity();

	Matrix_t * matrix = createMatrix();
	addTransformPoint(matrix, 1, 0, 0, 0);
	addTransformPoint(matrix, 0, 1, 0, 0);
	addTransformPoint(matrix, 0, 0, 1, 0);
	addTransformPoint(matrix, 0, 0, 0, 1);

	int result = equalMatrix(identity, matrix);
	freeMatrices(2, identity, matrix);
	return result;
}

// run all unit tests
void testAll(){
	puts("Begin unit tests.\n");

	TEST(testMultiplyScalar());
	TEST(testMultiplyMatrices());
	TEST(testEqualMatrix());
	TEST(testPointsFileIO());
	TEST(testAddPoint());
	TEST(testAddPolygons());
	TEST(testAddBezier());
	TEST(testAddHermite());
	TEST(testAddRectangularPrism());
	TEST(testAddSphere());
	TEST(testAddTorus());
	TEST(testCreateTranslation());
	TEST(testCreateScale());
	TEST(testCreateRotation());
	TEST(testAddEdge());
	TEST(testCreateIdentity());

	puts("\nUnit tests completed successfully.");
}

// temporary function, used while writing unit_tests.c
void test(){
	testAddEdge();
}

// read points from a CSV file named filename, and return them in a Matrix_t
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
		addPoint(points, x, y, z);
	}

	fclose(file);
	return points;
}

// write points contained in a Matrix_t to a CSV file named filename
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
			(int)points->points[X][point],
			(int)points->points[Y][point],
			(int)points->points[Z][point]);
	fclose(file);
}
