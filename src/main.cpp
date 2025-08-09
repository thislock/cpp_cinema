
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include "error.h"
#include "video.h"

const char * WindowTitle = "test";

const SDL_InitFlags SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <video-file>\n";
    return 1;
  }

  const char* filename_cstr = argv[1];

  // Open the input file
  AVFormatContext* fmt_ctx = nullptr;
  if (avformat_open_input(&fmt_ctx, filename_cstr, nullptr, nullptr) != 0) {
    std::cerr << "Could not open file: " << filename_cstr << "\n";
    return 1;
  }

  // Retrieve stream information
  if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
    std::cerr << "Could not find stream information\n";
    avformat_close_input(&fmt_ctx);
    return 1;
  }

  std::string filename(filename_cstr);
  Video video = Video(filename);
  
  if (SDL_Init( SDL_INIT_FLAGS ) == false) {  
    auto error = SDL_GetError();
    showErrorPopup(error);
    return 1;
  }
  
  SDL_Window * window = SDL_CreateWindow(WindowTitle, 800, 600, SDL_WINDOW_RESIZABLE);
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

  SDL_Surface * screen = SDL_CreateSurface(500, 500, SDL_PIXELFORMAT_ARGB32);

  SDL_Event event;

  int video_frames = 0;

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

    if (video_frames >= video.video_frames.size())
      video_frames = 0;
    auto tex = SDL_CreateTextureFromSurface(renderer, video.video_frames[video_frames++]);
    SDL_RenderTexture(renderer, tex, NULL, NULL);
    SDL_DestroyTexture(tex);

    SDL_RenderPresent(renderer);
    SDL_Delay(20);
  }

  SDL_DestroySurface(screen);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
