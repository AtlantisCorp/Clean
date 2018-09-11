/** \file Core/RenderPipeline.cpp
**/

#include "RenderPipeline.h"

namespace Clean 
{
    void RenderPipeline::shader(std::uint8_t stage, std::shared_ptr < Shader > const& shad) 
    {
        if (!shad) return;
        std::scoped_lock < std::mutex > lck(shadersMutex);
        shaders[stage] = shad;
    }
}