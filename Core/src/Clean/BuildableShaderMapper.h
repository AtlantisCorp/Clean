/** ********************************************
 *  \file Core/BuildableShaderMapper.h
 *  \date 10/21/2018
 *  \author luk2010
**/

#ifndef CLEAN_BUILDABLESHADERMAPPER_H
#define CLEAN_BUILDABLESHADERMAPPER_H

#include "ShaderMapper.h"
#include "Property.h"

#include <string>
#include <cstddef>
#include <vector>
#include <memory>

namespace Clean 
{
    /** @brief A ShaderMapper that can be parameted. 
     *
     * This class offers the possibility to build a dynamic ShaderMapper. Normally, ShaderMapper
     * maps VertexDescriptor and EffectParameter in a static way, enabling a simple interface for 
     * a user to create a new ShaderMapper. 
     *
     * However, this system is not adequate because the user must create a ShaderMapper for each
     * shaders he uses. This class enables some dynamic creation of a ShaderMapper, for an external
     * loader to be able to load a ShaderMapper from given simpler interface. 
     *
     * As an example, JSONMapperLoader is a loader that creates a ShaderMapper from a JSON file. This
     * file is very simple to write and can be created directly by the shader's writer. This way the 
     * shader's user only needs to load this JSON file to use the shaders associated to it. 
     *
    **/
    class BuildableShaderMapper : public ShaderMapper 
    {
    public:
        
        /*! @brief Generic structure to hold some useful informations about an attribute. */
        struct Attribute 
        {
            std::string name;
            std::uint8_t vertexComponent = 0;
            std::int8_t index = -1;
            std::uint8_t type = 0;
            std::uint8_t components = 0;
        };
        
        /*! @brief Generic structure to hold some useful informations about a constant in a shader, or
         *  a uniform (GL way). 
        **/
        struct Constant
        {
            std::string name;
            std::uint8_t type = 0;
            std::int8_t index = -1;
            std::uint64_t hash = 0;
        };
        
    private:
        
        //! @brief Predefined array of attributes.
        Property < std::vector < Attribute > > attributes;
        
        //! @brief Holds constants defined for the shader. 
        Property < std::vector < Constant > > constants;
        
        //! @brief Name of this mapper. 
        Property < std::string > name;
        
    public:
        
        /*! @brief Maps the given descriptor to given pipeline. 
         *
         * It looks inside the attributes array to find a correspondance with each VertexComponent present
         * in the descriptor. The resulting ShaderAttributesMap will contain ShaderAttribute filled with 
         * informations from this attribute array and from the descriptor. 
         *
        **/
        ShaderAttributesMap map(VertexDescriptor const& descriptor, RenderPipeline const& pipeline) const;
        
        /*! @brief Maps the given parameter to given pipeline. */
        ShaderParameter map(EffectParameter const& param, RenderPipeline const& pipeline) const;
        
    private:
        
        /*! @brief Finds an attribute already parameted. */
        Attribute findAttribute(std::uint8_t component) const;
        
        /*! @brief Finds a constant already parameted. */
        Constant findConstant(std::uint64_t hash) const;
        
    public:
        
        /*! @brief Adds an Attribute. */
        bool addAttribute(Attribute const& attrib);
        
        /*! @brief Adds a constant. */
        bool addConstant(Constant const& constant);
        
        /*! @brief Clears all attributes and constants. */
        void clear();
        
        /*! @brief Changes name. */
        void setName(std::string const& value);
        
        /*! @brief Returns name. */
        std::string getName() const;
    };
    
    /* Example use of an hypothetic JSON Mapper loader. 
    
    {
        std::shared_ptr < BuildableShaderMapper > result = AllocateShared < BuildableShaderMapper >();
        
        JSONElement root;
        
        for (JSONElement const& child : root)
        {
            if (child.name() == "Name") {
                result->setName(child.value().to < std::string >());
            }
            
            else if (child.name() == "Version") {
                assert(child.value().to < Version >() == JSONMapperLoader::Version);
            }
            
            else if (child.name() == "Attributes") {
                for (JSONElement const& attribute : child.value().children()) 
                {
                    Attribute attrib; 
                    attrib.name = attribute.name();
                    
                    JSONElement const& value = attribute.value().to < JSONElementObject >();
                    std::uint8_t component = VertexComponent::FromName(value.at("name"));
                        
                    attrib.vertexComponent = value.at("name");
                    attrib.index = value.has("index") ? value.at("index") : -1;
                    attrib.type = value.has("type") ? value.at("type") : VertexComponent::FindShaderParamType(component);
                    attrib.components = value.has("elements") ? value.at("elements") : VertexComponent::FindShaderParamElements(component);
                    
                    result->addAttribute(attrib);
                }
            }
            
            else if (child.name() == "Constants") {
                for (JSONElement const& constant : child.value().children())
                {
                    Constant cst;
                    cst.name = constant.name();
                    
                    JSONElement const& value = constant.value().to < JSONElementObject >();
                    cst.hash = Hash64(value.at("name"));
                    cst.type = value.has("type") ? value.at("type") : ShaderParam::FindTypeFromHash(cst.hash);
                    cst.index = value.has("index") ? value.at("index") : -1;
                    
                    result->addConstant(cst);
                }
            }
        }
        
        return result;
    }
    
    */
}

#endif // CLEAN_BUILDABLESHADERMAPPER_H