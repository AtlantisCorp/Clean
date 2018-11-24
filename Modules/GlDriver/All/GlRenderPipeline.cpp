/** \file GlDriver/GlRenderPipeline.cpp
**/

#include "GlRenderPipeline.h"
#include "GlShader.h"
#include "GlCheckError.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include <Clean/NotificationCenter.h>
#include <Clean/Traits.h>
using namespace Clean;

static GLenum GlGetShaderAttrib(std::uint8_t type)
{
    switch(type)
    {
        case kShaderAttribI8: return GL_BYTE;
        case kShaderAttribU8: return GL_UNSIGNED_BYTE;
        case kShaderAttribI16: return GL_SHORT;
        case kShaderAttribU16: return GL_UNSIGNED_SHORT;
        case kShaderAttribI32: return GL_INT;
        case kShaderAttribU32: return GL_UNSIGNED_INT;
        case kShaderAttribHalfFloat: return GL_HALF_FLOAT;
        case kShaderAttribFloat: return GL_FLOAT;
        case kShaderAttribDouble: return GL_DOUBLE;
        
        default:
        return GL_INVALID_ENUM;
    }
}

static GLenum GlGetPolygonMode(std::uint8_t mode)
{
    switch(mode)
    {
        case kDrawingMethodPoints: return GL_POINT;
        case kDrawingMethodLines: return GL_LINE;
        case kDrawingMethodFilled: return GL_FILL;
        
        default:
        return GL_INVALID_ENUM;
    }
}

static GLuint GlGetCurrentProgram(void)
{
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    return static_cast < GLuint >(currentProgram);
}

GlRenderPipeline::GlRenderPipeline(Driver* driver)
    : RenderPipeline(driver), unitCounter(0)
{
    programHandle = glCreateProgram();
}

void GlRenderPipeline::shader(std::uint8_t stage, std::shared_ptr < Shader > const& shad)
{
    if (isLinked()) {
        Notification notif = BuildNotification(kNotificationLevelWarning,
            "Can't attach shader #%i because program #%i is already linked.", 
            shad->getHandle(), this->getHandle());
        NotificationCenter::GetDefault()->send(notif);
        return;
    }
    
    auto shader = ReinterpretShared < GlShader >(shad);
    glAttachShader(programHandle, shader->getGLHandle());
    
    GlError error = GlCheckError();
    if (error.error != GL_NO_ERROR) {
        Notification notif = BuildNotification(kNotificationLevelError,
            "An error occured while glAttachShader: %s.",
            error.string.data());
        NotificationCenter::GetDefault()->send(notif);
    }
}

void GlRenderPipeline::link()
{
    textureUnits.lock().clear();
    textureUnits.unlock();
    unitCounter.reset(0);
    
    glLinkProgram(programHandle);
    
    GLint result;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &result);
    
    if (result != GL_TRUE) {
        GLint maxLength;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLsizei length;
        GLchar buffer[maxLength];
        glGetProgramInfoLog(programHandle, maxLength, &length, buffer);
        
        Notification notif = BuildNotification(kNotificationLevelError,
            "Can't link program #%i: %s",
            this->getHandle(), buffer);
        NotificationCenter::GetDefault()->send(notif);
    }
}

void GlRenderPipeline::bind(Driver const& driver) const 
{
    if (!isLinked()) {
        const_cast < GlRenderPipeline* >(this)->link();
    }
    
    glUseProgram(programHandle);
}

bool GlRenderPipeline::isLinked() const 
{
    GLint result;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &result);
    return result == GL_TRUE;
}

std::string GlRenderPipeline::validate() const 
{
    glValidateProgram(programHandle);
    
    GLint maxLength, length;
    glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &maxLength);
    
    GLchar buffer[maxLength];
    glGetProgramInfoLog(programHandle, maxLength, &length, buffer);
    
    return std::string(buffer, length);
}

