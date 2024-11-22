
#include <SDL.h>
#include <stdio.h>
#include <string>

bool init();
bool loadMedia();
void close();

enum KeyPressSurfaces {
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Surface* loadSurface(std::string path);
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface* gCurrentSurface = NULL;

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

	
	return success;
}

void close() {

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
			SDL_Event e;
			bool quit = false;
			while (!quit) {
				while (SDL_PollEvent(&e)) {
					if (e.type == SDL_QUIT)
						quit = true;
					else if (e.type == SDL_KEYDOWN) {
						switch (e.key.keysym.sym) {
							case SDLK_UP:
							SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xEE, 0xEE, 0xEE));
							break;

							case SDLK_DOWN:
							SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xDD, 0xDD, 0xDD));
							break;

							case SDLK_LEFT:
							SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xCC, 0xCC, 0xCC));
							break;

							case SDLK_RIGHT:
							SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xBB, 0xBB, 0xBB));
							break;

							default:
							SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xAA, 0x22, 0xCC));
							break;
						}
					}
				}
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	close();

	return 0;
}
