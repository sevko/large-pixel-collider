#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen.h"

void drawLine(int x1, int y1, int x2, int y2){
	int w = x2 - x1;
	int h = y2 - y1;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

	if(w < 0){
		dx1 = -1;
		dx2 = -1;
	}
	else if(w > 0){
		dx1 = 1;
		dx2 = 1;
	}

	if(h < 0)
		dy1 = -1;
	else if(h > 0)
		dy1 = 1;

	int longest = abs(w);
	int shortest = abs(h);

	if(!(longest > shortest)){
		int temp = longest;
		longest = shortest;
		shortest = temp;

		if(h < 0)
			dy2 = -1;
		else if(h > 0)
			dy2 = 1;
		dx2 = 0;
	}

	int numerator = longest >> 1;
	int i;
	for(i = 0; i <= longest; i++){
		drawPixel(x1, y1, 0x00FF0000);
		numerator += shortest;
		if(!(numerator < longest)){
			numerator -= longest;
			x1 += dx1;
			y1 += dy1;
		}
		else {
			x1 += dx2;
			y1 += dy2;
		}
	}
}