void GlRenderPipeline::bindParameter(ShaderParameter const& parameter) const 
{
    GLuint currentProgram = GlGetCurrentProgram();
    bool rebindCurrent = false;
    
    if (currentProgram != programHandle) {
        glUseProgram(programHandle);
        rebindCurrent = true;
    }
    
    GLint location = static_cast < GLint >(parameter.idx);
    if (location < 0) {
        location = glGetUniformLocation(programHandle, parameter.name.data());
        if (location < 0) {
            Notification notif = BuildNotification(kNotificationLevelInfo,
                "Can't bind ShaderParameter '%s' because it was not found in GlRenderPipeline #%i.",
                parameter.name.data(), this->getHandle());
            NotificationCenter::GetDefault()->send(notif);
            return;
        }
    }
    
    switch(parameter.type)
    {
        case kShaderParamU32:
        glUniform1ui(location, parameter.value.u32);
        break;
        
        case kShaderParamI32:
        glUniform1i(location, parameter.value.i32);
        break;
        
        case kShaderParamFloat:
        glUniform1f(location, parameter.value.fl);
        break;
        
        case kShaderParamVec2:
            glUniform2fv(location, 1, glm::value_ptr(parameter.value.vec2));
        break;
        
        case kShaderParamVec3:
            glUniform3fv(location, 1, glm::value_ptr(parameter.value.vec3));
        break;
        
        case kShaderParamVec4:
            glUniform4fv(location, 1, glm::value_ptr(parameter.value.vec4));
        break;
        
        case kShaderParamUVec2:
        glUniform2uiv(location, 1, glm::value_ptr(parameter.value.uvec2));
        break;
        
        case kShaderParamUVec3:
        glUniform3uiv(location, 1, glm::value_ptr(parameter.value.uvec3));
        break;
        
        case kShaderParamUVec4:
        glUniform4uiv(location, 1, glm::value_ptr(parameter.value.uvec4));
        break;
        
        case kShaderParamIVec2:
        glUniform2iv(location, 1, glm::value_ptr(parameter.value.ivec2));
        break;
        
        case kShaderParamIVec3:
        glUniform3iv(location, 1, glm::value_ptr(parameter.value.ivec3));
        break;
        
        case kShaderParamIVec4:
        glUniform4iv(location, 1, glm::value_ptr(parameter.value.ivec4));
        break;
        
        case kShaderParamMat2:
        glUniformMatrix2fv(location, 1, false, glm::value_ptr(parameter.value.mat2));
        break;
        
        case kShaderParamMat3:
        glUniformMatrix3fv(location, 1, false, glm::value_ptr(parameter.value.mat3));
        break;
        
        case kShaderParamMat4:
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(parameter.value.mat4));
        break;
        
        case kShaderParamMat2x3:
        glUniformMatrix2x3fv(location, 1, false, glm::value_ptr(parameter.value.mat2x3));
        break;
        
        case kShaderParamMat3x2:
        glUniformMatrix3x2fv(location, 1, false, glm::value_ptr(parameter.value.mat3x2));
        break;
        
        case kShaderParamMat2x4:
        glUniformMatrix2x4fv(location, 1, false, glm::value_ptr(parameter.value.mat2x4));
        break;
        
        case kShaderParamMat4x2:
        glUniformMatrix4x2fv(location, 1, false, glm::value_ptr(parameter.value.mat4x2));
        break;
        
        case kShaderParamMat3x4:
        glUniformMatrix3x4fv(location, 1, false, glm::value_ptr(parameter.value.mat3x4));
        break;
        
        case kShaderParamMat4x3:
        glUniformMatrix4x3fv(location, 1, false, glm::value_ptr(parameter.value.mat4x3));
        break;
        
        default:
        break;
    }
    
    GlError error = GlCheckError();
    if (error.error != GL_NO_ERROR) {
        Notification notif = BuildNotification(kNotificationLevelError,
            "An error occured while glUniform: %s.",
            error.string.data());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    if (rebindCurrent) {
        glUseProgram(currentProgram);
    }
}

