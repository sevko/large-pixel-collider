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

#define INTERPOLATE_COLOR(l1, l2, guide, axis) \
	({\
		double divisor = 1.0 / (l2->pos[axis] - l1->pos[axis]),\
			colCoef1 = l2->pos[axis] - guide[axis],\
			colCoef2 = guide[axis] - l1->pos[axis];\
		RGB(\
			divisor * (colCoef1 * l1->color[R] + colCoef2 * l2->color[R]),\
			divisor * (colCoef1 * l1->color[G] + colCoef2 * l2->color[G]),\
			divisor * (colCoef1 * l1->color[B] + colCoef2 * l2->color[B])\
		);\
	})

// The exponential rate at which specular light diffuses.
#define SPECULAR_FADE_CONSTANT 10

/*
 * @brief Convert an ::RGB_t to an int.
 *
 * @param color A color.
 *
 * @return An int representing the RGB values stored in @p color, in the form:
 *      0xRRGGBB.
*/
static unsigned int rgbToInt(RGB_t *color);

/*
 * @brief Convert an int to an ::RGB_t.
 *
 * @param color An integer (0xRRGGBB) representation of a color.
 *
 * @return An ::RGB_t with its R, G, and B values corresponding to @p color.
*/
static RGB_t *intToRgb(int color);

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

void drawHorizontalGradientLine(Light_t *p1, Light_t *p2){
	Point_t *guide = COPY_POINT(p1->pos);

	while(guide[X] < p2->pos[X]){
		plotPixel(guide, rgbToInt(INTERPOLATE_COLOR(p1, p2, guide, X)));
		guide[X]++;
	}
}

void scanlineRender(Light_t *l1, Light_t *l2, Light_t *l3){
	Light_t **pts;
	l1->pos = COPY_POINT(l1->pos);
	l2->pos = COPY_POINT(l2->pos);
	l3->pos = COPY_POINT(l3->pos);

	l1->pos[Y] = (int)l1->pos[Y];
	l2->pos[Y] = (int)l2->pos[Y];
	l3->pos[Y] = (int)l3->pos[Y];

	if(l1->pos[Y] >= l2->pos[Y] && l1->pos[Y] >= l3->pos[Y]){
		if(l3->pos[Y] > l2->pos[Y])
			pts = (Light_t *[]){l1, l3, l2};
		else
			pts = (Light_t *[]){l1, l2, l3};
	}

	else if(l2->pos[Y] >= l1->pos[Y] && l2->pos[Y] >= l3->pos[Y]){
		if(l3->pos[Y] > l1->pos[Y])
			pts = (Light_t *[]){l2, l3, l1};
		else
			pts = (Light_t *[]){l2, l1, l3};
	}

	else {
		if(l2->pos[Y] > l1->pos[Y])
			pts = (Light_t *[]){l3, l2, l1};
		else
			pts = (Light_t *[]){l3, l1, l2};
	}

	double m1 = (pts[1]->pos[Y] - pts[2]->pos[Y] != 0)?
			(pts[1]->pos[X] - pts[2]->pos[X]) / (pts[1]->pos[Y] - pts[2]->pos[Y]):0,
		m2 = (pts[0]->pos[Y] - pts[1]->pos[Y] != 0)?
			(pts[0]->pos[X] - pts[1]->pos[X]) / (pts[0]->pos[Y] - pts[1]->pos[Y]):0,
		m3 = (pts[0]->pos[Y] - pts[2]->pos[Y] != 0)?
			(pts[0]->pos[X] - pts[2]->pos[X]) / (pts[0]->pos[Y] - pts[2]->pos[Y]):0;
	Point_t *shortGuide = COPY_POINT(pts[2]->pos),
		*longGuide = COPY_POINT(pts[2]->pos);

	while(shortGuide[Y] < pts[1]->pos[Y]){
		// drawLine(shortGuide, longGuide, color);
		drawHorizontalGradientLine(
			&(Light_t){
				.pos = shortGuide,
				.color = INTERPOLATE_COLOR(pts[2], pts[1], shortGuide, Y)
			},
			&(Light_t){
				.pos = longGuide,
				.color = INTERPOLATE_COLOR(pts[2], pts[0], longGuide, Y)
			}
		);

		shortGuide[X] += m1;
		shortGuide[Y]++;

		longGuide[X] += m3;
		longGuide[Y]++;
	}

	shortGuide = COPY_POINT(pts[1]->pos);

	while(shortGuide[Y] < pts[0]->pos[Y]){
		// drawLine(shortGuide, longGuide, color);
		drawHorizontalGradientLine(
			&(Light_t){
				.pos = shortGuide,
				.color = INTERPOLATE_COLOR(pts[1], pts[0], shortGuide, Y)
			},
			&(Light_t){
				.pos = longGuide,
				.color = INTERPOLATE_COLOR(pts[2], pts[0], longGuide, Y)
			}
		);

		shortGuide[X] += m2;
		shortGuide[Y]++;

		longGuide[X] += m3;
		longGuide[Y]++;
	}
}

RGB_t *flatShade(Point_t *vertex, Point_t *surfaceNorm){
	unsigned int ambientLight = rgbToInt(RGB(
		0.2 * 0x00,
		0.2 * 0x00,
		0.2 * 0xFF
	));

	Light_t diffuseSource = {
		.color = RGB(0xAA, 0xBB, 0x00),
		.pos = POINT(0, 1000, 0, 0)
	};

	Point_t *dLightVector = SUB_POINT(vertex, diffuseSource.pos);
	NORMALIZE(dLightVector);
	double diffuseDot = dotProduct(surfaceNorm, dLightVector);

	unsigned int diffuseLight = (diffuseDot < 0)?0:rgbToInt(RGB(
		diffuseSource.color[R] * diffuseDot,
		diffuseSource.color[G] * diffuseDot,
		diffuseSource.color[B] * diffuseDot
	));

	Light_t specularSource = {
		.color = RGB(0xAA, 0xBB, 0x00),
		.pos = POINT(0, 0, 1, 0)
	};
	Point_t *view = POINT(0, 0, 1, 0);

	Point_t *sLightVector = SUB_POINT(vertex, specularSource.pos);
	NORMALIZE(view);
	NORMALIZE(sLightVector);

	double specularDot = pow(dotProduct(view, sLightVector), 10);
	unsigned int specularLight = (specularDot < 0)?0:rgbToInt(RGB(
		specularSource.color[R] * specularDot,
		specularSource.color[G] * specularDot,
		specularSource.color[B] * specularDot
	));

	unsigned int sum = ambientLight + diffuseLight + specularLight;
	return intToRgb((0xFFFFFF < sum)?0xFFFFFF:sum);
}

static unsigned int rgbToInt(RGB_t *color){
	if(0xFFFFFF < ((color[R] << 4 * 4) & (color[G] << 4 * 2) & color[B]))
		FATAL("Stop: %X, %X, %X", color[R], color[G], color[B]);

	return (color[R] << 4 * 4) + (color[G] << 4 * 2) + color[B];
}

static RGB_t *intToRgb(int color){
	RGB_t *rgb = malloc(3 * sizeof(RGB_t));
	rgb[R] = color >> 4 * 4;
	rgb[G] = (color & 0x00FF00) >> 4 * 2;
	rgb[B] = color & 0x0000FF;
	return rgb;
}
