/** Copyright (c) 2021
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * Authored by: Kris Henriksen <krishenriksen.work@gmail.com>
 *
 * AnberPorts
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengles.h>
#include <stdbool.h>
#include <dirent.h> 
#include <stdio.h>
#include "font.h"

// Screen dimension constants
int SCREEN_WIDTH = 480;
int SCREEN_HEIGHT = 320;

// The window we'll be rendering to
SDL_Window* window = NULL;

// hw renderer
SDL_Renderer* renderer = NULL;

// Our game controller
SDL_Joystick* GameController = NULL;

// re render
bool invalidated = false;

char *ltrim(char *s) {
	while(isspace(*s)) s++;
	return s;
}

char *rtrim(char *s) {
	char* back = s + strlen(s);
	while(isspace(*--back));
	*(back+1) = '\0';
	return s;
}

char *trim(char *s) {
	return rtrim(ltrim(s));
}

SDL_Color colorConverter(unsigned long hexValue) {
	SDL_Color rgbColor;
	rgbColor.r = ((hexValue >> 16) & 0xFF);	// Extract the RR byte
	rgbColor.g = ((hexValue >> 8) & 0xFF);	// Extract the GG byte
	rgbColor.b = ((hexValue) & 0xFF);		// Extract the BB byte
	rgbColor.a = 255;						// Alpha
	return rgbColor;
}

/*
 * Write out text
 */
void initTextRect(char text[], int hexColorFont) {
	// font
	SDL_RWops *io = SDL_RWFromConstMem(DejaVuSans, 757076);
	TTF_Font* font = TTF_OpenFontRW(io, 1, 18);

	int w, h;
	TTF_SizeText(font, text, &w, &h);

	SDL_Rect pos;
	pos.x = (SCREEN_WIDTH / 2) - (w / 2);
	pos.y = (SCREEN_HEIGHT / 2) - (h / 2);
	pos.w = w;
	pos.h = h;

	char leftHalf[100], rightHalf[100];
    int length, mid, i, k;

    if (w > SCREEN_WIDTH) {
	 	/* Find length of string using strlen function */
	    length = strlen(text);

		mid = length/2;
	    /* Copy left half of text to leftHalf */
	    for (i = 0; i < mid; i++) {
	        leftHalf[i] = text[i];
	    }
	    leftHalf[i] = '\0';

		/* Copy right half of text to rightHalf  */
	    for (i = mid, k = 0; i <= length; i++, k++) {
	 		rightHalf[k] = text[i];
	    }

	    font = TTF_OpenFontRW(io, 1, 16);

	    SDL_Surface* textSurfaceLeftHalf = TTF_RenderUTF8_Blended(font, leftHalf, colorConverter(hexColorFont));
		SDL_Texture* blendedTextureLeftHalf = SDL_CreateTextureFromSurface(renderer, textSurfaceLeftHalf);
		SDL_FreeSurface(textSurfaceLeftHalf);

	    TTF_SizeText(font, leftHalf, &w, &h);

		pos.x = (SCREEN_WIDTH / 2) - (w / 2);
		pos.y = (SCREEN_HEIGHT / 2) - (h / 2) - 30;
		pos.w = w;
		pos.h = h;

		SDL_QueryTexture(blendedTextureLeftHalf, NULL, NULL, &pos.w, &pos.h); 
		SDL_RenderCopy(renderer, blendedTextureLeftHalf, NULL, &pos);

		// right
	    SDL_Surface* textSurfaceRightHalf = TTF_RenderUTF8_Blended(font, rightHalf, colorConverter(hexColorFont));
		SDL_Texture* blendedTextureRightHalf = SDL_CreateTextureFromSurface(renderer, textSurfaceRightHalf);
		SDL_FreeSurface(textSurfaceRightHalf);

	    TTF_SizeText(font, rightHalf, &w, &h);

		pos.x = (SCREEN_WIDTH / 2) - (w / 2);
		pos.y = (SCREEN_HEIGHT / 2) - (h / 2);
		pos.w = w;
		pos.h = h;

		SDL_QueryTexture(blendedTextureRightHalf, NULL, NULL, &pos.w, &pos.h); 
		SDL_RenderCopy(renderer, blendedTextureRightHalf, NULL, &pos);		
	}
	else {
		SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text, colorConverter(hexColorFont));

		SDL_Texture* blendedTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_FreeSurface(textSurface);

		SDL_QueryTexture(blendedTexture, NULL, NULL, &pos.w, &pos.h); 
		SDL_RenderCopy(renderer, blendedTexture, NULL, &pos);
	}

    TTF_CloseFont(font);
    font = NULL;
}

