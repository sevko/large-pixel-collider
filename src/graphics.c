#include <stdio.h>

#include "src/graphics.h"
#include "src/screen.h"

/*!
 *  @brief Return the absolute value of a numeric value.
 *
 *  @param val The numeric value to be interpreted.
 *  @return The absolute value of @p val.
 */
#define ABS(val) (val > 0?val:-val)

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
