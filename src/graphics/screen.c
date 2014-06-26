#include <SDL.h>
#include <X11/Xlib.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/graphics/matrix.h"

// The ms delay before the SDL screen quits after ::quitScreen() is called.
#define QUIT_DELAY 400
#define SCREEN_NAME "Graphics Engine: Screen" // The name of the SDL screen.

int g_screenWidth, // the width of ::g_screen
	g_screenHeight; // the height of ::g_screen
static SDL_Surface *g_screen; // The engine's SDL screen.
ZBuffer_t *g_zbuffer = NULL; // The screen's z-buffer.

/*
 * @brief Draw a pixel on the SDL screen.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
*/
static inline void drawPixel(int x, int y, int color);

void configureScreen(void){
	Display *display = XOpenDisplay(NULL);
	Screen *screen = DefaultScreenOfDisplay(display);
	g_screenWidth = (int)(screen->width * 0.6);
	g_screenHeight = (int)(screen->height * 0.8);
	XCloseDisplay(display);

	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1))
		FATAL("Could not initialize SDL: %s.\n", SDL_GetError());
	g_screen = SDL_SetVideoMode(g_screenWidth, g_screenHeight, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption(SCREEN_NAME, NULL);

	g_zbuffer = createZBuffer();
}

void (plotPixel)(Point_t *pt, int color){
	int x = pt[X] + g_screenWidth / 2,
		y = pt[Y] + g_screenHeight / 2;

	if(!(x < 0 || g_screenWidth - 1 < x || y < 0 || g_screenHeight - 1 < y) &&
		(g_zbuffer->buf[y][x][1] == -1 || g_zbuffer->buf[y][x][0] < pt[Z])){
		g_zbuffer->buf[y][x][0] = pt[Z];
		g_zbuffer->buf[y][x][1] = color;
	}
}

void renderScreen(void){
	int y, x;
	for(y = 0; y < g_screenHeight; y++)
		for(x = 0; x < g_screenWidth; x++)
			drawPixel(x, y,
					g_zbuffer->buf[y][x][1] == -1?0x0:g_zbuffer->buf[y][x][1]);
	SDL_Flip(g_screen);
}

void clearScreen(void){
	clearZBuffer(g_zbuffer);
	// SDL_FillRect(g_screen, NULL, 0x000000);
}

void quitScreen(void){
	freeZBuffer(g_zbuffer);
	SDL_Delay(QUIT_DELAY);
	SDL_Quit();
}

int writeScreen(const char * const filename){
	return SDL_SaveBMP(g_screen, filename);
}

ZBuffer_t *createZBuffer(void){
	ZBuffer_t *zBuf = malloc(sizeof(ZBuffer_t));
	zBuf->buf = malloc(g_screenHeight * sizeof(double *));

	int y;
	for(y = 0; y < g_screenHeight; y++){
		zBuf->buf[y] = malloc(g_screenWidth * sizeof(double *));
		int x;
		for(x = 0; x < g_screenWidth; x++){
			zBuf->buf[y][x] = malloc(2 * sizeof(double));
			zBuf->buf[y][x][0] = 0;
			zBuf->buf[y][x][1] = -1;
		}
	}

	return zBuf;
}

void freeZBuffer(ZBuffer_t *zBuf){
	int y;
	for(y = 0; y < g_screenHeight; y++){
		int x;
		for(x = 0; x < g_screenWidth; x++)
			free(zBuf->buf[y][x]);
		free(zBuf->buf[y]);
	}

	free(zBuf->buf);
	free(zBuf);
}

void clearZBuffer(ZBuffer_t *zBuf){
	int y, x;
	for(y = 0; y < g_screenHeight; y++)
		for(x = 0; x < g_screenWidth; x++){
			zBuf->buf[y][x][0] = 0;
			zBuf->buf[y][x][1] = -1;
		}
}

ZBuffer_t *readZBufferFromFile(const char *filePath){
	char *fullFilePath = malloc(strlen(filePath) + strlen(TEST_FILE_DIR) + 1);
	strcpy(fullFilePath, TEST_FILE_DIR);
	strcat(fullFilePath, filePath);
	FILE *file = fopen(fullFilePath, "r");

	int width, height;
	if(fscanf(file, "%d, %d:", &width, &height) < 1)
		FATAL("Reading '%s'. Failed to read width and height.", fullFilePath);

	if(width != g_screenWidth || height != g_screenHeight)
		return NULL;

	ZBuffer_t *zBuf = createZBuffer();

	int y, x;
	for(y = 0; y < g_screenHeight; y++)
		for(x = 0; x < g_screenWidth; x++)
			if(fscanf(file, "%lf,%lf,", &zBuf->buf[y][x][0],
				&zBuf->buf[y][x][1]) < 2)
				FATAL("Reading '%s'. Failed to read pixel (%d, %d).",
					fullFilePath, x, y);

	fclose(file);
	free(fullFilePath);
	return zBuf;
}

void writeZBufferToFile(ZBuffer_t *zBuf, const char *filePath){
	char *fullFilePath = malloc(strlen(filePath) + strlen(TEST_FILE_DIR) + 1);
	strcpy(fullFilePath, TEST_FILE_DIR);
	strcat(fullFilePath, filePath);
	FILE *file = fopen(fullFilePath, "w");
	free(fullFilePath);

	fprintf(file, "%d, %d:", g_screenWidth, g_screenHeight);
	int y, x;
	for(y = 0; y < g_screenHeight; y++)
		for(x = 0; x < g_screenWidth; x++)
			fprintf(
					file, "%d,%d,", (int)zBuf->buf[y][x][0],
					(int)zBuf->buf[y][x][1]);

	fclose(file);
}

int equalZBuffers(ZBuffer_t *zBuf1, ZBuffer_t *zBuf2){
	int y, x;
	for(y = 0; y < g_screenHeight; y++)
		for(x = 0; x < g_screenWidth; x++)
			if((int)zBuf1->buf[y][x][0] != (int)zBuf2->buf[y][x][0] ||
				(int)zBuf1->buf[y][x][1] != (int)zBuf2->buf[y][x][1])
				return 0;
	return 1;
}

static inline void drawPixel(int x, int y, int color){
	Uint8 * pixelAddress = (Uint8 *)g_screen->pixels + y * g_screen->pitch +
		x * g_screen->format->BytesPerPixel;
	*(Uint32 *)pixelAddress = color;
};
