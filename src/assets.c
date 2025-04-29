#include "assets.h"
#include <SDL3_image/SDL_image.h>

static Texture *load_texture(SDL_Renderer *renderer, const char *file_name) {
  SDL_Surface *surface;
  char *path;
  SDL_Texture *sdl_texture;
  Texture *texture = (Texture *)SDL_malloc(sizeof(Texture));

  SDL_asprintf(&path, "%s/assets/%s", SDL_GetBasePath(), file_name);

  sdl_texture = IMG_LoadTexture(renderer, path);
  SDL_free(path);
  if (!sdl_texture) {
    SDL_Log("Couldn't load texture: %s", SDL_GetError());
    return NULL;
  }

  texture->file_name = SDL_strdup(file_name);
  texture->texture = sdl_texture;

  return texture;
}

bool Assets_LoadAll(SDL_Renderer *renderer, Texture ***out_textures,
                    int *out_count) {
  char *path;
  SDL_asprintf(&path, "%s/assets/", SDL_GetBasePath());
  char **files = SDL_GlobDirectory(path, NULL, 0, out_count);
  SDL_free(path);

  *out_textures = (Texture **)SDL_calloc(*out_count, sizeof(Texture *));
  for (int i = 0; i < *out_count; i++) {
    Texture *texture = load_texture(renderer, files[i]);
    if (!texture) {
      SDL_free(files);
      return false;
    }
    (*out_textures)[i] = texture;
  }

  SDL_free(files);

  return true;
}

void Assets_DestroyAll(Texture **textures, int count) {
  for (int i = 0; i < count; i++) {
    SDL_DestroyTexture(textures[i]->texture);
    SDL_free((void *)textures[i]->file_name);
    SDL_free(textures[i]);
  }
  SDL_free(textures);
}
