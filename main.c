#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define CELL_SIZE_IN_PIXELS 32
#define GAME_WIDTH 24U
#define GAME_HEIGHT 18U
#define SDL_WINDOW_WIDTH (CELL_SIZE_IN_PIXELS * GAME_WIDTH)
#define SDL_WINDOW_HEIGHT (CELL_SIZE_IN_PIXELS * GAME_HEIGHT)

enum CellState {
  HIDDEN,
  FLAGGED,
  CLEARED,
};

typedef struct {
  enum CellState state;
  bool isMine;
  Uint8 mineCount;
  Uint8 x, y;
} Cell;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  Cell cells[GAME_HEIGHT][GAME_WIDTH];
} AppState;

SDL_AppResult handle_mouse_button_event(AppState *as,
                                        SDL_MouseButtonEvent event) {
  Uint8 x = (int)event.x / CELL_SIZE_IN_PIXELS;
  Uint8 y = (int)event.y / CELL_SIZE_IN_PIXELS;
  Cell *cell = &as->cells[y][x];
  switch (event.button) {
  case SDL_BUTTON_LEFT:
    cell->state = CLEARED;
    break;
  case SDL_BUTTON_RIGHT:
    cell->state = FLAGGED;
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

void render_cell(SDL_Renderer *renderer, Cell *cell) {
  SDL_FRect rect;
  Uint8 r = 0, g = 0, b = 0;
  Uint16 x = cell->x * CELL_SIZE_IN_PIXELS;
  Uint16 y = cell->y * CELL_SIZE_IN_PIXELS;

  switch (cell->state) {
  case HIDDEN:
    b = 255;
    break;
  case FLAGGED:
    r = 255;
    break;
  case CLEARED:
    g = 255;
    break;
  }

  rect.x = x, rect.y = y;
  rect.w = rect.h = CELL_SIZE_IN_PIXELS;
  SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &rect);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = (AppState *)appstate;

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  for (unsigned row = 0; row < GAME_HEIGHT; row++) {
    for (unsigned col = 0; col < GAME_WIDTH; col++) {
      render_cell(as->renderer, &as->cells[row][col]);
    }
  }

  SDL_RenderPresent(as->renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  srand(time(NULL));

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

  for (unsigned row = 0; row < GAME_HEIGHT; row++) {
    for (unsigned col = 0; col < GAME_WIDTH; col++) {
      Cell *cell = &as->cells[row][col];
      cell->isMine = rand() & 1;
      cell->state = HIDDEN;
      cell->x = col;
      cell->y = row;
    }
  }

  for (unsigned row = 0; row < GAME_HEIGHT; row++) {
    for (unsigned col = 0; col < GAME_WIDTH; col++) {
      Uint8 count = 0;
      for (unsigned r = row - 1; r <= row + 1; r++) {
        for (unsigned c = col - 1; c <= col + 1; c++) {
          if (r < 0 || r >= GAME_HEIGHT || c < 0 || c >= GAME_WIDTH) {
            continue;
          }
          count += as->cells[r][c].isMine;
        }
      }
      as->cells[row][col].mineCount = count;
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
    SDL_free(as);
  }
}
