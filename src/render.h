#ifndef RENDER_H
#define RENDER_H

#include "assets.h"
#include "config.h"
#include "game.h"
#include <SDL3/SDL.h>

void Render_Cell(SDL_Renderer *renderer, Texture **textures, int n_textures,
                 Cell *cell);

void Render_All(SDL_Renderer *renderer, Texture **textures, int n_textures,
                Cell cells[GAME_HEIGHT][GAME_WIDTH]);

#endif
