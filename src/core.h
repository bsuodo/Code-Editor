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
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUTransferBuffer* transferBuffer;
    SDL_Window* Window;
    SDL_Event Event;
    SDL_GPUDevice* device;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    SDL_GPUGraphicsPipeline* graphicsPipeline;
    bool shouldRestart;
    bool Running;
};