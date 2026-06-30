#pragma once
#include <SDL3/SDL.h>

SDL_GPUShader* LoadShader(const char * pad, SDL_GPUShaderStage type, SDL_GPUDevice* device);