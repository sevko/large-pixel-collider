#define NUM_MAX_POINTS 2000
#define NUM_POINT_VARS 4
#define PI 3.14159265359
#define TEST_COLOR (Uint32)0x00FF0000

void drawLine(int x1, int y1, int x2, int y2, Uint32 color);
void drawPolygon(int numSides, int radius, int xOffset, int yOffset, int inclineAngle);
void drawPointsLines();
void addPoint(int x, int y, int z);
