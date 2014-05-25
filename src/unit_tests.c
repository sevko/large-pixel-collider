/*!
 *  @file
 *  @brief Unit-test functions used to perform regression testing.
 */

#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/unit_tests.h"
#include "src/graphics/matrix.h"
#include "src/graphics/screen.h"

/*!
 *  @brief Execute a unit-test function, and print an appropriate message.
 *
 *  Print a formatted message indicating whether the unit-test @p func executed
 *  successfully (a return value of 1), or failed (return value of 0). If
 *  possible, display the success/failure message with an appropriate color,
 *  using the TERM_COLOR_* macros.
 *
 *  @param func A unit-test function to run: must have a return value of 1 on
 *      success, and 0 on failure.
 */
#define TEST(func) \
	do {\
		int testResult = func;\
		if(hasColors)\
			printf("Testing %-30s %s%s\n", #func ":",\
				testResult? TERM_COLOR_SUCCESS "Success.":\
					TERM_COLOR_FAILURE "Failure.\tx", TERM_COLOR_NORMAL);\
		else\
			printf("Testing %-30s %s\n", #func ":",\
				testResult?"Success.":"Failure.\tx");\
	} while(0)

/*!
 *  @brief A helper macro for writing new unit tests.
 *
 *  Execute any code in ..., and display the results in the SDL screen; then,
 *  write the ::Matrix_t @p matrix to a file located at path @p filename.  The
 *  macro simplifies the process of authoring new unit-tests, which often
 *  require visual verification before a given points ::Matrix_t is written to
 *  a file and checked against during future unit-tests.
 *
 *  The following code sample will create a ::Matrix_t named @a points, add a
 *  sphere of radius 100, centered on the @f$(0, 0, 0)@f$, display the results
 *  in an SDL screen, then write @a points to a file named "testMyFunction.csv".
 *
 *  @code{.c}
 *      static int testMyFunction(void){
 *          SAVE(points, "testMyFunction.csv",
 *              Matrix_t * points = createMatrix();
 *              addSphere(points, 0, 0, 100);
 *          )
 *      }
 *  @endcode
 *
 *  @param matrix
 */
#define SAVE(matrix, filename, ...) \
	do {\
		configureScreen();\
		__VA_ARGS__\
		drawMatrix(matrix);\
		renderScreen();\
		usleep(4e6);\
		writePointsToFile(matrix, filename);\
		quitScreen();\
	} while(0)

/*!
 *  @brief Indicate whether a ::Matrix_t matches that stored in a points CSV
 *      file.
 *
 *  @param matrix The ::Matrix_t whose points will be compared.
 *  @param filename The path of the points CSV file that a ::Matrix_t will be
 *      generated from, and @p matrix compared to.
 *
 *  @return 1 if the points in @p matrix match those contained in the ::Matrix_t
 *      generated from the CSV file.
 */
#define ASSERT_EQUAL(matrix, filename) \
	do {\
		Matrix_t * filePoints = readPointsFromFile(filename);\
		int result = equalMatrix(matrix, filePoints);\
		freeMatrices(2, matrix, filePoints);\
		return result;\
	} while(0)

//! The terminal escape code to set a foreground color for a success message.
#define TERM_COLOR_SUCCESS "\033[38;5;34m"

//! The terminal escape code to set a foreground color for a failure message.
#define TERM_COLOR_FAILURE "\033[38;5;160m"

//! The terminal escape code to set a foreground color for an important message.
#define TERM_COLOR_HEADER "\033[38;5;33m"

//! The terminal escape code to reset the terminal's foreground color.
#define TERM_COLOR_NORMAL "\033[0;00m"

/*!
 *  @brief Test matrix.h addPoint().
 */
static int testAddPoint(void);

/*!
 *  @brief Test matrix.h addEdge().
 */
static int testAddEdge(void);

/*!
 *  @brief Test matrix.h addCircle() and addPolygon().
 */
static int testAddPolygons(void);

/*!
 *  @brief Test matrix.h addBezier().
 */
static int testAddBezier(void);

/*!
 *  @brief Test matrix.h addHermite().
 */
static int testAddHermite(void);

/*!
 *  @brief Test matrix.h addRectangularPrism().
 */
static int testAddRectangularPrism(void);

/*!
 *  @brief Test matrix.h addSphere().
 */
static int testAddSphere(void);

/*!
 *  @brief Test matrix.h addTorus().
 */
static int testAddTorus(void);

/*!
 *  @brief Test matrix.h multiplyScalar().
 */
static int testMultiplyScalar(void);

/*!
 *  @brief Test matrix.h multiplyMatrix() and multiplyMatrices().
 */
static int testMultiplyMatrices(void);

/*!
 *  @brief Test matrix.h writePointsToFile() and readPointsFromFile().
 */
static int testPointsFileIO(void);

/*!
 *  @brief Test matrix.h createTranslation().
 */
