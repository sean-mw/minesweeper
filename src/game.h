#ifndef GAME_H
#define GAME_H

#include "config.h"
#include <SDL3/SDL.h>

typedef enum { HIDDEN, FLAGGED, CLEARED } CellState;
typedef struct {
  CellState state;
  bool is_mine;
  Uint8 mine_count;
  Uint8 x, y;
} Cell;

void Game_Init(Cell cells[GAME_HEIGHT][GAME_WIDTH]);

void Game_ClearZeros(Cell cells[GAME_HEIGHT][GAME_WIDTH], int x, int y);

#endif
