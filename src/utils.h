#include <SDL/SDL.h>

#define TEST_COLOR (Uint32)0x00FF0000
#define ABS(val) (val > 0?val:-val)

void drawLine(int x1, int y1, int x2, int y2, Uint32 color);
void drawPolygon(int numSides, int radius, int xOffset, int yOffset, int inclineAngle);
void setup();
void sig_handler(int sig);
