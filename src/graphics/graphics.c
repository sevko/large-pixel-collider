#include <stdio.h>

#include "src/graphics/graphics.h"
#include "src/graphics/screen.h"

/*!
 *  @brief Return the absolute value of a numeric value.
 *
 *  @param val The numeric value to be interpreted.
 *  @return The absolute value of @p val.
 */
#define ABS(val) (val > 0?val:-val)

// The exponential rate at which specular light diffuses.
#define SPECULAR_FADE_CONSTANT 10

/*
 * @brief Calculate the flat-shaded color of a triangle.
 *
 * @param p1 The first vertex of the triangle.
 * @param p2 The second vertex of the triangle.
 * @param p3 The third vertex of the triangle.
 *
 * @return The RGB color of the triangle with ambient, diffuse, and spectral
 *      lighting applied.
*/
static int flatShade(Point_t *p1, Point_t *p2, Point_t *p3);

/*
 * @brief Convert an ::RGB_t to an int.
 *
 * @param color A color.
 *
 * @return An int representing the RGB values stored in @p color, in the form:
 *      0xRRGGBB.
*/
static int rgbToInt(RGB_t *color);

void (drawLine)(Point_t *p1, Point_t *p2, int color){
	p1 = COPY_POINT(p1);
	int width = p2[X] - p1[X],
		height = p2[Y] - p1[Y];
	int dx1 = 0,
		dy1 = 0,
		dx2 = 0,
		dy2 = 0;

	dx1 = dx2 = (width < 0)?-1:1;
	dy1 = (height < 0)?-1:1;

	// Unsigned arithmetic is faster than signed.
	unsigned int longDist = ABS(width) + 1;
	unsigned int shortDist = ABS(height);

	if(longDist < shortDist){
		unsigned int tempDist = longDist;
		longDist = shortDist;
		shortDist = tempDist;

		dy2 = (height < 0)?-1:1;
		dx2 = 0;
	}

	unsigned int numerator = longDist >> 1, pixel;
	for(pixel = 0; pixel <= longDist; pixel++){
		plotPixel(p1, color);
		numerator += shortDist;
		if(numerator >= longDist){
			numerator -= longDist;
			p1[X] += dx1;
			p1[Y] += dy1;
		}
		else {
			p1[X] += dx2;
			p1[Y] += dy2;
		}
	}
}

void scanlineRender(Point_t *p1, Point_t *p2, Point_t *p3){
	Point_t **pts;
	int color = flatShade(p1, p2, p3);

	p1 = COPY_POINT(p1);
	p2 = COPY_POINT(p2);
	p3 = COPY_POINT(p3);

	p1[Y] = (int)p1[Y];
	p2[Y] = (int)p2[Y];
	p3[Y] = (int)p3[Y];

	if(p1[Y] >= p2[Y] && p1[Y] >= p3[Y]){
		if(p3[Y] > p2[Y])
			pts = (Point_t *[]){p1, p3, p2};
		else
			pts = (Point_t *[]){p1, p2, p3};
	}

	else if(p2[Y] >= p1[Y] && p2[Y] >= p3[Y]){
		if(p3[Y] > p1[Y])
			pts = (Point_t *[]){p2, p3, p1};
		else
			pts = (Point_t *[]){p2, p1, p3};
	}

	else {
		if(p2[Y] > p1[Y])
			pts = (Point_t *[]){p3, p2, p1};
		else
			pts = (Point_t *[]){p3, p1, p2};
	}

	double m1 = (pts[1][Y] - pts[2][Y] != 0)?
			(pts[1][X] - pts[2][X]) / (pts[1][Y] - pts[2][Y]):0,
		m2 = (pts[0][Y] - pts[1][Y] != 0)?
			(pts[0][X] - pts[1][X]) / (pts[0][Y] - pts[1][Y]):0,
		m3 = (pts[0][Y] - pts[2][Y] != 0)?
			(pts[0][X] - pts[2][X]) / (pts[0][Y] - pts[2][Y]):0;
	Point_t *guide = COPY_POINT(pts[2]);

	while(pts[2][Y] < pts[1][Y]){
		drawLine(pts[2], guide, color);

		pts[2][X] += m1;
		pts[2][Y]++;

		guide[X] += m3;
		guide[Y]++;
	}

	while(pts[1][Y] < pts[0][Y]){
		drawLine(pts[1], guide, color);
		pts[1][X] += m2;
		pts[1][Y]++;

		guide[X] += m3;
		guide[Y]++;
	}
}

static int flatShade(Point_t *p1, Point_t *p2, Point_t *p3){
	int ambientLight = rgbToInt(RGB(
		0.2 * 0x00,
		0.2 * 0x00,
		0.2 * 0xFF
	));

	Light_t diffuseSource = {
		.color = RGB(0xAA, 0xBB, 0x00),
		.pos = POINT(0, 1000, 0)
	};

	Point_t *norm = surfaceNormal(p1, p2, p3),
		*dLightVector = SUB_POINT(p3, diffuseSource.pos);
	NORMALIZE(norm);
	NORMALIZE(dLightVector);
	double diffuseDot = dotProduct(norm, dLightVector);

	int diffuseLight = (diffuseDot < 0)?0:rgbToInt(RGB(
		diffuseSource.color[R] * diffuseDot,
		diffuseSource.color[G] * diffuseDot,
		diffuseSource.color[B] * diffuseDot
	));
	free(norm);

	Light_t specularSource = {
		.color = RGB(0xAA, 0xBB, 0x00),
		.pos = POINT(0, 0, 1, 0)
	};
	Point_t *view = POINT(0, 0, 1, 0);

	Point_t *sLightVector = SUB_POINT(p3, specularSource.pos);
	NORMALIZE(view);
	NORMALIZE(sLightVector);

	double specularDot = pow(dotProduct(view, sLightVector), 10);
	int specularLight = (specularDot < 0)?0:rgbToInt(RGB(
		specularSource.color[R] * specularDot,
		specularSource.color[G] * specularDot,
		specularSource.color[B] * specularDot
	));

	int sum = ambientLight + diffuseLight + specularLight;
	return (0xFFFFFF < sum)?0xFFFFFF:sum;
}

static int rgbToInt(RGB_t *color){
	return (color[R] << 4 * 4) + (color[G] << 4 * 2) + color[B];
}
