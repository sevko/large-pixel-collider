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

void (drawLine)(Point_t *p1, Point_t *p2, int color){
	p1 = COPY_POINT(p1);
	int width = p2[X] - p1[X], height = p2[Y] - p1[Y];
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

	if(width < 0)
		dx1 = dx2 = -1;
	else
		dx1 = dx2 = 1;

	if(height < 0)
		dy1 = -1;
	else
		dy1 = 1;

	// Unsigned arithmetic is faster than signed.
	unsigned int longDist = ABS(width);
	unsigned int shortDist = ABS(height);

	if(longDist < shortDist){
		unsigned int tempDist = longDist;
		longDist = shortDist;
		shortDist = tempDist;

		if(height < 0)
			dy2 = -1;
		else
			dy2 = 1;
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

void scanlineRender(Point_t *p1, Point_t *p2, Point_t *p3, int color){
	Point_t **pts;

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
