#ifndef SDL_BACKEND_H
#define SDL_BACKEND_H

#include "callbacks.h"

void SDLBackendInit(int argc, char** argv);

bool SDLBackendCreateWindow(unsigned int Width, unsigned int Height, unsigned int bpp, bool isFullScreen, const char* pTitle, SDL_Window** Window, SDL_GLContext Context);

void SDLBackendRun(ICallbacks* pCallbacks, SDL_Window* Window);

void SDLLoadClass(ICallbacks* pCallbacks);

#endif