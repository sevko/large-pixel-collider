/*!
 * @file
 * @brief Constructs for shape creation.
 *
 * A collection of functions and macros used to add various shapes
 * (eg, tori, spheres) to ::Matrix_t.
*/

#pragma once

#include "src/graphics/matrix.h"

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

// The angle between the subsequent, rotated circles that compose a sphere.
#define CIRCLE_STEP_SIZE 8

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
