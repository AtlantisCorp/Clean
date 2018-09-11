/** \file Core/ShaderAttribute.h
**/

#ifndef CLEAN_SHADERATTRIBUTE_H
#define CLEAN_SHADERATTRIBUTE_H

namespace Clean 
{
    class Buffer;
    
    /** @brief Generic attribute in a shader. */
    struct ShaderAttribute 
    {
        //! @brief Index used in the shader to identify the attribute.
        std::size_t index;
        
        //! @brief Offset in the buffer to localize first data, in bytes. 
        std::ptrdiff_t offset;
        
        //! @brief Stride between two data, in bytes.
        std::ptrdiff_t stride;
        
        //! @brief Buffer used to bind data. 
        std::shared_ptr < Buffer > buffer;
    };
    
    class ShaderAttributesMap final
    {
        //! @brief Stores ShaderAttributes by value. 
        std::vector < ShaderAttribute > attribs;
        
        //! @brief Protects attribs.
        std::mutex attribsMutex;
        
    public:
        
        /*! @brief Default constructor. */
        ShaderAttributesMap();
        
        /*! @brief Default destructor. */
        ~ShaderAttributesMap() = default;
        
        /*! @brief Adds a ShaderAttribute to this map. */
        void add(ShaderAttribute&& attrib);
    };
}

#endif // CLEAN_SHADERATTRIBUTE_H