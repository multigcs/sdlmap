
#include <stdlib.h>
#include <stdio.h>
#include <SDL_test_common.h>
#include <SDL_image.h>

static int long2tilex(float lon, int z) {
	return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z))); 
}

static int lat2tiley(float lat, int z) {
	return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z))); 
}

static float tilex2long(int x, int z) {
	return x / pow(2.0, z) * 360.0 - 180;
}

static float tiley2lat(int y, int z) {
	float n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

static float y2lat (int y, float lat, float zoom) {
	int tile_ny = y / 256;
	int tile_y = lat2tiley(lat, zoom);
	float t_lat = tiley2lat(tile_y + tile_ny, zoom);
	float t_lat2 = tiley2lat(tile_y + tile_ny + 1, zoom);
	float t_lat_div = t_lat2 - t_lat;
	int tpos_y = y - tile_ny * 256;
	float mouse_lat = t_lat + t_lat_div * (float)tpos_y / 256.0;
	return mouse_lat;
}

static float x2long (int x, float lon, float zoom) {
	int tile_nx = x / 256;
	int tile_x = long2tilex(lon, zoom);
	float t_long = tilex2long(tile_x + tile_nx, zoom);
	float t_long2 = tilex2long(tile_x + tile_nx + 1, zoom);
	float t_long_div = t_long2 - t_long;
	int tpos_x = x - tile_nx * 256;
	float mouse_long = t_long + t_long_div * (float)tpos_x / 256.0;
	return mouse_long;
}

static int lat2y (float mark_lat, float lat, float zoom) {
	int tile_y = lat2tiley(lat, zoom);
	int mark_tile_y = lat2tiley(mark_lat, zoom) - tile_y;
	float m_lat = tiley2lat(tile_y + mark_tile_y, zoom);
	float m_lat2 = tiley2lat(tile_y + mark_tile_y + 1, zoom);
	float m_lat_div = m_lat2 - m_lat;
	float m_lat_div2 = mark_lat - m_lat;
	float mark_tile_y_div = 256.0 * m_lat_div2 / m_lat_div;
	float mark_y = mark_tile_y * 256.0 + mark_tile_y_div;
	return (int)mark_y;
}

static int long2x (float mark_long, float lon, float zoom) {
	int tile_x = long2tilex(lon, zoom);
	int mark_tile_x = long2tilex(mark_long, zoom) - tile_x;
	float m_long = tilex2long(tile_x + mark_tile_x, zoom);
	float m_long2 = tilex2long(tile_x + mark_tile_x + 1, zoom);
	float m_long_div = m_long2 - m_long;
	float m_long_div2 = mark_long - m_long;
	float mark_tile_x_div = 256.0 * m_long_div2 / m_long_div;
	float mark_x = (float)mark_tile_x * 256.0 + mark_tile_x_div;
	return (int)mark_x;
}

static void DrawTile (SDL_Renderer *renderer, int x, int y, char *tile) {
	SDL_Rect rect;
	SDL_Texture *texture;
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);
	texture = IMG_LoadTexture(renderer, tile);
	rect.x = x;
	rect.y = y;
	rect.w = 256;
	rect.h = 256;
	SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
	SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
	SDL_DestroyTexture(texture);
}

void GeoMap_drawPoint (SDL_Renderer *renderer, float pos_lat, float pos_lon, float lat, float lon, int zoom) {
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);
	uint8_t tiles_x = (viewport.w + 255) / 256;
	uint8_t tiles_y = (viewport.h + 255) / 256;

	int pos_x = long2x(pos_lon, lon, zoom) + (tiles_x / 2 * 256);
	int pos_y = lat2y(pos_lat, lat, zoom) + (tiles_y / 2 * 256);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawLine(renderer, pos_x - 10, pos_y, pos_x + 10, pos_y);
	SDL_RenderDrawLine(renderer, pos_x, pos_y - 10, pos_x, pos_y + 10);
}

void GeoMap_drawMap (SDL_Renderer *renderer, float lat, float lon, int zoom) {
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);
	int tile_x = long2tilex(lon, zoom);
	int tile_y = lat2tiley(lat, zoom);
	int x_n = 0;
	int y_n = 0;
	char tile_name[1024];
	char tile_url[1024];
	uint8_t tiles_x = (viewport.w + 255) / 256;
	uint8_t tiles_y = (viewport.h + 255) / 256;
	for (y_n = 0; y_n < tiles_y; y_n++) {
		for (x_n = 0; x_n < tiles_x; x_n++) {
			sprintf(tile_name, "MAPS/osm_%i_%i_%i.png", zoom, tile_x + x_n - (tiles_x / 2), tile_y + y_n - (tiles_y / 2));
			if (file_exists(tile_name) == 0) {
#ifdef WINDOWS
				mkdir("MAPS");
#else
				mkdir("MAPS", 0755);
#endif
				sprintf(tile_url, "http://tile.openstreetmap.org/%i/%i/%i.png", zoom, tile_x + x_n - (tiles_x / 2), tile_y + y_n - (tiles_y / 2));
				htmlget(tile_url, tile_name);
			}
			DrawTile(renderer, x_n * 256, y_n * 256, tile_name);
		}
	}
}

void GeoMap_getPoint (SDL_Renderer *renderer, int x, int y, float *lon, float *lat, float center_lat, float center_lon, int zoom) {
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);
	uint8_t tiles_x = (viewport.w + 255) / 256;
	uint8_t tiles_y = (viewport.h + 255) / 256;
	*lon = x2long(x - (tiles_x / 2 * 256), center_lon, zoom);
	*lat = y2lat(y - (tiles_y / 2 * 256), center_lat, zoom);
}