void initButtonRect(int hexColorRect, char text[], int hexColorFont) {
	SDL_RWops *io = SDL_RWFromConstMem(DejaVuSans, 757076);
	TTF_Font* font = TTF_OpenFontRW(io, 1, 24);
	
	int w, h;
	TTF_SizeText(font, text, &w, &h);

	SDL_Surface* backgroundSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

	SDL_Color rgb = colorConverter(hexColorRect);
	SDL_FillRect(backgroundSurface, NULL, SDL_MapRGBA(backgroundSurface->format, rgb.r, rgb.g, rgb.b, 255));
	SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);

	SDL_Rect pos;
	pos.x = (SCREEN_WIDTH / 2) - (w / 2);
	pos.y = (SCREEN_HEIGHT - h) - 30;
	pos.w = (w + 30);
	pos.h = (h + 20);

	SDL_RenderCopy(renderer, backgroundTexture, NULL, &pos);

	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text, colorConverter(hexColorFont));

    TTF_CloseFont(font);
    font = NULL;

	SDL_Texture* blendedTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);

	// center text inside box
	pos.x = (pos.x + 15);
	pos.y = (pos.y + 10);

	SDL_QueryTexture(blendedTexture, NULL, NULL, &pos.w, &pos.h); 
	SDL_RenderCopy(renderer, blendedTexture, NULL, &pos);
}

static void MainExit(void) {
	// Close controller
	SDL_JoystickClose(GameController);
	GameController = NULL;

	// Destroy renderer
	SDL_DestroyRenderer(renderer);
	renderer = NULL;

    // Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    // Quit SDL subsystems
    SDL_Quit();
}

bool init() {
	// initialize SDL and Controller
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);	
	window = SDL_CreateWindow("AnberPorts", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_OPENGL | SDL_RENDERER_ACCELERATED | SDL_WINDOW_FULLSCREEN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

	// get width / height
	SDL_GetRendererOutputSize(renderer, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	// vsync
	SDL_GL_SetSwapInterval(1);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	if (SDL_RenderFillRect(renderer, NULL) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not fill background color: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

    // controller
	if (SDL_NumJoysticks() < 1) {
		printf("Warning: No joysticks connected!\n" );
		return false;
	}

	GameController = SDL_JoystickOpen(0);

	if (GameController == NULL) {
		printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// font
	TTF_Init();

	return true;
}

int main(int argc, char* argv[]) {
	atexit(MainExit);

	unsigned int Frame_Starting_Time = 0, Elapsed_Time;
	SDL_Event event;

    // Start up SDL and create window
    if (!init()) {
        printf("Failed to initialize!\n");
        return EXIT_FAILURE;
    }

    // white background
	SDL_Rect pos;
	pos.x = 0;
	pos.y = 0;
	pos.w = SCREEN_WIDTH;
	pos.h = SCREEN_HEIGHT;

	SDL_Surface* backgroundSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	SDL_Color rgb = colorConverter(0xFFFFFF);
	SDL_FillRect(backgroundSurface, NULL, SDL_MapRGBA(backgroundSurface->format, rgb.r, rgb.g, rgb.b, 255));
	SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);

	SDL_RenderCopy(renderer, backgroundTexture, NULL, &pos);


    char desc[100];

	// command line arguments
	if (argc >= 2) {
		strcpy(desc, argv[1]);
		initTextRect(trim(desc), 0x000000);
	}

	char button[100];

	if (argc >= 3) {
		strcpy(button, argv[2]);
		initButtonRect(0x24A0ED, trim(button), 0xFFFFFF);
	}

	invalidated = true;

	bool quit = false;

	// Process incoming SDL events
	while (!quit) {
		// Keep the time corresponding to the frame rendering beginning
		Frame_Starting_Time = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			// buttons
			if (event.type == SDL_JOYBUTTONDOWN) {
				quit = true;
			}
		}

		if (invalidated) {
			// Render the changes above
			SDL_RenderPresent(renderer);
			invalidated = false;
		}

		// Wait enough time to get a 60Hz refresh rate
		Elapsed_Time = SDL_GetTicks() - Frame_Starting_Time;
		if (Elapsed_Time < 16) {
			SDL_Delay(16 - Elapsed_Time);
		}
	}

	return 0;
}