void GlRenderPipeline::bindShaderAttributes(ShaderAttributesMap const& attributes) const 
{
    assert(driver && "Null Clean::Driver provided for this pipeline.");
    std::size_t attribsCount = attributes.countAttributes();
    
    for (std::size_t attribNum = 0; attribNum < attribsCount; attribNum++)
    {
        ShaderAttribute attrib = attributes.find(attribNum);
        
        if (attrib.enabled)
        {
            if (!attrib.buffer) {
                Notification notif = BuildNotification(kNotificationLevelWarning,
                     "ShaderAttribute index %i has a null buffer but is enabled.",
                     attribNum);
                NotificationCenter::GetDefault()->send(notif);
                continue;
            }
            
            GLuint index = static_cast < GLuint >(attrib.index);
            GLint size = static_cast < GLint >(std::clamp<std::uint8_t>(attrib.components, 1, 4));
            GLsizei stride = static_cast < GLsizei >(attrib.stride);
            GLvoid* pointer = (char*)NULL + attrib.offset;
            
            GLenum type = GlGetShaderAttrib(attrib.type);
            assert(type != GL_INVALID_ENUM && "Illegal ShaderAttribute type.");
            
            if (attrib.buffer->isBindable()) {
                attrib.buffer->bind(*driver);
            } else {
                pointer = (GLvoid*) attrib.buffer->lock(kBufferIOReadOnly);
            }
            
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, type, false, stride, pointer);
            
            GlError error = GlCheckError();
            if (error.error != GL_NO_ERROR) {
                Notification notif = BuildNotification(kNotificationLevelError,
                    "Can't bind ShaderAttribute: %s",
                    error.string.data());
                NotificationCenter::GetDefault()->send(notif);
            }
            
            if (!attrib.buffer->isBindable()) {
                attrib.buffer->unlock(kBufferIOReadOnly);
            }
        }
        
        else if (attrib.index < kShaderAttributeMax)
        {
            glDisableVertexAttribArray(static_cast < GLuint >(attrib.index));
        }
    }
}

void GlRenderPipeline::setDrawingMethod(std::uint8_t drawingMethod) const 
{
    glPolygonMode(GL_FRONT_AND_BACK, GlGetPolygonMode(drawingMethod));
}

bool GlRenderPipeline::hasAttribute(std::string const& attrib) const 
{
    if (!isLinked() || attrib.empty()) 
        return false;
    
    GLint location = glGetAttribLocation(programHandle, attrib.data());
    return location > -1;
}

std::uint8_t GlRenderPipeline::findAttributeIndex(std::string const& attrib) const
{
    GLuint result = static_cast < GLuint >(glGetAttribLocation(programHandle, attrib.data()));
    
    GlError error = GlCheckError();
    if (error.error != GL_NO_ERROR) {
        Notification notif = BuildNotification(kNotificationLevelError,
            "glGetAttribLocation('%s') failed: %s",
            attrib.data(), error.string.data());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    return result;
}

void GlRenderPipeline::bindTexture(ShaderParameter const& parameter, Texture const& texture) const
{
    GLuint currentProgram = GlGetCurrentProgram();
    bool rebindCurrent = false;
    
    if (currentProgram != programHandle) {
        glUseProgram(programHandle);
        rebindCurrent = true;
    }
    
    GLint location = static_cast < GLint >(parameter.idx);
    if (location < 0) {
        location = glGetUniformLocation(programHandle, parameter.name.data());
        if (location < 0) {
            Notification notif = BuildNotification(kNotificationLevelInfo,
                "Can't bind ShaderParameter '%s' because it was not found in GlRenderPipeline #%i.",
                parameter.name.data(), this->getHandle());
            NotificationCenter::GetDefault()->send(notif);
            return;
        }
    }
    
    // Based on location, we find the texture unit associated to this parameter. Next, we have to activate
    // this texture unit and bind the texture to it. 
    
    GLint unit = findTextureUnit(location);
    
    if (unit == -1) 
    {
        Notification notif = BuildNotification(kNotificationLevelWarning, 
            "Parameter location %i can't be associated to a Texture unit. (GlRenderPipeline #%i)",
            location, getHandle());
        NotificationCenter::GetDefault()->send(notif);
        return;
    }
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glUniform1i(location, unit);
    texture.bind();
    
    GlError error = GlCheckError();
    if (error.error != GL_NO_ERROR) {
        Notification notif = BuildNotification(kNotificationLevelError,
            "An error occured: %s.",
            error.string.data());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    if (rebindCurrent) {
        glUseProgram(currentProgram);
    }
}

bool GlRenderPipeline::isModifiable() const
{
    return !isLinked();
}

void GlRenderPipeline::releaseResource()
{
    if (programHandle) {
        glDeleteProgram(programHandle);
        programHandle = 0;
        released = true;
    }
}

GLint GlRenderPipeline::findTextureUnit(GLint location) const 
{
    auto& units = textureUnits.lock();
    auto it = units.find(location);
    
    if (it == units.end()) 
    {
        GLint allocatedUnit = unitCounter.next();
        if (allocatedUnit >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
            unitCounter.undo();
            return -1;
        }
        
        units[location] = allocatedUnit;
        textureUnits.unlock();
        return allocatedUnit;
    }
    
    GLint allocatedUnit = it->second;
    textureUnits.unlock();
    return allocatedUnit;
}
