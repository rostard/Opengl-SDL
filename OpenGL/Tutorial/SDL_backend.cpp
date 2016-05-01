#include <stdio.h>
#include <gl\glew.h>
#include <SDL\SDL.h>

#include "SDL_backend.h"

static ICallbacks* s_pCallbacks = NULL;

static void KeyboardDownCB(int Key)
{
	s_pCallbacks->KeyboardDownCB(Key);
}

static void KeyboardUpCB(int Key)
{
	s_pCallbacks->KeyboardUpCB(Key);
}

static void PassiveMouseCB(int x, int y)
{
	s_pCallbacks->PassiveMouseCB(x, y);
}

static void MouseCB(int Button, int State, int x, int y)
{
	s_pCallbacks->MouseCB(Button, State, x, y);
}

static void RenderSceneCB()
{
	s_pCallbacks->RenderSceneCB();
}

static void InitCallbacks(SDL_Window* Window)
{
	bool run = true;
	int x = 0, y = 0;
	SDL_Event e;

	while (run)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
				//User requests quit
			case SDL_QUIT:
				run = false;
				break;

			case SDL_KEYDOWN:
				
				KeyboardDownCB(e.key.keysym.sym);
				if (e.key.keysym.sym == 27)run = false;
				break;
			case SDL_KEYUP:
				KeyboardUpCB(e.key.keysym.sym);
				break;
/*
			case SDL_TEXTINPUT:


				//handleKeys(e.text.text[0], x, y);
				break;*/
			case SDL_MOUSEMOTION:
				
				SDL_GetMouseState(&x, &y);
				PassiveMouseCB(x, y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				//int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				
				MouseCB(e.button.button,e.button.state,x, y);
				break;
			}
		}
		RenderSceneCB();

		SDL_GL_SwapWindow(Window);
	}
}

void SDLLoadClass(ICallbacks* pCallbacks)
{
	s_pCallbacks = pCallbacks;
}

void SDLBackendInit(int argc, char** argv)
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		exit(0);
	}
	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

}

bool SDLBackendCreateWindow(unsigned int Width, unsigned int Height, unsigned int bpp, bool isFullScreen, const char* pTitle,SDL_Window** Window, SDL_GLContext Context)
{
	if (isFullScreen)
	{
		//Create window
		*Window = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
		if (*Window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			return false;
		}
	}
	else
	{
		//Create window
		*Window = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (*Window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			return false;
		}
	}

	//Create context
	Context = SDL_GL_CreateContext(*Window);
	if (Context == NULL)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(res));
		return false;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	return true;
}

void SDLBackendRun(ICallbacks* pCallbacks, SDL_Window* Window){
	if (!pCallbacks){
		fprintf(stderr, "%s : callbacks not specified!\n", __FUNCTION__);
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	s_pCallbacks = pCallbacks;
	InitCallbacks(Window);
}