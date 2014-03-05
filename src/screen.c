/*
 *  screen.c contains functions for initializing, rendering, and drawing to an
 *  SDL GUI.
*/

#include <SDL/SDL.h>

#include "src/globals.h"
#include "src/screen.h"

#define QUIT_DELAY 400                  // milliseconds before screen quits
#define SCREEN_NAME "sevko's window"
#define TEST_COLOR (Uint32)0x00FF0000

static SDL_Surface * screen;

// initialize and configure screen
void configureScreen(){
	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)){
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	screen = SDL_SetVideoMode(IMAGE_WIDTH, IMAGE_HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption(SCREEN_NAME, NULL);
}

// plot pixel to screen
void drawPixel(int x, int y) {
	if(x < 0 || IMAGE_WIDTH - 1 < x || y < 0 || IMAGE_HEIGHT - 1 < y)
		return;

	Uint8 * pixelAddress = (Uint8 * )screen->pixels + y * screen->pitch +
		x * screen->format->BytesPerPixel;
	*(Uint32 *)pixelAddress = TEST_COLOR;
}

// render screen, after pixel buffer has been internally updated
void renderScreen(){
	SDL_Flip(screen);
}

// fill the screen with black pixels
void clearScreen(){
	SDL_FillRect(screen, NULL, 0x000000);
}

// close screen, perform memory cleanup
void quitScreen(){
	SDL_Delay(QUIT_DELAY);
	SDL_Quit();
}

// write the screen's pixels matrix to a BMP file named filename
int writeScreen(const char * const filename){
	return SDL_SaveBMP(screen, filename);
}
