/*!
 * @file
 * @brief Constructs for matrix manipulation.
 *
 * A collection of functions, macros, and structs used to create and manipulate
 * points matrices, which are represented with ::Point_t and ::Matrix_t.
 */

#pragma once

#include <math.h>

#define CLEAR(matrix_pointer) \
	do {\
		freeMatrix(matrix_pointer);\
		matrix_pointer = createMatrix();\
	} while(0)

/*
 * @brief Create a point with a specific w-coordinate.
 *
 * @params See ::createPoint().
*/
#define POINT4(x, y, z, w) ((double []){x, y, z, w})

/*
 * @brief Create a point with a w-coordinate of 1.
 *
 * @params See ::createPoint().
*/
#define POINT3(x, y, z) ((double []){x, y, z, 1})

/*
 * @brief Create a point with no z or w coordinate.
 *
 * @params See ::createPoint().
*/
#define POINT2(x, y) ((double []){x, y, 0, 1})

/*
 *  @brief Helper macro for the overloaded ::POINT().
 *
 *  Replaces the ::POINT() macro with ::POINT2(), ::POINT3(), or ::POINT4(),
 *  depending on the number of received arguments.
 *
 *  @param arg1 Placeholder for an argument.
 *  @param arg2 Placeholder for an argument.
 *  @param arg3 Placeholder for an argument.
 *  @param arg4 Placeholder for an argument.
 *  @param func_name The macro to replace ::POINT() with.
 *  @param ... Any subsequent arguments.
 */
#define CREATE_POINT_VA_MACRO(arg1, arg2, arg3, arg4, func_name, ...) \
	func_name

/*
 *  @brief Overloaded double array constructor.
 *
 *  Uses ::CREATE_POINT_VA_MACRO() to select an appropriate function-like macro
 *  wrapper for the number of arguments (2, 3, or 4).
 *
 *  @code
 *      // The following macro-function calls are both valid.
 *      POINT(10, 20);
 *      POINT(10, 20, 30);
 *      POINT(10, 20, 30, 40);
 *  @code
 */
#define POINT(...) \
	CREATE_POINT_VA_MACRO(__VA_ARGS__, POINT4, POINT3, POINT2)(__VA_ARGS__)

/*
 * @brief Normalize a vector.
 *
 * @param vector (::Point_t *) A vector.
*/
#define NORMALIZE(vector) \
	do {\
		double length = sqrt(vector[X] * vector[X] + vector[Y] * vector[Y] +\
			vector[Z] * vector[Z]);\
		vector[X] /= length;\
		vector[Y] /= length;\
		vector[Z] /= length;\
	} while(0)

/*
 * @brief Create the ::Point_t * difference of two ::Point_t.
 *
 * @param p1 (::Point_t *) A point.
 * @param p2 (::Point_t *) A point.
*/
#define SUB_POINT(p1, p2) \
	POINT(p1[X] - p2[X], p1[Y] - p2[Y], p1[Z] - p2[Z], p1[Z] - p2[Z])

/*
 * @brief Create a copy of a ::Point_t.
 *
 * @param pt (::Point_t *) A ::Point_t.
*/
#define COPY_POINT(pt) (POINT(pt[X], pt[Y], pt[Z], pt[W]))

#define X_AXIS 0 // The x-axis.
#define Y_AXIS 1 // The y-axis.
#define Z_AXIS 2 // The z-axis.
#define X 0 // The index of the x-coordinate of a point in ::Point_t *.
#define Y 1 // The index of the y-coordinate of a point in ::Point_t *.
#define Z 2 // The index of the x-coordinate of a point in ::Point_t *.
#define W 3 // The index of the w-coordinate of a point in ::Point_t *.

typedef double Point_t; // Used to represent multi-dimensional points.

// A struct to contain point coordinates.
typedef struct Matrix {
	Point_t **points; // An pointer to an array of ::Point_t.
	int numPoints; // The number of points in this ::Matrix_t.
} Matrix_t;

