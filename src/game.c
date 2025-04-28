#include "game.h"

static bool out_of_bounds(Uint8 x, Uint8 y) {
  return y < 0 || y >= GAME_HEIGHT || x < 0 || x >= GAME_WIDTH;
}

void Game_Init(Cell cells[GAME_HEIGHT][GAME_WIDTH]) {
  for (int y = 0; y < GAME_HEIGHT; y++) {
    for (int x = 0; x < GAME_WIDTH; x++) {
      cells[y][x].is_mine = SDL_randf() < MINE_SPAWN_RATE;
      cells[y][x].state = HIDDEN;
      cells[y][x].x = x;
      cells[y][x].y = y;
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
          count += cells[ny][nx].is_mine;
        }
      }
      cells[y][x].mine_count = count;
    }
  }
}

void Game_ClearZeros(Cell cells[GAME_HEIGHT][GAME_WIDTH], int x, int y) {
  bool seen[GAME_HEIGHT][GAME_WIDTH];
  for (int sy = 0; sy < GAME_HEIGHT; sy++) {
    for (int sx = 0; sx < GAME_WIDTH; sx++) {
      seen[sy][sx] = false;
    }
  }

  Cell *queue[GAME_HEIGHT * GAME_WIDTH];
  queue[0] = &cells[y][x];
  seen[y][x] = true;
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
        Cell *neighbor = &cells[ny][nx];
        if (!seen[ny][nx] && !neighbor->is_mine) {
          queue[q_tail++] = neighbor;
        }
        seen[ny][nx] = true;
      }
    }
  }
}
