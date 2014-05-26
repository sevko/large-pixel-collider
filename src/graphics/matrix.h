/*!
 *  @file
 *  @brief Functions that concern shape creation and point manipulations.
 *
 *  matrix.h contains functions that constitute the backbone of the graphics
 *  engine's processing and rendering. The module introduces a ::Matrix_t
 *  struct, which contains a matrix of points that can be transformed and house
 *  various shapes (eg tori, spheres), and the methods necessary to modfy it.
 */

#pragma once

#include <math.h>

#define CLEAR(matrix_pointer) \
	do {\
		freeMatrix(matrix_pointer);\
		matrix_pointer = createMatrix();\
	} while(0)

/*
 *  @brief Add a circle to a ::Matrix_t.
 *
 *  Add an approximated circle of points to ::Matrix_t @p points, by calling
 *  ::addPolygonFull() with a large number of sides: (@f$\frac{radius}{2}@f$).
 *
 *  @param points (::Matrix_t) The matrix to add the circle's points to.
 *  @param origin (Point_t *) The circle's origin.
 *  @param radius (double) The radius of the circle.
 */
#define addCircle(points, origin, radius) \
	addPolygon(points, origin, radius, (radius / 2));

/*
 *  @brief Add a half circle to a ::Matrix_t.
 *
 *  Add an approximated half-circle of points to ::Matrix_t @p points, by
 *  calling addPolygonFull() with a large number of sides,
 *  (@f$\frac{radius}{2}@f$), and half the angle used by addCircle().
 *
 *  @param points (::Matrix_t) The matrix to add the half-circle's points to.
 *  @param origin (Point_t *) The torus's origin.
 *  @param radius (double) The radius of the half-circle.
 */
#define addHalfCircle(points, origin, radius) {\
		int numSides = (radius / 2);\
		numSides += numSides % 2;\
		addPolygonFull(points, origin, radius, numSides, (2 * M_PI / radius));\
	} while(0)

/*
 *  @brief A wrapper for addPolygonFull(), with an implicit angle calculation.
 *
 *  @param points (::Matrix_t) The matrix to add the polygon's points to.
 *  @param origin (Point_t *) The polygon's centroid.
 *  @param numSides (int) The number of sides the polygon will contain.
 */
#define addPolygon(points, origin, radius, numSides) \
	addPolygonFull(points, origin, radius, numSides, 2 * M_PI / numSides)

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

#define COPY_POINT(pt) (POINT(pt[X], pt[Y], pt[Z], pt[W]))

// Macro for the x-axis; used as an argument to createRotation().
#define X_AXIS 0

// Macro for the y-axis; used as an argument to createRotation().
#define Y_AXIS 1

// Macro for the z-axis; used as an argument to createRotation().
#define Z_AXIS 2

// The index of the x-coordinate of a point in ::Point_t *.
#define X 0

// The index of the y-coordinate of a point in ::Point_t *.
#define Y 1

// The index of the x-coordinate of a point in ::Point_t *.
#define Z 2

// The index of the w-coordinate of a point in ::Point_t *.
#define W 3

typedef double Point_t;

// A struct to contain point coordinates.
typedef struct Matrix {
	Point_t **points; // An pointer to an array of ::Point_t.
	int numPoints; // The number of points in this ::Matrix_t.
} Matrix_t;

/*!
 *  @brief Allocate memory for a ::Matrix_t.
 *
 *  @return A pointer to a newly allocated ::Matrix_t.
 */
Matrix_t *createMatrix(void);

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
 *  @brief Add a line's endpoints to a ::Matrix_t.
 *
 *  @param matrix A pointer to the ::Matrix_t to add the endpoints to.
 *  @param p1 The first endpoint.
 *  @param p2 The second endpoint.
 *
 *  @deprecated Deprecated since the introduction of polygonal rendering, which
 *      requires that points be added to a ::Matrix_t with addTriangle().
 */
void addEdge(Matrix_t *const matrix, Point_t *p1, Point_t *p2);

/*!
 *  @brief Add a triangle's three vertices to a ::Matrix_t.
 *
 *  @param matrix A pointer to the ::Matrix_t to add the vertices to.
 *  @param p1 The first vertex.
 *  @param p2 The second vertex.
 *  @param p3 The third vertex.
 */
void addTriangle(Matrix_t *const matrix, Point_t *p1, Point_t *p2, Point_t *p3);

/*!
 *  @brief Add the vertices of a polygon to a ::Matrix_t.
 *
 *  @param points A pointer to the ::Matrix_t to add the vertices to.
 *  @param origin The polygon's centroid.
 *  @param radius The distance between the centroid and any of the polygon's
 *      vertices.
 *  @param numSides The number of edges the polygon has.
 *  @param theta The angle of the arc, centered on the origin, and extending
 *      between subsequent vertices of the polygon.
 */
void addPolygonFull(Matrix_t *points, Point_t *origin, int radius, int
	numSides, double theta);

/*!
 *  @brief Add the points of a Bezier curve to a ::Matrix_t.
 *
 *  @param points A pointer to the ::Matrix_t to add the curve's points to.
 *  @param p1 The first control point.
 *  @param p2 The second control point.
 *  @param p3 The third control point.
 *  @param p4 The fourth control point.
 */
void addBezier(Matrix_t *points, Point_t *p1, Point_t *p2, Point_t *p3,
	Point_t *p4);

/*!
 *  @brief Add the points of a Hermite curve to a ::Matrix_t.
 *
 *  @param points A pointer to the ::Matrix_t to add the curve's points to.
 *  @param p1 The first control point.
 *  @param p2 The second control point.
 *  @param p3 The third control point.
 *  @param p4 The fourth control point.
 */
void addHermite(Matrix_t *points, Point_t *p1, Point_t *p2, Point_t *p3,
	Point_t *p4);

/*!
 *  @brief Add the vertices of a rectangular prism to a ::Matrix_t.
 *
 *  @param points A pointer to the ::Matrix_t to add the prism's points to.
 *  @param p1 The front-top-left vertex of the prism.
 *  @param p2 The back-bottom-right vertex of the prism.
 */
void addRectangularPrism(Matrix_t *pts, Point_t *p1, Point_t *p2);

/*!
 *  @brief Add the points of a sphere to a ::Matrix_t.
 *
 *  Internally calls ::generateSphere(), and adds the points of the returned
 *  ::Matrix_t to @p points in such a manner as to facilitate proper triangular
 *  rendering.
 *
 *  @param points A pointer to the ::Matrix_t to add the sphere's points to.
 *  @param origin The origin of the sphere.
 *  @param radius The radius of the sphere.
 */
void addSphere(Matrix_t *points, Point_t *origin, double radius);

/*!
 *  @brief Add the points of a torus to a ::Matrix_t.
 *
 *  Internally calls ::generateTorus(), and adds the points of the returned
 *  ::Matrix_t to @p points in such a manner as to facilitate proper triangular
 *  rendering.
 *
 *  @param points A pointer to the ::Matrix_t to add the torus's points to.
 *  @param origin The origin of the torus.
 *  @param rad1 The minor radius of the torus.
 *  @param rad2 The major radius of the torus.
 */
void addTorus(Matrix_t *points, Point_t *origin, double rad1, double rad2);

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
 *  @brief Render a ::Matrix_t by drawing line segments.
 *
 *  Draw @p matrix by drawing line segments for every pair of points. The first
 *  two  points of ::Matrix_t::points are the endpoints of the first line,
 *  the second two are the endpoints of the second, etc.
 *
 *  @param matrix The ::Matrix_t to be rendered.
 */
void drawMatrixLines(const Matrix_t *const matrix);

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
