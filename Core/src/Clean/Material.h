/** =======================================================
 *  \file Core/Material.h
 *  \date 10/14/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_MATERIAL_H
#define CLEAN_MATERIAL_H

#include "Handled.h"
#include "EffectParameter.h"
#include "FileLoader.h"
#include "Property.h"

#include <memory>
#include <vector>

namespace Clean 
{
    class Material : public Handled < Material >
    {
        typedef std::shared_ptr < EffectParameter > SharedParameter;
        
        //! @brief Diffuse color. 
        SharedParameter diffuseColor;
        
        //! @brief Specular color.
        SharedParameter specularColor;
        
        //! @brief Ambient color.
        SharedParameter ambientColor;
        
        //! @brief Emissive color.
        SharedParameter emissiveColor;
        
        //! @brief Material's name.
        Property < std::string > name;
        
    public:
        
        /*! @brief Constructs an empty material. */
        Material();
        
        /*! @brief Constructs a material with a name. */
        Material(std::string const& name);
        
        /*! @brief Copies a given material. */
        Material(Material const& rhs) = default;
        
        /*! @brief Returns the diffuse color. */
        glm::vec4 getDiffuseColor() const;
        
        /*! @brief Sets the diffuse color. */
        void setDiffuseColor(glm::vec4 const& color);
        
        /*! @brief Returns the specular color. */
        glm::vec4 getSpecularColor() const;
        
        /*! @brief Sets the specular color. */
        void setSpecularColor(glm::vec4 const& color);
        
        /*! @brief Returns the ambient color. */
        glm::vec4 getAmbientColor() const;
        
        /*! @brief Sets the ambient color. */
        void setAmbientColor(glm::vec4 const& color);
        
        /*! @brief Returns the emissive color. */
        glm::vec4 getEmissiveColor() const;
        
        /*! @brief Sets the emissive color. */
        void setEmissiveColor(glm::vec4 const& color);
        
        /*! @brief Returns all EffectParameters. */
        std::vector < std::shared_ptr < EffectParameter > > findAllParameters() const;
        
        /*! @brief Returns name. */
        std::string getName() const;
    };
    
    /** @brief Specialization of FileLoader for Material. */
    template <> 
    class FileLoader < Material > : public FileLoaderInterface 
    {
    public:
        virtual ~FileLoader() = default;
        
        /*! @brief Loads a Material file and returns a list of Materials. */
        virtual std::vector < std::shared_ptr < Material > > load(std::string const& filepath) const = 0;
    };
}

#endif // CLEAN_MATERIAL_H
