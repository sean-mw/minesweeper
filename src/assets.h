#ifndef ASSETS_H
#define ASSETS_H

#include "config.h"
#include <SDL3/SDL.h>

typedef struct {
  const char *file_name;
  SDL_Texture *texture;
} Texture;

bool Assets_LoadAll(SDL_Renderer *renderer, Texture ***out_textures,
                    int *out_count);

void Assets_DestroyAll(Texture **textures, int count);

#endif
