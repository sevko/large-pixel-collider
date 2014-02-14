#define PI 3.14159265359

#define NUM_MAX_POINTS 2000
#define NUM_POINT_VARS 4

void drawLine(int x1, int y1, int x2, int y2);
void drawPolygon(int numSides, int radius, int xOffset, int yOffset, int inclineAngle);
void drawPointsLines();
void addPoint(int x, int y, int z);
