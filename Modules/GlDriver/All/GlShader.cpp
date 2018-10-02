/** \file GlDriver/GlShader.cpp
**/

#include "GlShader.h"

#include <Clean/NotificationCenter.h>
using namespace Clean;

GlShader::GlShader(const char* src, std::uint8_t type)
    : Shader(type), shaderHandle(0), compiled(false), compilerError()
{
    shaderHandle = glCreateShader(GlShaderStage(type));
    if (!shaderHandle) return;
    
    glShaderSource(shaderHandle, 1, &src, NULL);
    glCompileShader(shaderHandle);
    
    GLint status;
    glGetShader(shaderHandle, GL_COMPILE_STATUS, &status);
    
    if (status != GL_TRUE) {
        GLsizei maxLength, length;
        glGetShader(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar buffer[maxLength];
        glGetShaderInfoLog(shaderHandle, maxLength, &length, buffer);
        
        Notification notif = BuildNotification(kNotificationLevelError, "Shader #%i cannot be compiled. Error is: %s", getHandle(), buffer);
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

void GlShader::releaseResource()
{
    if (!compiled) return;
    glDeleteShader(shaderHandle);
    shaderHandle = 0;
    compiled = false;
    compilerError.clear();
}