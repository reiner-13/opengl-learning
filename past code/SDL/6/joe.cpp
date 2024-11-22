
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

bool init();
bool loadMedia();
void close();

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Surface* loadSurface(std::string path);
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gStretchedSurface = NULL;

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
			int imgFlags = IMG_INIT_PNG;

			if (!IMG_Init(imgFlags) & imgFlags) {
				printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
			else {
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
		}
	}

	return success;
}

SDL_Surface* loadSurface(std::string path) {
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL) {
		printf("Unable to load image %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else {
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image.");
		}
		
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

bool loadMedia() {
	bool success = true;

	gStretchedSurface = loadSurface("./wario.png");
	if (gStretchedSurface == NULL) {
		printf("Failed to load stretching image.\n");
		success = false;
	}
	
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
				}
				SDL_Rect stretchRect;
				stretchRect.x = 0;
				stretchRect.y = 0;
				stretchRect.w = SCREEN_WIDTH;
				stretchRect.h = SCREEN_HEIGHT;
				SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);

				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	close();

	return 0;
}
