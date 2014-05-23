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

void drawLine(int x1, int y1, int x2, int y2, int color){
	int width = x2 - x1, height = y2 - y1;
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
		drawPixel(x1, y1, color);
		numerator += shortDist;
		if(numerator >= longDist){
			numerator -= longDist;
			x1 += dx1;
			y1 += dy1;
		}
		else {
			x1 += dx2;
			y1 += dy2;
		}
	}
}

void scanlineRender(double x1, double y1, double x2, double y2, double x3,
	double y3, int color){
	double * pts;
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
		drawLine(pts[4], pts[5], mainX, mainY, color);

		pts[4] += m1;
		pts[5]++;

		mainX += m3;
		mainY++;
	}

	while(pts[3] < pts[1]){
		drawLine(pts[2], pts[3], mainX, mainY, color);

		pts[2] += m2;
		pts[3]++;

		mainX += m3;
		mainY++;
	}

}
