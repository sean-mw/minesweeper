#include "assets.h"

static Texture *load_texture(SDL_Renderer *renderer, const char *file_name) {
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

  sdl_texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!sdl_texture) {
    SDL_Log("Couldn't create static texture: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(sdl_texture);
    return NULL;
  }

  SDL_DestroySurface(surface);

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
