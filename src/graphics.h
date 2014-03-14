#pragma once
#include "src/matrix.h"

// rasterize a polygon with a large number of sides
#define drawCircle(points, oX, oY, radius) \
	drawPolygon(points, oX, oY, radius, radius)

void drawLine(int x1, int y1, int x2, int y2);
void drawPolygon(Matrix_t * points, int oX, int oY, int radius, int numSides);
