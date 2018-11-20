/** 
 *  \file GlDriver/GlTexture.cpp
 *  \date 11/06/2018
**/

#include "GlTexture.h"
#include "GlCheckError.h"

#include <Clean/NotificationCenter.h>
using namespace Clean;

/*! @brief Converts a GL Texture Target to its corresponding GL Texture binding. */
GLenum GlGetTextureTargetBinding(GLenum target) 
{
    switch (target)
    {
        case GL_TEXTURE_1D: return GL_TEXTURE_BINDING_1D;
        case GL_TEXTURE_1D_ARRAY: return GL_TEXTURE_BINDING_1D_ARRAY;
        case GL_TEXTURE_2D: return GL_TEXTURE_BINDING_2D;
        case GL_TEXTURE_2D_ARRAY: return GL_TEXTURE_BINDING_2D_ARRAY;
        case GL_TEXTURE_2D_MULTISAMPLE: return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
        case GL_TEXTURE_3D: return GL_TEXTURE_BINDING_3D;
        case GL_TEXTURE_BUFFER: return GL_TEXTURE_BINDING_BUFFER;
        case GL_TEXTURE_CUBE_MAP: return GL_TEXTURE_BINDING_CUBE_MAP;
        case GL_TEXTURE_RECTANGLE: return GL_TEXTURE_BINDING_RECTANGLE;
        
        default:
        return GL_INVALID_ENUM;
    }
}

/*! @brief Returns currently bound texture. */
GLint GlGetTextureBound(GLenum binding)
{
    GLint result;
    glGetIntegerv(binding, &result);
    return result;
}

/** @brief Uses RAII to bind and unbind a texture object. Useful to back up currently bound texture. */
struct GlTextureBinder 
{
    GLenum target;
    GLuint boundTex;
    bool shouldRebind;
    
    GlTextureBinder(GLenum trg, Texture const& tex) : target(trg)
    {
        GLenum binding = GlGetTextureTargetBinding(target);
        boundTex = (GLuint) GlGetTextureBound(binding);
        shouldRebind = (boundTex > 0);
        tex.bind();
    }
    
    ~GlTextureBinder()
    {
        if (shouldRebind) 
            glBindTexture(target, boundTex);
    }
};

GlTexture::GlTexture(Driver* creator, GLuint hdl, GLenum trg) : Texture(creator), handle(hdl), target(trg)
{
    assert(handle && "Invalid GL Handle.");
    assert(target && "Invalid GL Texture target.");
}

std::size_t GlTexture::getWidth() const 
{
    GlTextureBinder binder(target, *this);
    
    GLint width;
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
    
    return static_cast < std::size_t >(width);
}

std::size_t GlTexture::getHeight() const 
{
    GlTextureBinder binder(target, *this);
    
    GLint height;
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
    
    return static_cast < std::size_t >(height);
}

void GlTexture::bind() const 
{
    glBindTexture(target, handle);
}

GLenum GlGetInternalFormat(std::uint8_t format)
{
    switch (format)
    {
        case kPixelFormatRGB8: return GL_RGB8;
        case kPixelFormatRGBA8: return GL_RGBA8;
        
        default:
        return GL_INVALID_ENUM;
    }
}

GLenum GlChooseBestInternalPixelFormat(std::uint8_t internal, std::uint8_t external)
{
    GLenum desiredInternalFormat = GlGetInternalFormat(internal);
    if (desiredInternalFormat == GL_INVALID_ENUM) desiredInternalFormat = GlGetInternalFormat(external);
    return desiredInternalFormat;
}

GLenum GlGetPixelFormat(std::uint8_t format)
{
    switch (format)
    {
        case kPixelFormatRGB8: return GL_RGB;
        case kPixelFormatRGBA8: return GL_RGBA;
        
        default:
        return GL_INVALID_ENUM;
    }
}

GLenum GlGetPixelDataType(std::uint8_t format)
{
    switch (format)
    {
        case kPixelFormatRGB8:
        case kPixelFormatRGBA8:
        return GL_UNSIGNED_BYTE;
        
        default:
        return GL_INVALID_ENUM;
    }
}

bool GlTexture::upload(std::shared_ptr < Image > const& image)
{
    if (!image) return false;
    GlTextureBinder binder(target, *this);
    
    GLenum desiredInternalFormat = GlChooseBestInternalPixelFormat(internalFormat, image->pixelFormat());
    
    if (desiredInternalFormat == GL_INVALID_ENUM)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "GlChooseBestInternalPixelFormat failed. Image format is: %s.", 
            PixelFormatToString(image->pixelFormat()).data());
        NotificationCenter::GetDefault()->send(notif);
        return false;
    }
    
    auto size = image->getSize();
    GLsizei width = static_cast < GLsizei >(size.x);
    GLsizei height = static_cast < GLsizei >(size.y);
    
    GLenum format = GlGetPixelFormat(image->pixelFormat());
    GLenum dataType = GlGetPixelDataType(image->pixelFormat());
    
    if (format == GL_INVALID_ENUM || dataType == GL_INVALID_ENUM)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "GlGetPixelFormat or GlGetPixelDataType failed. Image format is: %s.", 
            PixelFormatToString(image->pixelFormat()).data());
        NotificationCenter::GetDefault()->send(notif);
        return false;
    }
    
    glPixelStorei(GL_PACK_ROW_LENGTH, (GLint) image->findRowLength());
    
    switch (target)
    {
        case GL_TEXTURE_2D:
        glTexImage2D(target, 0, 
                     desiredInternalFormat, 
                     width, height, 
                     0, 
                     format, dataType, 
                     static_cast < const GLvoid* >(image->raw()));
        break;
    }
    
    auto error = GlCheckError();
    
    if (error.error != GL_NO_ERROR)
    {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "glTexImage failed: %s.", error.string.data()));
        return false;
    }
    
    return true;
}

void GlTexture::releaseResource() 
{
    glDeleteTextures(1, &handle);
    released.store(true);
    handle = 0;
    target = 0;
}