/*
 * @brief Return an allocated ::Point_t.
 *
 * @param Point_t The ::Point_t to create an allocated copy of.
 *
 * @return A pointer to the new ::Point_t.
*/
Point_t *createPoint(Point_t *pt);

/*!
 *  @brief Add a point to a ::Matrix_t.
 *
 *  @param matrix A pointer to the ::Matrix_t to add the @p point to.
 *  @param point The point.
 */
void addPoint(Matrix_t *const matrix, Point_t *point);

/*!
 *  @brief Allocate memory for a ::Matrix_t.
 *
 *  @return A pointer to a newly allocated ::Matrix_t.
 */
Matrix_t *createMatrix(void);

/*!
 *  @brief Expand a ::Matrix_t::points to accomodate an additional point.
 *
 *  Realloc a given ::Matrix_t::points, and increment ::Matrix_t::numPoints.
 *
 *  @param matrix The ::Matrix_t whose ::Matrix_t::points matrix will be
 *      expanded to an additional column
 */
void expandMatrix(Matrix_t * const matrix);

/*!
 *  @brief ::freeMatrix() for varargs ::Matrix_t.
 *
 *  @param numMatrices The number of ::Matrix_t arguments.
 *  @param ... Varargs ::Matrix_t to be called ::freeMatrix() on.
 */
void freeMatrices(int numMatrices, ...);

/*!
 *  @brief Deallocate a ::Matrix_t.
 *
 *  Deallocate all of the memory belonging to the ::Matrix_t pointed to by @p
 *  matrix.
 *
 *  @param matrix Pointer to a ::Matrix_t.
 */
void freeMatrix(Matrix_t *matrix);

/*
 * @brief Free a ::Matrix_t from a void pointer.
 *
 * Used to facilitate freeing matrices from a ::Stack_t using the module's
 * inbuilt ::freeStack() macro, which requires a @p freeElement(void *)
 * function; note the generalized void pointer
 *
 * @param matrix A void pointer to a ::Matrix_t to be deallocated.
*/
void freeMatrixFromVoid(void *matrix);

/*!
 *  @brief Render a ::Matrix_t by drawing triangles.
 *
 *  Draw @p matrix by rendering triangles for every triplet of points. The first
 *  three points of ::Matrix_t::points are the three vertices of the first
 *  triangle, the second three points are the vertices of the second, etc.
 *
 *  @param matrix The ::Matrix_t to be rendered.
 */
void drawMatrix(const Matrix_t *const matrix);

/*!
 *  @brief Multiply a ::Matrix_t by a scalar value.
 *
 *  @param scalar The scalar value to multiply @p matrix by.
 *  @param matrix The ::Matrix_t to be multiplied into (in place).
 */
void multiplyScalar(double scalar, Matrix_t *const matrix);

/*!
 *  @brief Multiply varargs matrices into the last one.
 *
 *  Call ::multiplyMatrix() for each of the first @f$numMatrices - 1@f$
 *  ::Matrix_t and the last ::Matrix_t; in short, multiply every argument
 *  ::Matrix_t before the last, into the last.
 *
 *  @param numArgs The number of argument ::Matrix_t.
 *  @param ... More than one ::Matrix_t.
 */
void multiplyMatrices(int numMatrices, ...);

/*!
 *  @brief Multiply ::Matrix_t @P m1 into ::Matrix_t @p m2.
 *
 *  @param m1 ::Matrix_t to be multiplied into @p m2.
 *  @param m2 ::Matrix_t to contain the product of the original @p m1 and @p m2.
 */
void multiplyMatrix(Matrix_t *const m1, Matrix_t *const m2);

/*!
 *  @brief Return a @a 4x4 identity matrix.
 *
 *  @return An identity ::Matrix_t with four points.
 */
Matrix_t *createIdentity(void);

