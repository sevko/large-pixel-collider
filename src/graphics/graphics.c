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
		drawPixel(p1, color);
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
	double *pts;
	double x1 = p1[X];
	double y1 = p1[Y];
	double x2 = p2[X];
	double y2 = p2[Y];
	double x3 = p3[X];
	double y3 = p3[Y];
	if(y1 >= y2 && y1 >= y3){
		if(y3 > y2)
			pts = (double []){x1, y1, x3, y3, x2, y2};
		else
			pts = (double []){x1, y1, x2, y2, x3, y3};
	}

	else if(y2 >= y1 && y2 >= y3){
		if(y3 > y1)
			pts = (double []){x2, y2, x3, y3, x1, y1};
		else
			pts = (double []){x2, y2, x1, y1, x3, y3};
	}

	else {
		if(y2 > y1)
			pts = (double []){x3, y3, x2, y2, x1, y1};
		else
			pts = (double []){x3, y3, x1, y1, x2, y2};
	}

	double m1 = (pts[3] - pts[5] != 0)?(pts[2] - pts[4]) / (pts[3] - pts[5]):0,
		m2 = (pts[1] - pts[3] != 0)?(pts[0] - pts[2]) / (pts[1] - pts[3]):0,
		m3 = (pts[1] - pts[5] != 0)?(pts[0] - pts[4]) / (pts[1] - pts[5]):0;
	double mainX = pts[4],
		mainY = pts[5];

	while(pts[5] < pts[3]){
		drawLine(POINT(pts[4], pts[5]), POINT(mainX, mainY), color);

		pts[4] += m1;
		pts[5]++;

		mainX += m3;
		mainY++;
	}

	while(pts[3] < pts[1]){
		drawLine(POINT(pts[2], pts[3]), POINT(mainX, mainY), color);
		pts[2] += m2;
		pts[3]++;

		mainX += m3;
		mainY++;
	}
}