static int testCreateTranslation(void);

/*!
 *  @brief Test matrix.h createScale().
 */
static int testCreateScale(void);

/*!
 *  @brief Test matrix.h createRotation().
 */
static int testCreateRotation(void);

/*!
 *  @brief Test matrix.h createIdentity().
 */
static int testCreateIdentity(void);

/*!
 *  @brief Test matrix.h equalMatrix().
 */
static int testEqualMatrix(void);

static int testAddPoint(void){
	Matrix_t * points = createMatrix();

	addPoint(points, POINT(0, 0, 0));
	addPoint(points, POINT(100, 100, 100));
	addPoint(points, POINT(100, 200, 200));

	addPoint(points, POINT(0, 0, 0));
	addPoint(points, POINT(-100, -100, -100));
	addPoint(points, POINT(-100, -200, -200));

	ASSERT_EQUAL(points, "testAddPoint.csv");
}

static int testAddEdge(void){
	Matrix_t * points = createMatrix();
	addEdge(points, POINT(0, 0, 0), POINT(100, 0, 100));
	addEdge(points, POINT(100, 0, 100), POINT(100, 100, 100));
	addEdge(points, POINT(100, 100, 100), POINT(0, 0, 0));
	ASSERT_EQUAL(points, "testAddEdge.csv");
}

static int testAddPolygons(void){
	Matrix_t * points = createMatrix();
	addCircle(points, 0, 0, 200);
	addPolygon(points, 0, 0, 100, 8);
	ASSERT_EQUAL(points, "testAddPolygons.csv");
}

static int testAddBezier(void){
	Matrix_t * points = createMatrix();
	addBezier(points, 200, 250, 150, 50, 300, 250, 300, 250);
	ASSERT_EQUAL(points, "testAddBezier.csv");
}

static int testAddHermite(void){
	Matrix_t * points = createMatrix();
	addHermite(points, 150, 150, 150, 50, 350, 150, 350, 300);
	ASSERT_EQUAL(points, "testAddHermite.csv");
}

static int testAddRectangularPrism(void){
	Matrix_t * points = createMatrix();
	addRectangularPrism(points, POINT(0, 0, 0), (double[]){100, 200, 300});
	ASSERT_EQUAL(points, "testAddRectangularPrism.csv");
}

static int testAddSphere(void){
	Matrix_t * points = createMatrix();
	addSphere(points, POINT(0, 0), 100);
	addSphere(points, POINT(100, 0), 50);
	addSphere(points, POINT(100, 100), 50);
	ASSERT_EQUAL(points, "testAddSphere.csv");
}

static int testAddTorus(void){
	Matrix_t * points = createMatrix();
	addTorus(points, POINT(0, 0), 50, 200);
	addTorus(points, POINT(0, 0), 20, 100);
	ASSERT_EQUAL(points, "testAddTorus.csv");
}

static int testMultiplyScalar(void){
	Matrix_t * points = createMatrix();
	addPoint(points, POINT(11, 22, 33));
	addPoint(points, POINT(22, 33, 44));
	addPoint(points, POINT(33, 44, 55));
	addPoint(points, POINT(44, 55, 66));

	Matrix_t * product = createMatrix();
	addPoint(product, POINT(33, 66, 99, 3));
	addPoint(product, POINT(66, 99, 132, 3));
	addPoint(product, POINT(99, 132, 165, 3));
	addPoint(product, POINT(132, 165, 198, 3));

	multiplyScalar(3, points);
	int result = equalMatrix(points, product);
	freeMatrices(2, points, product);
	return result;
}

