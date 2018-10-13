/** \file GlDriver/GlShader.h
**/

#ifndef GLDRIVER_GLSHADER_H
#define GLDRIVER_GLSHADER_H

#include "GlInclude.h"

#include <string>
#include <Clean/Shader.h>

/** @brief OpenGL implementation for Clean::Shader. */
class GlShader : public Clean::Shader
{
    //! @brief Our OpenGL handle for shader object.
    GLuint shaderHandle;
    
    //! @brief Is this shader correctly compiled?
    bool compiled;
    
    //! @brief Error stored from the compilation, or empty if no error.
    std::string compilerError;
    
public:
    
    /*! @brief Constructs the shader. */
    GlShader(const char* src, std::uint8_t stage);
    
    /*! @brief Destructs the shader. */
    ~GlShader();
    
    /*! @brief Returns true if shader is valid. */
    bool isValid() const;
    
    /*! @brief Returns GL Handle. */
    GLuint getGLHandle() const;
    
protected:
    
    /*! @brief Releases the shader object. */
    void releaseResource();
};

#endif // GLDRIVER_GLSHADER_H
