#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include "error.h"

const char * WindowTitle = "test";

const SDL_InitFlags SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

int main(int argc, char* argv[]) {
  
  if (SDL_Init( SDL_INIT_FLAGS ) == false) {  
    auto error = SDL_GetError();
    showErrorPopup(error);
    return 1;
  }
  
  SDL_Window * window = SDL_CreateWindow(WindowTitle, 800, 600, 0);
  if (window == nullptr) {
    auto error = SDL_GetError();
    showErrorPopup(error);
    SDL_Quit();
    return 1;
  }

  SDL_Renderer * renderer = SDL_CreateRenderer(window, nullptr);
  if (renderer == nullptr) {
    return showSdlError("renderer error");
  }

  SDL_Surface * screen = SDL_CreateSurface(100, 100, SDL_PIXELFORMAT_ARGB32);

  SDL_Event event;

  bool isRunning = true;
  while (isRunning) {

    const int event_limit = 100;
    int events_read = 0;
    while (SDL_PollEvent(&event) == true && ++events_read < event_limit) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          isRunning = false;
          break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(20);
  }

  SDL_DestroySurface(screen);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
