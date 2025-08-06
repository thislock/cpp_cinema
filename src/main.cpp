#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include "error.h"

const char * WindowTitle = "test";

int main(int, char**) {
  
  if (SDL_Init( SDL_INIT_VIDEO ) == false) {  
    auto error = SDL_GetError();
    showErrorPopup(error);
    return 1;
  }
  
  SDL_Window * window = nullptr;
  window = SDL_CreateWindow(WindowTitle, 800, 600, 0);
  if (window == nullptr) {
    auto error = SDL_GetError();
    showErrorPopup(error);
    SDL_Quit();
    return 1;
  }

  SDL_Surface * screen = SDL_CreateSurface(100, 100, SDL_PIXELFORMAT_ARGB32);

  SDL_Event event;

  bool isRunning = true;
  while (isRunning) {

    while (SDL_PollEvent(&event) == true) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          isRunning = false;
          break;
      }
    }

    SDL_FillSurfaceRect( screen, nullptr, SDL_MapSurfaceRGB( screen, 0xFF, 0xFF, 0xFF ) );
          
    //Render image on screen
    SDL_BlitSurface( nullptr, nullptr, screen, nullptr );
    //Update the surface
    SDL_UpdateWindowSurface( window );

  }

  SDL_DestroySurface(screen);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
