/** \file GlDriver/GlRenderPipeline.h
**/

#ifndef GLDRIVER_GLRENDERPIPELINE_H
#define GLDRIVER_GLRENDERPIPELINE_H

#include "GlInclude.h"

#include <Clean/RenderPipeline.h>
#include <Clean/Property.h>
#include <Clean/AtomicCounter.h>

class GlRenderPipeline : public Clean::RenderPipeline
{
    //! @brief OpenGL Program object.
    GLuint programHandle;
    
    //! @brief Association map between parameter's location and texture unit. 
    mutable Clean::Property < std::map < GLint, GLint > > textureUnits;
    
    //! @brief Counter for newly allocated texture units. 
    mutable Clean::AtomicCounter < GLint > unitCounter;
    
public:
    
    /*! @brief Constructs a pipeline. */
    GlRenderPipeline(Clean::Driver* driver);
    
    /*! @brief Links the given shader to its program object. 
     *
     * Shader given must be of type GlShader, and not null. Linking of the program object
     * is done when requesting an attribute, or a uniform or any other resources that needs
     * the pipeline to be linked. GlRenderPipeline::link() can also be used. It is also done
     * in GlRenderPipeline::bind(). 
     *
    **/
    void shader(std::uint8_t stage, std::shared_ptr < Clean::Shader > const& shad);
    
    /*! @brief Links the program. */
    void link();
    
    /*! @brief Binds the program to given driver. */
    void bind(Clean::Driver const& driver) const;
    
    /*! @brief Returns true if the program is linked. */
    bool isLinked() const;
    
    /*! @brief Calls glValidateProgram and returns the information in program's info log. */
    std::string validate() const;
    
    /*! @brief Binds one parameter onto this pipeline. 
     *  Calls the correct glUniform* function.
    **/
    void bindParameter(Clean::ShaderParameter const& parameters) const;
    
    /*! @brief Binds multiple ShaderAttribute onto this pipeline. */
    void bindShaderAttributes(Clean::ShaderAttributesMap const& attributes) const;
    
    /*! @brief Sets the current drawing method. */
    void setDrawingMethod(std::uint8_t drawingMethod) const;
    
    /*! @brief Returns true if the given attribute is present in this pipeline. */
    bool hasAttribute(std::string const& attrib) const;
    
    /*! @brief Returns an index representing the location of a given attribute in the pipeline. */
    std::uint8_t findAttributeIndex(std::string const& attrib) const;
    
    /*! @brief Binds the given Texture to the given EffectParameter.
     * 
     * Texture unit is guessed from the current session. Each ShaderParameter's location used is associated
     * to a new Texture unit. The Texture is bound to the Texture unit guessed from the parameter's location.
     * This way, there is no need to purge the Texture's list as it will always overwrite the parameter's
     * location associated texture. 
     * 
    **/
    void bindTexture(Clean::ShaderParameter const& parameter, Clean::Texture const& texture) const;
    
protected:
    
    /*! @brief Releases our GL program. */
    void releaseResource();
    
    /*! @brief Finds the Texture Unit associated to given parameter's location. */
    GLint findTextureUnit(GLint location) const;
};

#endif // GLDRIVER_GLRENDERPIPELINE_H
