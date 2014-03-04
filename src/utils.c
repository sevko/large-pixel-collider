/*
 *  utils.c contains processing functions to render lines and shapes.
*/

#include <math.h>
#include <SDL/SDL.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/utils.h"
#include "src/screen.h"

#define ABS(val) (val > 0?val:-val)

static void sigHandler(int sig);

// Bresenham rasterize line with endpoints (x1, y1) and (x2, y2)
void drawLine(int x1, int y1, int x2, int y2, Uint32 color){
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

// draw a polygon with numSides sides, a radius of radius pixels, centered on
// (xOffset, yOffset)
void drawPolygon(int numSides, int radius, int xOffset, int yOffset, int inclineAngle){
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

		drawLine(x1, y1, x2, y2, TEST_COLOR);
	}
}

static void sigHandler(int sig){
	(void)sig;
	exit(EXIT_SUCCESS);
}

void setup(){
	signal(SIGINT, sigHandler);
}
