#include "core.h"

void App::Destroy()
{
    SDL_DestroyWindow(Window);
}

void App::Run()
{
    while (Running)
    {
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) { Running = false; shouldRestart = false; }
        }
        
        SDL_Delay(1000 / 60);
    }

    Destroy();
}

bool App::getShouldRestart() {return shouldRestart;}

void App::Init()
{
    Window == SDL_CreateWindow("test", 1280, 720, 0);
    shouldRestart = true;
    Running = true;
}

App::App() { Init(); }