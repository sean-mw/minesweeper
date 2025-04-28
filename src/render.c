#include "render.h"

static SDL_Texture *find_texture(Texture **textures, int n_textures,
                                 const char *name) {
  // TODO: We are doing an O(n) lookup every time we need a texture.
  // 	   This is very bad. This is just meant as a temporary solution.
  for (int i = 0; i < n_textures; i++) {
    if (SDL_strcmp(textures[i]->file_name, name) == 0)
      return textures[i]->texture;
  }
  return NULL;
}

void Render_Cell(SDL_Renderer *renderer, Texture **textures, int n_textures,
                 Cell *cell) {
  SDL_FRect rect;
  SDL_Texture *texture;
  const char *file_name;
  Uint16 x = cell->x * CELL_SIZE_IN_PIXELS;
  Uint16 y = cell->y * CELL_SIZE_IN_PIXELS;

  switch (cell->state) {
  case HIDDEN:
    file_name = "hidden.bmp";
    break;
  case FLAGGED:
    file_name = "flagged.bmp";
    break;
  case CLEARED:
    if (cell->is_mine) {
      file_name = "mine.bmp";
    } else {
      char count_name[11];
      SDL_snprintf(count_name, sizeof(count_name), "count%d.bmp",
                   cell->mine_count);
      file_name = count_name;
    }
    break;
  }

  rect.x = x, rect.y = y;
  rect.w = rect.h = CELL_SIZE_IN_PIXELS;
  texture = find_texture(textures, n_textures, file_name);
  SDL_RenderTexture(renderer, texture, NULL, &rect);
}

void Render_All(SDL_Renderer *renderer, Texture **textures, int n_textures,
                Cell cells[GAME_HEIGHT][GAME_WIDTH]) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  for (int y = 0; y < GAME_HEIGHT; y++) {
    for (int x = 0; x < GAME_WIDTH; x++) {
      Render_Cell(renderer, textures, n_textures, &cells[y][x]);
    }
  }
}
