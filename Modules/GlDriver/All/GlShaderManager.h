/** \file GlDriver/GlShaderManager.h
**/

#ifndef GLDRIVER_GLSHADERMANAGER_H
#define GLDRIVER_GLSHADERMANAGER_H

#include "GlShader.h"
#include <Clean/Manager.h>

class GlShaderManager : public Clean::Manager < GlShader >
{
public:
    
    GlShaderManager() = default;
    
    std::shared_ptr < GlShader > findByHandle(std::size_t handle) const;
    
    std::shared_ptr < GlShader > findByPath(std::string const& origin) const;
};

#endif // GLDRIVER_GLSHADERMANAGER_H