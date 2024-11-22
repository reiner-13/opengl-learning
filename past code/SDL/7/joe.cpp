
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

bool init();
bool loadMedia();
void close();
SDL_Texture* loadTexture(std::string path);

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;

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
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

			if (gRenderer == NULL) {
				printf("Renderer could not be created.");
				success = false;
			}
			else {
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				int imgFlags = IMG_INIT_PNG;

				if (!IMG_Init(imgFlags) & imgFlags) {
					printf("SDL_image could not initialize.");
					success = false;
				}
			}
		}
	}

	return success;
}

SDL_Texture* loadTexture(std::string path) {
	SDL_Texture* newTexture = IMG_LoadTexture(gRenderer, path.c_str());
	
	if (newTexture == NULL) {
		printf("Unable to load texture.");
	}

	return newTexture;
}

bool loadMedia() {
	bool success = true;

	gTexture = loadTexture("./wario.png");
	if (gTexture == NULL) {
		printf("Failed to load texture.\n");
		success = false;
	}
	
	return success;
}

void close() {
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
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
					if (e.type == SDL_QUIT) {
						quit = true;
					}
				}
				// Clear screen
				SDL_RenderClear(gRenderer);

				// Render texture to screen
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

				// Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	close();

	return 0;
}
