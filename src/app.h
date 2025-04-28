#ifndef APP_H
#define APP_H

#include "assets.h"
#include "config.h"
#include "game.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  Cell cells[GAME_HEIGHT][GAME_WIDTH];
  Texture **textures;
  int n_textures;
} AppState;

#endif
