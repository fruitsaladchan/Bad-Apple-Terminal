# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
STB_INCLUDE = -I/usr/include/stb/
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = -lSDL2

# Targets
FRAME_EXTRACT = frame-extract
BADAPPLE = badapple

# Source files
FRAME_EXTRACT_SRC = frame-extract.c
BADAPPLE_SRC = badapple.c

# only compile frame_extractor
$(FRAME_EXTRACT): $(FRAME_EXTRACT_SRC)
	$(CC) $(CFLAGS) $(STB_INCLUDE) -o $@ $< -lm

# only compile bad apple player
$(BADAPPLE): $(BADAPPLE_SRC)
	$(CC) $(SDL_CFLAGS) -o $@ $< $(SDL_LIBS)

# Default target
all: $(FRAME_EXTRACT) $(BADAPPLE)
	./$(FRAME_EXTRACT)

# run after compiling
run: $(FRAME_EXTRACT) $(BADAPPLE)
	./$(FRAME_EXTRACT)
	./$(BADAPPLE)

# Extract frames and audio
extract: $(FRAME_EXTRACT)
	./$(FRAME_EXTRACT)

# clean build artifacts
clean:
	rm -f $(FRAME_EXTRACT) $(BADAPPLE)

# clean media files
clean-media:
	rm -f audio.wav
	rm -rf frames/

# clean everything
clean-all: clean clean-media

# install dependencies (Arch)
install-deps:
	sudo pacman -S sdl2 ffmpeg stb

.PHONY: all extract play run clean clean-media clean-all install-deps
