/*
 *  screen.c contains functions for initializing, rendering, and modfying an SDL GUI.
*/

#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "screen.h"

SDL_Surface * screen;

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
void drawPixel(int x, int y, Uint32 pixel) {
	Uint8 * pixelAddress = (Uint8 * )screen->pixels + y * screen->pitch +
		x * screen->format->BytesPerPixel;
	*(Uint32 *)pixelAddress = pixel;
}

// render screen, after pixel buffer has been internally updated
void renderScreen(){
	SDL_Flip(screen);
}

void clearScreen(){
	SDL_FillRect(screen, NULL, 0x000000);
}

// close screen, perform memory cleanup
void quitScreen(){
	SDL_Delay(QUIT_DELAY);
	SDL_Quit();
}
