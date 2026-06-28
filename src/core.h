#pragma once
#include <SDL3/SDL.h>

class App
{
public:
    App();
    void Init();
    void Run();
    void Destroy();
    bool getShouldRestart();
private:
    SDL_Window* Window;
    SDL_Event Event;
    bool shouldRestart;
    bool Running;
};