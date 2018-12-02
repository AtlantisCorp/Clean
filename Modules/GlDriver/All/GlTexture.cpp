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
GLint GlGetTextureBound(GLenum binding, GlPtrTable const& gl)
{
    GLint result;
    gl.getIntegerv(binding, &result);
    return result;
}

/** @brief Uses RAII to bind and unbind a texture object. Useful to back up currently bound texture. */
struct GlTextureBinder 
{
    GLenum target;
    GLuint boundTex;
    bool shouldRebind;
    GlPtrTable const& gl;
    
    GlTextureBinder(GLenum trg, Texture const& tex, GlPtrTable const& tbl) : target(trg), gl(tbl)
    {
        GLenum binding = GlGetTextureTargetBinding(target);
        boundTex = (GLuint) GlGetTextureBound(binding, gl);
        shouldRebind = (boundTex > 0);
        tex.bind();
    }
    
    ~GlTextureBinder()
    {
        if (shouldRebind) 
            gl.bindTexture(target, boundTex);
    }
};

GlTexture::GlTexture(Driver* creator, GLuint hdl, GLenum trg, GlPtrTable const& tbl) : Texture(creator), handle(hdl), target(trg), gl(tbl)
{
    assert(handle && "Invalid GL Handle.");
    assert(target && "Invalid GL Texture target.");
}

std::size_t GlTexture::getWidth() const 
{
    GlTextureBinder binder(target, *this, gl);
    
    GLint width;
    gl.getTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
    
    return static_cast < std::size_t >(width);
}

std::size_t GlTexture::getHeight() const 
{
    GlTextureBinder binder(target, *this, gl);
    
    GLint height;
    gl.getTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
    
    return static_cast < std::size_t >(height);
}

void GlTexture::bind() const 
{
    gl.bindTexture(target, handle);
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
    GlTextureBinder binder(target, *this, gl);
    
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
    
    gl.pixelStorei(GL_PACK_ROW_LENGTH, (GLint) image->findRowLength());
    gl.pixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    switch (target)
    {
        case GL_TEXTURE_2D:
        gl.texImage2D(target, 0, 
                      desiredInternalFormat,
                      width, height, 
                      0, 
                      format, dataType,
                      static_cast < const GLvoid* >(image->raw()));
        break;
    }
    
    gl.generateMipmap(GL_TEXTURE_2D);
    auto error = GlCheckError(gl.getError);
    
    if (error.error != GL_NO_ERROR)
    {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "glTexImage failed: %s.", error.string.data()));
        return false;
    }
    
    return true;
}

void GlTexture::releaseResource() 
{
    gl.deleteTextures(1, &handle);
    released.store(true);
    handle = 0;
    target = 0;
}
