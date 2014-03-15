/*
 *  utils.c contains processing functions to render lines and shapes.
*/

#include <math.h>
#include <stdio.h>

#include "src/graphics.h"
#include "src/screen.h"

#define ABS(val) (val > 0?val:-val)
#define INTERPOL(a, b) (a + (b - a) * t)

// Bresenham rasterize line with endpoints (x1, y1) and (x2, y2)
void drawLine(int x1, int y1, int x2, int y2){
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

	// unsigned arithmetic is faster
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
		drawPixel(x1, y1);
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

// draw a polygon with origin (oX, oY), radius, and numSides
void drawPolygon(Matrix_t * points, int oX, int oY, int radius, int numSides){
	double theta = 2 * M_PI / numSides;
	double tanFactor = tan(theta), radFactor = cos(theta);
	double x = radius, y = 0;

	int segment;
	for(segment = 0; segment < numSides; segment++){
		addPoint(points, x + oX, y + oY, 0);
		float tempX = x;
		x = (x - y * tanFactor) * radFactor;
		y = (y + tempX * tanFactor) * radFactor;
		addPoint(points, x + oX, y + oY, 0);
	}
}

void drawBezier(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3){

	double t;
	for(t = 0; t < 1; t += 0.001){
		double abX = INTERPOL(x0, x1);
		double bcX = INTERPOL(x1, x2);
		double cdX = INTERPOL(x2, x3);
		double abbcX = INTERPOL(abX, bcX);
		double bccdX = INTERPOL(bcX, cdX);

		double abY = INTERPOL(y0, y1);
		double bcY = INTERPOL(y1, y2);
		double cdY = INTERPOL(y2, y3);
		double bccdY = INTERPOL(bcY, cdY);
		double abbcY = INTERPOL(abY, bcY);

		addPoint(points, INTERPOL(abbcX, bccdX), INTERPOL(abbcY, bccdY), 0);
	}
}
