#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "matrix.h"
#include "unit_tests.h"

#define TEST(func) printf("Testing " #func ": %s.\n", func?"Success":"Failure")

#define TEST_FILE_DIR "test/"

static int testMultiplyMatrix();
static int testMultiplyMatrices();
static int testPointPrimitives();
static int testPointsFileIO();

// test multiplyMatrix(), by multiplying two arbitrary matrices
static int testMultiplyMatrix(){
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
	Matrix_t * result = createMatrix();
	addTransformPoint(result, 3916, 5742, 7568, 166);
	addTransformPoint(result, 4642, 6831, 9020, 199);
	addTransformPoint(result, 5368, 7920, 10472, 232);
	addTransformPoint(result, 6094, 9009, 11924, 265);

	multiplyMatrix(m1, m2);
	int res = equalMatrix(m2, result);
	freeMatrices(3, m1, m2, result);
	return res;
}

// test multiplyMatrices(), by multiplying three arbitrary matrices
static int testMultiplyMatrices(){
	// matrix1, to be multiplied into matrix3
	Matrix_t * m1 = createMatrix();
	addPoint(m1, 11, 22, 33);
	addPoint(m1, 22, 33, 44);
	addPoint(m1, 33, 44, 55);
	addPoint(m1, 44, 55, 66);

	// matrix2, to be multiplied into matrix3
	Matrix_t * m2 = createMatrix();
	addPoint(m2, 44, 55, 66);
	addPoint(m2, 55, 66, 77);
	addPoint(m2, 66, 77, 88);
	addPoint(m2, 77, 88, 99);

	// matrix3, to be multiplied into and store multiplication result
	Matrix_t * m3 = createMatrix();
	addPoint(m3, 11, 22, 33);
	addPoint(m3, 22, 33, 44);
	addPoint(m3, 33, 44, 55);
	addPoint(m3, 44, 55, 66);

	Matrix_t * product = createMatrix();
	addTransformPoint(product, 429748, 512160, 594572, 7492);
	addTransformPoint(product, 619960, 738672, 857384, 10792);
	addTransformPoint(product, 810172, 965184, 1120196, 14092);
	addTransformPoint(product, 1000384, 1191696, 1383008, 17392);

	multiplyMatrices(3, m1, m2, m3);
	int result = equalMatrix(m3, product);
	freeMatrices(4, m1, m2, m3, product);
	return result;
}

// test addPoint(), addEdge()
static int testPointPrimitives(){
	Matrix_t * points = createMatrix();

	addPoint(points, 10, 20, 30);
	addPoint(points, 20, 30, 40);
	addPoint(points, 30, 40, 50);

	addEdge(points, 0, 10, 20, 30, 40, 50);
	addEdge(points, 30, 40, 50, 60, 70, 80);
	addEdge(points, 0, 10, 20, 60, 70, 80);

	Matrix_t * filePoints = readPointsFromFile("testPointPrimitives.csv");
	int result = equalMatrix(points, filePoints);
	freeMatrices(2, points, filePoints);
	return result;
}

// test readPointsFromFile(), writePointsToFile()
static int testPointsFileIO(){
	Matrix_t * points = createMatrix();
	addPoint(points, 11, 22, 33);
	addPoint(points, 22, 33, 44);
	addPoint(points, 33, 44, 55);
	addPoint(points, 44, 55, 66);

	writePointsToFile(points, "testPointsFileIO.csv");
	Matrix_t * filePoints = readPointsFromFile("testPointsFileIO.csv");

	int result = equalMatrix(points, filePoints);
	freeMatrices(2, points, filePoints);
	return result;
}

// run all unit tests
void testAll(){
	TEST(testMultiplyMatrix());
	TEST(testMultiplyMatrices());
	TEST(testPointsFileIO());
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
