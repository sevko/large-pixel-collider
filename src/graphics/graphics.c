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

/*
 * @brief Interpolate the intensity (color) of a ::Point_t along a line between
 *      two ::Light_t.
 *
 * @param l1 (::Light_t *) The first light endpoint.
 * @param l2 (::Light_t *) The second light endpoint.
 * @param guide (::Point_t *) A point along the line formed by @p l1 and @p l2.
 * @param axis (int) Determines which axis to interpolate along: either X or Y.
*/
#define INTERPOLATE_COLOR(l1, l2, guide, axis) \
	({\
		if(l1->pos[axis] > l2->pos[axis]){\
			Light_t *tmp = l1;\
			l1 = l2;\
			l2 = tmp;\
		}\
		\
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
#define SPECULAR_FADE_CONSTANT 2

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
 * @brief Return the inverse slope of a line.
 *
 * @param p1 The first endpoint.
 * @param p2 The second endpoint.
 *
 * @return The inverse slope (delta x)/(delta y) of the line formed by endpoints
 *      @p p1 and @p p2.
*/
static double inverseSlope(Point_t *p1, Point_t *p2);

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

void drawHorizontalGradientLine(Light_t *light1, Light_t *light2){
	if(light1->pos[X] >= light2->pos[X]){
		Light_t *tmp = light1;
		light1 = light2;
		light2 = tmp;
	}
	Point_t *guide = COPY_POINT(light1->pos);

	while(guide[X] < light2->pos[X]){
		plotPixel(guide, rgbToInt(INTERPOLATE_COLOR(light1, light2, guide, X)));
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

	double m1 = inverseSlope(pts[1]->pos, pts[2]->pos),
		m2 = inverseSlope(pts[0]->pos, pts[1]->pos),
		m3 = inverseSlope(pts[0]->pos, pts[2]->pos);

	Point_t *shortGuide = COPY_POINT(pts[2]->pos),
		*longGuide = COPY_POINT(pts[2]->pos);

	while(shortGuide[Y] < pts[1]->pos[Y]){
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
	RGB_t *ambientLight = RGB(
		0.2 * 0x00,
		0.2 * 0x00,
		0.2 * 0x22
	);

	Light_t diffuseSource = {
		.color = RGB(0x00, 0x00, 0xAA),
		.pos = POINT(0, 1000, 0, 0)
	};

	Point_t *dLightVector = NORMALIZE(SUB_POINT(vertex, diffuseSource.pos));
	double diffuseDot = dotProduct(surfaceNorm, dLightVector);

	RGB_t *diffuseLight = (diffuseDot < 0)?RGB(0, 0, 0):RGB(
		diffuseSource.color[R] * diffuseDot,
		diffuseSource.color[G] * diffuseDot,
		diffuseSource.color[B] * diffuseDot
	);

	Light_t specularSource = {
		.color = RGB(0xFF, 0xFF, 0xFF),
		.pos = POINT(10, -100, 50, 0)
	};
	Point_t *view = POINT(0, 0, 1, 0);
	Point_t *sLightVector = NORMALIZE(SUB_POINT(vertex, specularSource.pos));
	double specularDot = pow(dotProduct(view, sLightVector), 10);
	RGB_t *specularLight = (specularDot < 0)?RGB(0, 0, 0):RGB(
		specularSource.color[R] * specularDot,
		specularSource.color[G] * specularDot,
		specularSource.color[B] * specularDot
	);

	// unsigned int sum = ambientLight + diffuseLight + specularLight;
	RGB_t *sum = malloc(3 * sizeof(RGB_t));
	sum[R] = ambientLight[R] + diffuseLight[R] + specularLight[R];
	sum[G] = ambientLight[G] + diffuseLight[G] + specularLight[G];
	sum[B] = ambientLight[B] + diffuseLight[B] + specularLight[B];

	int col;
	for(col = 0; col < 3; col++)
		if(0xFF < sum[col])
			sum[col] = 0xFF;
	return sum;
}

static unsigned int rgbToInt(RGB_t *color){
	if(0xFFFFFF < ((color[R] << 4 * 4) & (color[G] << 4 * 2) & color[B]))
		FATAL("Stop: %X, %X, %X", color[R], color[G], color[B]);

	return (color[R] << 4 * 4) + (color[G] << 4 * 2) + color[B];
}

static double inverseSlope(Point_t *p1, Point_t *p2){
	double deltaY = p1[Y] - p2[Y];
	return (deltaY != 0)?(p1[X] - p2[X]) / (deltaY):0;
}
