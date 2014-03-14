#pragma once
#include "src/matrix.h"

void drawLine(int x1, int y1, int x2, int y2);
void drawCircle(Matrix_t * points, double oX, double oY, double radius);
void drawPolygon(int numSides, int radius, int xOffset, int yOffset, int inclineAngle);