static int testMultiplyMatrices(void){
	// matrix1, to be multiplied into matrix2
	Matrix_t * m1 = createMatrix();
	addPoint(m1, POINT(11, 22, 33));
	addPoint(m1, POINT(22, 33, 44));
	addPoint(m1, POINT(33, 44, 55));
	addPoint(m1, POINT(44, 55, 66));

	// matrix2, to be multiplied into and store multiplication result
	Matrix_t * m2 = createMatrix();
	addPoint(m2, POINT(44, 55, 66));
	addPoint(m2, POINT(55, 66, 77));
	addPoint(m2, POINT(66, 77, 88));
	addPoint(m2, POINT(77, 88, 99));

	// resultant matrix
	Matrix_t * product1 = createMatrix();
	addPoint(product1, POINT(3916, 5742, 7568, 166));
	addPoint(product1, POINT(4642, 6831, 9020, 199));
	addPoint(product1, POINT(5368, 7920, 10472, 232));
	addPoint(product1, POINT(6094, 9009, 11924, 265));

	multiplyMatrix(m1, m2);
	int result1 = equalMatrix(m2, product1);
	freeMatrices(3, m1, m2, product1);

	// matrix1, to be multiplied into matrix3
	Matrix_t * m3 = createMatrix();
	addPoint(m3, POINT(11, 22, 33));
	addPoint(m3, POINT(22, 33, 44));
	addPoint(m3, POINT(33, 44, 55));
	addPoint(m3, POINT(44, 55, 66));

	// matrix2, to be multiplied into matrix3
	Matrix_t * m4 = createMatrix();
	addPoint(m4, POINT(44, 55, 66));
	addPoint(m4, POINT(55, 66, 77));
	addPoint(m4, POINT(66, 77, 88));
	addPoint(m4, POINT(77, 88, 99));

	// matrix3, to be multiplied into and store multiplication result
	Matrix_t * m5 = createMatrix();
	addPoint(m5, POINT(11, 22, 33));
	addPoint(m5, POINT(22, 33, 44));
	addPoint(m5, POINT(33, 44, 55));
	addPoint(m5, POINT(44, 55, 66));

	Matrix_t * product2 = createMatrix();
	addPoint(product2, POINT(429748, 512160, 594572, 7492));
	addPoint(product2, POINT(619960, 738672, 857384, 10792));
	addPoint(product2, POINT(810172, 965184, 1120196, 14092));
	addPoint(product2, POINT(1000384, 1191696, 1383008, 17392));

	multiplyMatrices(3, m3, m4, m5);
	int result2 = equalMatrix(m5, product2);
	freeMatrices(4, m3, m4, m5, product2);

	return result1 && result2;
}

static int testPointsFileIO(void){
	Matrix_t * points = createMatrix();
	addPoint(points, POINT(11, 22, 33));
	addPoint(points, POINT(22, 33, 44));
	addPoint(points, POINT(33, 44, 55));
	addPoint(points, POINT(44, 55, 66));

	writePointsToFile(points, "testPointsFileIO.csv");
	ASSERT_EQUAL(points, "testPointsFileIO.csv");
}

static int testCreateTranslation(void){
	Matrix_t * points = createMatrix(),
		* translation = createTranslation(POINT(-50, -100, 0));
	addTriangle(points, POINT(0, 0, 0), POINT(100, 0, 100),
		POINT(100, 100, 200));
	multiplyMatrix(translation, points);
	freeMatrix(translation);
	ASSERT_EQUAL(points, "testCreateTranslation.csv");
}

static int testCreateScale(void){
	Matrix_t * points = createMatrix(),
		* scale = createScale(POINT(3, 3, 3));
	addRectangularPrism(points, POINT(0, 0, 0), POINT(100, 100, 100));
	multiplyMatrix(scale, points);
	freeMatrix(scale);
	ASSERT_EQUAL(points, "testCreateScale.csv");
}

static int testCreateRotation(void){
	Matrix_t * points = createMatrix(),
		* rotX = createRotation(X_AXIS, 270),
		* rotY = createRotation(Y_AXIS, 30),
		* rotZ = createRotation(Z_AXIS, 45);
	addRectangularPrism(points, POINT(0, 0, 0), (double[]){20, 100, 300});
	multiplyMatrices(4, rotX, rotY, rotZ, points);
	freeMatrices(3, rotX, rotY, rotZ);
	ASSERT_EQUAL(points, "testCreateRotation.csv");
}

static int testCreateIdentity(void){
	Matrix_t * identity = createIdentity();

	Matrix_t * matrix = createMatrix();
	addPoint(matrix, POINT(1, 0, 0, 0));
	addPoint(matrix, POINT(0, 1, 0, 0));
	addPoint(matrix, POINT(0, 0, 1, 0));
	addPoint(matrix, POINT(0, 0, 0, 1));

	int result = equalMatrix(identity, matrix);
	freeMatrices(2, identity, matrix);
	return result;
}

static int testEqualMatrix(void){
	Matrix_t * m1 = createMatrix();
	addPoint(m1, POINT(11, 22, 33));
	addPoint(m1, POINT(22, 33, 44));
	addPoint(m1, POINT(33, 44, 55));
	addPoint(m1, POINT(44, 55, 66));

	Matrix_t * m2 = createMatrix();
	addPoint(m2, POINT(11, 22, 33));
	addPoint(m2, POINT(22, 33, 44));
	addPoint(m2, POINT(33, 44, 55));
	addPoint(m2, POINT(44, 55, 66));

	int result = equalMatrix(m1, m2);
	freeMatrices(2, m1, m2);
	return result;
}

void unitTests(void){
	// initscr();
	int hasColors = 1;
	// int hasColors = has_colors();
	// endwin();

	if(hasColors)
		printf("%sBegin unit tests.%s\n\n", TERM_COLOR_HEADER,
			TERM_COLOR_NORMAL);
	else
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

	if(hasColors)
		printf("\n%sUnit tests completed successfully.%s\n", TERM_COLOR_HEADER,
			TERM_COLOR_NORMAL);
	else
		puts("\nUnit tests completed successfully.");
}
