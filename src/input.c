#include "input.h"

SDL_AppResult Handle_MouseDown(AppState *as, SDL_MouseButtonEvent event) {
  int x = event.x / CELL_SIZE_IN_PIXELS;
  int y = event.y / CELL_SIZE_IN_PIXELS;
  Cell *cell = &as->cells[y][x];

  switch (event.button) {
  case SDL_BUTTON_LEFT:
    if (!cell->is_mine && cell->mine_count == 0)
      Game_ClearZeros(as->cells, x, y);
    else
      cell->state = CLEARED;
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
