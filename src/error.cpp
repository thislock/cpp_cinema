
#include "error.h"

#include <cstdlib>
#include <iostream>

#include <SDL3/SDL.h>

void showErrorPopup(const std::string& title, const std::string& message) {
#if defined(_WIN32)
  MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
#elif defined(__APPLE__)
  std::string command = "osascript -e 'display dialog \"" + message + "\" with title \"" + title + "\" buttons {\"OK\"} with icon stop'";
  system(command.c_str());
#elif defined(__linux__)
  std::string command = "zenity --error --title=\"" + title + "\" --text=\"" + message + "\"";
  system(command.c_str());
#else
  std::cerr << title << ": " << message << std::endl;
#endif
}

void showErrorPopup(const std::string& message) {
  const std::string title = "Error";
  showErrorPopup(title, message);
}

int showSdlError(const std::string & error_type) {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, error_type.c_str(), SDL_GetError(), nullptr);
  SDL_Quit();
  return -1;
}