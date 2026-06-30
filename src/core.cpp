#include "core.h"
#include "vertex.h"
#include "shader.h"
#include <iostream>
#include <string>

void App::Init()
{
    Window = SDL_CreateWindow("test", 1280, 720, 0);
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, nullptr);
    shouldRestart = true;
    Running = true;

    vertexBuffer = nullptr;
    transferBuffer = nullptr;
    vertexShader = nullptr;
    fragmentShader = nullptr;
    graphicsPipeline = nullptr;

    SDL_ClaimWindowForGPUDevice(device, Window);
}

void App::Run()
{
    Vertex vertices[] =
    {
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}
    };

    SDL_GPUBufferCreateInfo bufferInfo = {};
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
    transferBufferInfo.size = sizeof(vertices);
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);

    Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);

    data[0] = vertices[0];
    data[1] = vertices[1];
    data[2] = vertices[2];

    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    SDL_GPUTransferBufferLocation location = {};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    SDL_GPUBufferRegion region = {};
    region.buffer = vertexBuffer;
    region.size = sizeof(vertices);
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    std::string vertexPad = std::string(SDL_GetBasePath()) + "../shaders/vertex.spv";
    std::string fragmentPad = std::string(SDL_GetBasePath()) + "../shaders/fragment.spv";
    vertexShader = LoadShader(vertexPad.c_str(), SDL_GPU_SHADERSTAGE_VERTEX, device);
    fragmentShader = LoadShader(fragmentPad.c_str(), SDL_GPU_SHADERSTAGE_FRAGMENT, device);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);
    
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;
    
    SDL_GPUVertexAttribute vertexAttributes[2];
    
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].offset = 0;
    
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[1].offset = sizeof(float) * 3;
    
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, Window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;
 
    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);


    while (Running)
    {
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) { Running = false; shouldRestart = false; }
        }
        
        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (!commandBuffer) {std::cout << "hey";}
        
        SDL_GPUTexture* swapchainTexture;
        Uint32 Width, Height;
        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, Window, &swapchainTexture, &Width, &Height);
        if (!swapchainTexture)
        {
            SDL_SubmitGPUCommandBuffer(commandBuffer);
            continue;
        }
        
        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.clear_color = {0.5f, 0.1f, 0.8f, 1.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture = swapchainTexture;
        
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);

        SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

        SDL_GPUBufferBinding bufferBindings[1];
        bufferBindings[0].buffer = vertexBuffer;
        bufferBindings[0].offset = 0;

        SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass);

        SDL_SubmitGPUCommandBuffer(commandBuffer);

        SDL_Delay(1000 / 60);
    }

    Destroy();
}

void App::Destroy()
{
    SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_DestroyWindow(Window);
    SDL_DestroyGPUDevice(device);
}

bool App::getShouldRestart() {return shouldRestart;}

App::App() { Init(); }