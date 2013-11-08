
void GeoMap_drawPoint (SDL_Renderer *renderer, float pos_lat, float pos_lon, float lat, float lon, int zoom);
void GeoMap_drawMap (SDL_Renderer *renderer, float lat, float lon, int zoom);
void GeoMap_getPoint (SDL_Renderer *renderer, int x, int y, float *mouse_lon, float *mouse_lat, float center_lat, float center_lon, int zoom);
