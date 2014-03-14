/*
 *  utils.c contains processing functions to render lines and shapes.
*/

#include <math.h>
#include <stdio.h>

#include "src/graphics.h"
#include "src/utils.h"
#include "src/screen.h"

#define ABS(val) (val > 0?val:-val)

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

// rasterize a circle with origin (oX, oY) and radius by adding edges to points
void drawCircle(Matrix_t * points, double oX, double oY, double radius){
	double theta = 1 / radius;
	double tanFactor = tan(theta), radFactor = cos(theta);
	double x = radius, y = 0;

	int segment;
	for(segment = 0; segment < radius * 2 * M_PI; segment++){
		addPoint(points, x + oX, y + oY, 0);
		float tempX = x;
		x = (x - y * tanFactor) * radFactor;
		y = (y + tempX * tanFactor) * radFactor;
		addPoint(points, x + oX, y + oY, 0);
	}
}

// draw a polygon with numSides sides, a radius of radius pixels, centered on
// (xOffset, yOffset)
void drawPolygon(int numSides, int radius, int xOffset, int yOffset,
	int inclineAngle){
	double angle = (M_PI * 2) / numSides;
	double incline = (180 / M_PI) * inclineAngle;

	int side;
	double currAngle = M_PI * 0.5 - angle / 2 - incline;
	for(side = 0; side < numSides; side++){
		int x1 = xOffset + radius * cos(currAngle);
		int x2 = xOffset + radius * cos(currAngle + angle);
		int y1 = yOffset + radius * sin(currAngle);
		int y2 = yOffset + radius * sin(currAngle + angle);
		currAngle += angle;

		drawLine(x1, y1, x2, y2);
	}
}
