#include <SDL.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/graphics/matrix.h"

// The ms delay before the SDL screen quits after ::quitScreen() is called.
#define QUIT_DELAY 400

// The name of the SDL screen.
#define SCREEN_NAME "Graphics Engine: Screen"

// Used for z-buffering.
typedef struct {
	// 3D representation of each pixel's current height/color.
	double buf[IMAGE_HEIGHT][IMAGE_WIDTH][2];
} ZBuffer_t;

static SDL_Surface * g_screen; // The engine's SDL screen.
static ZBuffer_t * g_zbuffer; // The screen's z-buffer.

/*
 * @brief Draw a pixel on the SDL screen.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
*/
static void drawPixel(int x, int y, int color);

void configureScreen(void){
	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1))
		FATAL("Could not initialize SDL: %s.\n", SDL_GetError());
	g_screen = SDL_SetVideoMode(IMAGE_WIDTH, IMAGE_HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption(SCREEN_NAME, NULL);

	g_zbuffer = malloc(sizeof(ZBuffer_t));
}

void (plotPixel)(Point_t *pt, int color){
	int x = pt[X] + IMAGE_WIDTH / 2,
		y = pt[Y] + IMAGE_HEIGHT / 2;

	if(!(x < 0 || IMAGE_WIDTH - 1 < x || y < 0 || IMAGE_HEIGHT - 1 < y) &&
		(g_zbuffer->buf[y][x][1] == -1 || g_zbuffer->buf[y][x][0] < pt[Z])){
		g_zbuffer->buf[y][x][0] = pt[Z];
		g_zbuffer->buf[y][x][1] = color;
	}
}

void renderScreen(void){
	int y, x;
	for(y = 0; y < IMAGE_HEIGHT; y++)
		for(x = 0; x < IMAGE_WIDTH; x++)
			drawPixel(x, y, g_zbuffer->buf[y][x][1]);
	SDL_Flip(g_screen);
}

void clearScreen(void){
	memset(g_zbuffer->buf, -1, sizeof(double) * IMAGE_HEIGHT * IMAGE_WIDTH * 2);
	SDL_FillRect(g_screen, NULL, 0x000000);
}

void quitScreen(void){
	SDL_Delay(QUIT_DELAY);
	SDL_Quit();
}

int writeScreen(const char * const filename){
	return SDL_SaveBMP(g_screen, filename);
}

static void drawPixel(int x, int y, int color){
	Uint8 * pixelAddress = (Uint8 *)g_screen->pixels + y * g_screen->pitch +
		x * g_screen->format->BytesPerPixel;
	*(Uint32 *)pixelAddress = color;
}
