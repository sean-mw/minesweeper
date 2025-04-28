#include "app.h"
#include "input.h"
#include "render.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = (AppState *)appstate;

  SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(as->renderer);

  Render_All(as->renderer, as->textures, as->n_textures, as->cells);

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

  Game_Init(as->cells);

  if (!SDL_CreateWindowAndRenderer("Minesweeper", SDL_WINDOW_WIDTH,
                                   SDL_WINDOW_HEIGHT, 0, &as->window,
                                   &as->renderer)) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_SetRenderVSync(as->renderer, 1)) {
    return SDL_APP_FAILURE;
  }

  if (!Assets_LoadAll(as->renderer, &as->textures, &as->n_textures)) {
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
    return Handle_MouseDown(as, event->button);
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate == NULL) {
    return;
  }
  AppState *as = (AppState *)appstate;
  Assets_DestroyAll(as->textures, as->n_textures);
  SDL_DestroyRenderer(as->renderer);
  SDL_DestroyWindow(as->window);
  SDL_free(as);
}
