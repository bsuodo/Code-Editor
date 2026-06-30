#include "shader.h"
#include <iostream>

SDL_GPUShader* LoadShader(const char* pad, SDL_GPUShaderStage type, SDL_GPUDevice* device)
{
    size_t shaderSizeCode;
void* shaderCode = SDL_LoadFile(pad, &shaderSizeCode);
if (!shaderCode) {
    std::cout << "Failed to load shader: " << pad << " — " << SDL_GetError() << std::endl;
    return nullptr; // also: don't continue with a null shaderCode!
}

    SDL_GPUShaderCreateInfo shaderInfo = {};
    shaderInfo.stage = type;
    shaderInfo.code_size = shaderSizeCode;
    shaderInfo.entrypoint = "main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.code = (Uint8*)shaderCode;
    shaderInfo.num_samplers = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.num_storage_textures = 0;
    shaderInfo.num_uniform_buffers = 0;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);

    SDL_free(shaderCode);


    return shader;
}