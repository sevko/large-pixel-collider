#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen.h"

void drawLine(int x1, int y1, int x2, int y2){
	int width = x2 - x1, height = y2 - y1;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

	if(width < 0)
		dx1 = dx2 = -1;
	else if(width > 0)
		dx1 = dx2 = 1;

	if(height < 0)
		dy1 = -1;
	else if(height > 0)
		dy1 = 1;

	int longDist = abs(width);
	int shortDist = abs(height);

	if(longDist < shortDist){
		int tempDist = longDist;
		longDist = shortDist;
		shortDist = tempDist;

		if(height < 0)
			dy2 = -1;
		else if(height > 0)
			dy2 = 1;
		dx2 = 0;
	}

	int numerator = longDist >> 1;
	int pixel;
	for(pixel = 0; pixel <= longDist; pixel++){
		drawPixel(x1, y1, 0x00FF0000);
		numerator += shortDist;
		if(!(numerator < longDist)){
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
