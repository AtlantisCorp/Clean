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

GlShader::GlShader(const char* src, std::uint8_t type)
    : Shader(type), shaderHandle(0), compiled(false), compilerError()
{
    shaderHandle = glCreateShader(GlShaderStage(type));
    if (!shaderHandle) return;
    
    glShaderSource(shaderHandle, 1, &src, NULL);
    glCompileShader(shaderHandle);
    
    GLint status;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
    
    if (status != GL_TRUE) {
        GLsizei maxLength, length;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar buffer[maxLength];
        glGetShaderInfoLog(shaderHandle, maxLength, &length, buffer);
        
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
    glDeleteShader(shaderHandle);
    shaderHandle = 0;
    compiled = false;
    compilerError.clear();
    released = true;
}
