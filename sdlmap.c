
#include <stdlib.h>
#include <stdio.h>
#include <SDL_test_common.h>
#include <map.h>
#include <text.h>
#include <http.h>

static SDLTest_CommonState *state;
static SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;

int main(int argc, char *argv[]) {
	int zoom = 16;
	float center_lat = 50.29;
	float center_lon = 9.12;
	float pos_lat = center_lat;
	float pos_lon = center_lon;
	float mouse_lat = center_lat;
	float mouse_lon = center_lon;
	int mouse_x = 0;
	int mouse_y = 0;
	int i, done;
	SDL_Event event;
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	state = SDLTest_CommonCreateState(argv, SDL_INIT_VIDEO);
	if (!state) {
		return 1;
	}
	if (!SDLTest_CommonInit(state)) {
		return 2;
	}
	for (i = 0; i < state->num_windows; ++i) {
		SDL_Renderer *renderer = state->renderers[i];
		SDL_SetRenderDrawBlendMode(renderer, blendMode);
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
	}
	done = 0;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			SDLTest_CommonEvent(state, &event, &done);
			switch (event.type) {
				case SDL_MOUSEMOTION:
					mouse_x = event.motion.x;
					mouse_y = event.motion.y;
					GeoMap_getPoint(state->renderers[0], mouse_x, mouse_y, &mouse_lon, &mouse_lat, center_lat, center_lon, zoom);
				break;
				case SDL_MOUSEBUTTONDOWN:
					mouse_x = event.button.x;
					mouse_y = event.button.y;
					if (event.button.button == 3) {
						GeoMap_getPoint(state->renderers[0], mouse_x, mouse_y, &center_lon, &center_lat, center_lat, center_lon, zoom);
					} else {
						GeoMap_getPoint(state->renderers[0], mouse_x, mouse_y, &pos_lon, &pos_lat, center_lat, center_lon, zoom);
					}
				break;
				case SDL_MOUSEWHEEL:
					if (event.button.x > 0 && zoom < 18) {
						zoom++;
					} else if (event.button.x < 0 && zoom > 9) {
						zoom--;
					}
				break;
				case SDL_QUIT:
					done = 1;
				break;
			}
		}
		for (i = 0; i < state->num_windows; ++i) {
			SDL_Renderer *renderer = state->renderers[i];
			SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
			SDL_RenderClear(renderer);

			GeoMap_drawMap(renderer, center_lat, center_lon, zoom);
			GeoMap_drawPoint(renderer, pos_lat, pos_lon, center_lat, center_lon, zoom);

			char text[1024];
			sprintf(text, "%f, %f", mouse_lat, mouse_lon);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			draw_text(renderer, mouse_x + 1, mouse_y + 1, text);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			draw_text(renderer, mouse_x, mouse_y, text);

			SDL_RenderPresent(renderer);
		}
	}
	SDLTest_CommonQuit(state);
	return 0;
}





