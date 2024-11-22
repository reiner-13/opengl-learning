
#include <SDL.h>
#include <stdio.h>

bool init();
bool loadMedia();
void close();

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
// Image we will load and show on screen
SDL_Surface* gImage = NULL;

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize. SDL_ERROR: %s\n", SDL_GetError());
		success = false;
	}
	else {
		gWindow = SDL_CreateWindow("Window McWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			// Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	return success;
}

bool loadMedia() {
	bool success = true;

	gImage = SDL_LoadBMP("hello_world.bmp");
	if (gImage == NULL) {
		printf("Unable to load image %s. SDL Error: %s\n", "hello_world.bmp", SDL_GetError());
		success = false;
	}
	return success;
}

void close() {
	// Deallocate surface
	SDL_FreeSurface(gImage);
	gImage = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}

int main(int argc, char* args[]) {
	if (!init()) {
		printf("Failed to initialize.\n");
	}
	else {
		// Load media
		if (!loadMedia()) {
			printf("Failed to load media.\n");
		}
		else {
			SDL_BlitSurface(gImage, NULL, gScreenSurface, NULL);
			SDL_UpdateWindowSurface(gWindow);

			SDL_Event e; bool quit = false; while (quit == false) { while( SDL_PollEvent(&e)) { if(e.type == SDL_QUIT) quit = true; } }
		}
	}

	close();

	return 0;
}
