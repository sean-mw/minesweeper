#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define CELL_SIZE_IN_PIXELS 64
#define GAME_WIDTH 24U
#define GAME_HEIGHT 18U
#define SDL_WINDOW_WIDTH (CELL_SIZE_IN_PIXELS * GAME_WIDTH)
#define SDL_WINDOW_HEIGHT (CELL_SIZE_IN_PIXELS * GAME_HEIGHT)
#define MINE_SPAWN_RATE 0.2

enum CellState {
  HIDDEN,
  FLAGGED,
  CLEARED,
};

typedef struct {
  enum CellState state;
  bool is_mine;
  Uint8 mine_count;
  Uint8 x, y;
} Cell;

typedef struct {
  const char *file_name;
  SDL_Texture *texture;
} Texture;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  Cell cells[GAME_HEIGHT][GAME_WIDTH];
  Texture **textures;
  int n_textures;
} AppState;

Texture *load_texture(AppState *as, const char *file_name) {
  SDL_Surface *surface;
  char *bmp_path;
  SDL_Texture *sdl_texture;
  Texture *texture = (Texture *)SDL_malloc(sizeof(Texture));

  SDL_asprintf(&bmp_path, "%s/assets/%s", SDL_GetBasePath(), file_name);

  surface = SDL_LoadBMP(bmp_path);
  if (!surface) {
    SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_free(bmp_path);
    return NULL;
  }

  SDL_free(bmp_path);

  sdl_texture = SDL_CreateTextureFromSurface(as->renderer, surface);
  if (!sdl_texture) {
    SDL_Log("Couldn't create static texture: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(sdl_texture);
    return NULL;
  }

  SDL_DestroySurface(surface);

  texture->file_name = file_name;
  texture->texture = sdl_texture;

  return texture;
}

bool out_of_bounds(Uint8 x, Uint8 y) {
  return y < 0 || y >= GAME_HEIGHT || x < 0 || x >= GAME_WIDTH;
}

void clear_zeroes(AppState *as, Cell *cell) {
  bool seen[GAME_HEIGHT][GAME_WIDTH];
  for (size_t y = 0; y < GAME_HEIGHT; y++) {
    for (size_t x = 0; x < GAME_WIDTH; x++) {
      seen[y][x] = false;
    }
  }

  Cell *queue[GAME_HEIGHT * GAME_WIDTH];
  queue[0] = cell;
  seen[cell->y][cell->x] = true;
  Uint16 q_head = 0, q_tail = 1;

  while (q_head < q_tail) {
    Cell *head = queue[q_head++];
    head->state = CLEARED;

    if (head->mine_count != 0) {
      continue;
    }

    for (int ny = head->y - 1; ny <= head->y + 1; ny++) {
      for (int nx = head->x - 1; nx <= head->x + 1; nx++) {
        if (out_of_bounds(nx, ny)) {
          continue;
        }
        Cell *neighbor = &as->cells[ny][nx];
        if (!seen[ny][nx] && !neighbor->is_mine) {
          queue[q_tail++] = neighbor;
        }
        seen[ny][nx] = true;
      }
    }
  }
}

SDL_AppResult handle_mouse_button_event(AppState *as,
                                        SDL_MouseButtonEvent event) {
  Uint8 x = (int)event.x / CELL_SIZE_IN_PIXELS;
  Uint8 y = (int)event.y / CELL_SIZE_IN_PIXELS;
  Cell *cell = &as->cells[y][x];
  switch (event.button) {
  case SDL_BUTTON_LEFT:
    if (!cell->is_mine && cell->mine_count == 0) {
      clear_zeroes(as, cell);
    } else {
      cell->state = CLEARED;
    }
    break;
  case SDL_BUTTON_RIGHT:
    if (cell->state == HIDDEN) {
      cell->state = FLAGGED;
    } else if (cell->state == FLAGGED) {
      cell->state = HIDDEN;
    }
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_Texture *find_texture(AppState *as, const char *file_name) {
  // TODO: We are doing an O(n) lookup everytime we need a texture.
  // 	   This is very bad. This is just meant as a temporary solution.
  for (size_t i = 0; i < as->n_textures; i++) {
    if (SDL_strcmp(as->textures[i]->file_name, file_name) == 0) {
      return as->textures[i]->texture;
    }
  }
  SDL_Log("Couldn't find texture: %s", file_name);
  return NULL;
}

void render_cell(AppState *as, Cell *cell) {
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
  texture = find_texture(as, file_name);
  SDL_RenderTexture(as->renderer, texture, NULL, &rect);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = (AppState *)appstate;

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  for (size_t row = 0; row < GAME_HEIGHT; row++) {
    for (size_t col = 0; col < GAME_WIDTH; col++) {
      render_cell(as, &as->cells[row][col]);
    }
  }

  SDL_RenderPresent(as->renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_srand(0);

  if (!SDL_SetAppMetadata("Minesweeper", "1.0",
                          "com.github.sean-mw.Minesweeper")) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppState *as = (AppState *)SDL_malloc(sizeof(AppState));
  if (!as) {
    return SDL_APP_FAILURE;
  }
  *appstate = as;

  for (size_t row = 0; row < GAME_HEIGHT; row++) {
    for (size_t col = 0; col < GAME_WIDTH; col++) {
      Cell *cell = &as->cells[row][col];
      cell->is_mine = SDL_randf() < MINE_SPAWN_RATE;
      cell->state = HIDDEN;
      cell->x = col;
      cell->y = row;
    }
  }

  for (int y = 0; y < GAME_HEIGHT; y++) {
    for (int x = 0; x < GAME_WIDTH; x++) {
      Uint8 count = 0;
      for (int ny = y - 1; ny <= y + 1; ny++) {
        for (int nx = x - 1; nx <= x + 1; nx++) {
          if (out_of_bounds(nx, ny)) {
            continue;
          }
          count += as->cells[ny][nx].is_mine;
        }
      }
      as->cells[y][x].mine_count = count;
    }
  }

  if (!SDL_CreateWindowAndRenderer("Minesweeper", SDL_WINDOW_WIDTH,
                                   SDL_WINDOW_HEIGHT, 0, &as->window,
                                   &as->renderer)) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_SetRenderVSync(as->renderer, 1)) {
    return SDL_APP_FAILURE;
  }

  char *path;
  SDL_asprintf(&path, "%s/assets/", SDL_GetBasePath());
  char **files = SDL_GlobDirectory(path, NULL, 0, &as->n_textures);
  SDL_free(path);
  as->textures = (Texture **)SDL_calloc(as->n_textures, sizeof(Texture *));
  for (size_t i = 0; i < as->n_textures; i++) {
    Texture *texture = load_texture(as, files[i]);
    if (!texture) {
      return SDL_APP_FAILURE;
    }
    as->textures[i] = texture;
  }
  SDL_free(files);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *as = (AppState *)appstate;
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    return handle_mouse_button_event(as, event->button);
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate != NULL) {
    AppState *as = (AppState *)appstate;
    SDL_DestroyRenderer(as->renderer);
    SDL_DestroyWindow(as->window);
    for (size_t i = 0; i < as->n_textures; i++) {
      SDL_DestroyTexture(as->textures[i]->texture);
      SDL_free(as->textures[i]);
    }
    SDL_free(as->textures);
    SDL_free(as);
  }
}