/*!
 *  @brief Return a matrix used for translations.
 *
 *  Return a ::Matrix_t that, when multiplied into a points ::Matrix_t,
 *  translates every point by the @p delta point-vector.
 *
 *  @param delta A point representation of the translation displacements.
 *
 *  @return A @a 4x4 translaton ::Matrix_t.
 */
Matrix_t *createTranslation(Point_t *delta);

/*!
 *  @brief Return a matrix used for scaling.
 *
 *  Return a ::Matrix_t that, when multiplied into a points ::Matrix_t,
 *  scales every point by the (@p dx, @p dy, @p dz) vector.
 *
 *  @param delta A point representation of the scale factors.
 *
 *  @return A @a 4x4 scale ::Matrix_t.
 */
Matrix_t *createScale(Point_t *delta);

/*!
 *  @brief Return a matrix used for rotation.
 *
 *  Return a ::Matrix_t that, when multiplied into a points ::Matrix_t,
 *  rotates every point @p angle degrees through the @P axis.
 *
 *  @param axis The axis to rotate through.
 *  @param angle The number of degrees to rotate the points by.
 *
 *  @return A @a 4x4 rotation ::Matrix_t.
 */
Matrix_t *createRotation(int axis, double angle);

/*
 *  @brief Print ::Matrix_t::points in a point (x, y, z, w) format.
 *
 *  @param matrix The ::Matrix_t whose points will be printed.
 */
void printPointMatrix(const Matrix_t *const matrix);

/*
 *  @brief Print ::Matrix_t::points in a point matrix format.
 *
 *  @param matrix The ::Matrix_t whose points will be printed.
 */
void printMatrix(const Matrix_t *const matrix);

/*!
 *  @brief Return a boolean indicating whether two ::Matrix_t are equal.
 *
 *  @param m1 First ::Matrix_t to compare.
 *  @param m2 Second ::Matrix_t to compare.
 *
 *  @return If ::Matrix_t @p m1 and @p m2 contain the same number of points,
 *      with equal coordinate values, return 1; else, return 0.
 */
int equalMatrix(Matrix_t *m1, Matrix_t *m2);

/*
 * @brief Return a deep-copy of a ::Matrix_t.
 *
 * @param matrix The matrix whose points will be copied.
 *
 * @return A pointer to the new, copied ::Matrix_t with identical contents to
 *      @p matrix.
 */
Matrix_t *copyMatrix(const Matrix_t *const matrix);

/*!
 *  @brief Scan the point coordinates contained in a formatted CSV file into a
 *      ::Matrix_t.
 *
 *  Read in a CSV file containing point coordinates (as written by
 *  writePointsToFile()), and store its points in a new Matrix_t.
 *
 *  @param filename The name of the CSV file to read; this file must be stored
 *      inside the TEST_FILE_DIR.
 *
 *  @return A pointer to a ::Matrix_t containing points with the values stored
 *      in the argument CSV file.
 */
Matrix_t *readPointsFromFile(char *filename);

/*!
 *  @brief Save a ::Matrix_t::points coordinates in a CSV file.
 *
 *  @param points The ::Matrix_t whose points will be written to the file.
 *  @param filename The name of the CSV file to be written to; this file will
 *      be stored in the TEST_FILE_DIR directory.
 */
void writePointsToFile(Matrix_t *points, char *filename);

/*!
 *  @brief Calculate a dot-product.
 *
 *  @param p1 A point.
 *  @param p2 A point.
 *
 *  @return The dot-product of @p p1 and @p p2.
 */
double dotProduct(Point_t *p1, Point_t *p2);

/*
 * @brief Return the normal vector to a triangle.
 *
 * @param p1 The first vertex of the triangle.
 * @param p2 The second vertex of the triangle.
 * @param p3 The third vertex of the triangle.
 *
 * @return The surface normal of the triangle in ::Point_t form.
*/
Point_t *surfaceNormal(Point_t *p1, Point_t *p2, Point_t *p3);
