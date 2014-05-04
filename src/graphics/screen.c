#include <SDL.h>

#include "src/globals.h"
#include "src/graphics/screen.h"

/*!
 *  The millisecond delay before the SDL screen quits after quitScreen() is
 *  called.
 */
#define QUIT_DELAY 400

//! The name of the SDL screen.
#define SCREEN_NAME "Graphics Engine: Screen"

/*!
 *  @brief The global SDL screen -- global because only one will be necessary
 *      per run of the engine, and it renders use of the module cleaner.
 */
static SDL_Surface * screen;

void configureScreen(void){
	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)){
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	screen = SDL_SetVideoMode(IMAGE_WIDTH, IMAGE_HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption(SCREEN_NAME, NULL);
}

void (drawPixel)(int x, int y, int color) {
	x += IMAGE_WIDTH / 2;
	y += IMAGE_HEIGHT / 2;
	if(x < 0 || IMAGE_WIDTH - 1 < x || y < 0 || IMAGE_HEIGHT - 1 < y)
		return;

	Uint8 * pixelAddress = (Uint8 * )screen->pixels + y * screen->pitch +
		x * screen->format->BytesPerPixel;
	*(Uint32 *)pixelAddress = color;
}

void renderScreen(void){
	SDL_Flip(screen);
}

void clearScreen(void){
	SDL_FillRect(screen, NULL, 0x000000);
}

void quitScreen(void){
	SDL_Delay(QUIT_DELAY);
	SDL_Quit();
}

int writeScreen(const char * const filename){
	return SDL_SaveBMP(screen, filename);
}
