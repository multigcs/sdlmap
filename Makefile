

all:
	gcc -o sdlmap sdlmap.c map.c text.c http.c SDL_test_common.c \
		-I./ \
		$(shell pkg-config --cflags sdl2) $(shell pkg-config --cflags SDL2_image) $(shell pkg-config --cflags SDL_net) \
		$(shell pkg-config --libs sdl2) $(shell pkg-config --libs SDL2_image) $(shell pkg-config --libs SDL_net)

clean:
	rm -rf sdlmap

dist-clean:
	rm -rf sdlmap MAPS

