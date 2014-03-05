#include <SDL/SDL.h>

#define SCREEN_NAME "sevko's window"
#define TICK_MS (1e3 / 30.0)
#define QUIT_DELAY 400

void configureScreen();
void drawPixel(int x, int y, Uint32 pixel);
void renderScreen();
void clearScreen();
void quitScreen();
int writeScreen(const char * const filename);
