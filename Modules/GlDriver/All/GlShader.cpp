/** \file GlDriver/GlShader.cpp
**/

#include "GlShader.h"

#include <Clean/NotificationCenter.h>
using namespace Clean;

static GLenum GlShaderStage(std::uint8_t type)
{
    switch(type)
    {
        case kShaderTypeVertex: return GL_VERTEX_SHADER;
        case kShaderTypeGeometry: return GL_GEOMETRY_SHADER;
        case kShaderTypeTessEval: return GL_TESS_EVALUATION_SHADER;
        case kShaderTypeTessControl: return GL_TESS_CONTROL_SHADER;
        case kShaderTypeFragment: return GL_FRAGMENT_SHADER;
            
        default:
        return GL_INVALID_ENUM;
    }
}

GlShader::GlShader(const char* src, std::uint8_t type, GlPtrTable const& tbl)
    : Shader(type), shaderHandle(0), compiled(false), compilerError(), gl(tbl)
{
    shaderHandle = gl.createShader(GlShaderStage(type));
    if (!shaderHandle) return;
    
    gl.shaderSource(shaderHandle, 1, &src, NULL);
    gl.compileShader(shaderHandle);
    
    GLint status;
    gl.getShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
    
    if (status != GL_TRUE) {
        GLsizei maxLength, length;
        gl.getShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar buffer[maxLength];
        gl.getShaderInfoLog(shaderHandle, maxLength, &length, buffer);
        
        Notification notif = BuildNotification(kNotificationLevelError, "Shader #%i cannot be compiled. Error is: %s",
            this->getHandle(), buffer);
        NotificationCenter::GetDefault()->send(notif);
        
        compilerError = std::string(buffer, length);
        return;
    }
    
    compiled = true;
}

GlShader::~GlShader()
{
    if (!released) {
        releaseResource();
    }
}

bool GlShader::isValid() const
{
    return shaderHandle != 0;
}

GLuint GlShader::getGLHandle() const
{
    return shaderHandle;
}

void GlShader::releaseResource()
{
    if (!compiled) return;
    gl.deleteShader(shaderHandle);
    shaderHandle = 0;
    compiled = false;
    compilerError.clear();
    released = true;
}
