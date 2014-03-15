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

// draw a Bezier curve with control points (x0, y0), (x1, y1), (x2, y2),
// and (x3, y3)
void drawBezier(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3){

	double t, abX, bcX, cdX, abbcX, bccdX, abY, bcY, cdY, bccdY, abbcY;
	for(t = 0; t < 1; t += 0.001){
		abX = INTERPOL(x0, x1);
		bcX = INTERPOL(x1, x2);
		cdX = INTERPOL(x2, x3);
		abbcX = INTERPOL(abX, bcX);
		bccdX = INTERPOL(bcX, cdX);

		abY = INTERPOL(y0, y1);
		bcY = INTERPOL(y1, y2);
		cdY = INTERPOL(y2, y3);
		bccdY = INTERPOL(bcY, cdY);
		abbcY = INTERPOL(abY, bcY);

		addPoint(points, INTERPOL(abbcX, bccdX), INTERPOL(abbcY, bccdY), 0);
	}
}

// draw a Hermite curve with start and endpoints (x0, y0) and (x2, y2), and
// rates of change as calculated using (x1, y1) and (x3, y3).
void drawHermite(Matrix_t * points, int x0, int y0, int x1, int y1, int x2,
	int y2, int x3, int y3){
	float t;

	// calculate rates of change
	int r0X = x1 - x0, r0Y = y1 - y0;
	int r1X = x3 - x2, r1Y = y3 - y2;

	// calculate coefficients
	float aX = 2 * x0 - 2 * x2 + r0X + r1X;
	float aY = 2 * y0 - 2 * y2 + r0Y + r1Y;

	float bX = -3 * x0 + 3 * x2 - 2 * r0X - r1X;
	float bY = -3 * y0 + 3 * y2 - 2 * r0Y - r1Y;

	float cX = r0X;
	float cY = r0Y;

	float dX = x0;
	float dY = y0;

	float t3, t2, x, y;
	for(t = 0.001; t < 1; t += 0.001){
		t3 = t * t * t;
		t2 = t * t;

		x = aX * t3 + bX * t2 + cX * t + dX;
		y = aY * t3 + bY * t2 + cY * t + dY;
		addPoint(points, x, y, 0);
	}
}
