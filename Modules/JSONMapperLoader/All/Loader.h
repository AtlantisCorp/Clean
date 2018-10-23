/** **********************************************
 *  \file JSONMapperLoader/Loader.h
 *  \date 10/23/2018
 *  \author Luk2010
**/

#ifndef JSONMAPPERLOADER_LOADER_H
#define JSONMAPPERLOADER_LOADER_H

#include <Clean/FileLoader.h>
#include <Clean/ShaderMapper.h>
#include <Clean/Version.h>

//! @brief Current version.
static Clean::Version const kCurrentJSONMapperVersion = Clean::Version::FromString("1.0");

/** @brief Loads a JSON file and converts it to a ShaderMapper. 
 *
 * Reads the JSON file with RapidJSON. The JSON file is read as follow: 
 * 
 * 'Name': Name of the future ShaderMapper. Field is valid only when having a BuildableShaderMapper. 
 * 'Version': Version used by this file. For now, only the current version is supported. 
 *      \see kCurrentJSONMapperVersion.
 *
 * 'Attributes': Lists all Attributes for a shader, by name. 
 * 'Constants': Lists all Constants (Uniforms) for a shader pipeline, by name. 
 *
 * For each Attributes, the following fields are available: 
 * 'name': Corresponds to the VertexComponent constant describing best the attribute. 
 * 'type': Types used for this Attribute. Can be int, unsigned, float, double, vec2, vec3, vec4,
 *      ivec2, ivec3, ivec4, uvec2, uvec3, uvec4, mat2, mat3, mat4, and all glm types. 
 * 'index': Index used for the Attribute in the shader. -1 by default. 
 *
 * For each Constants, the following fields are available: 
 * 'name': Corresponds to the EffectParameter constant describing best the attribute. 
 * 'type': Types used for this Attribute. Can be int, unsigned, float, double, vec2, vec3, vec4,
 *      ivec2, ivec3, ivec4, uvec2, uvec3, uvec4, mat2, mat3, mat4, and all glm types. 
 * 'index': Index used for the Attribute in the shader. -1 by default. 
 *
 * Some fields may be added by the future. 
 *
**/
class JSONMapperLoader : public Clean::FileLoader < Clean::ShaderMapper >
{
public:
    
    /*! @brief Must return true if the given extension is loadable by this loader. */
    bool isLoadable(std::string const& extension) const;
    
    /*! @brief May return a FileLoaderInfos structure, if provided by the implementation. */
    Clean::FileLoaderInfos getInfos() const;
    
    /*! @brief Loads a JSON file and converts it to a ShaderMapper object. */
    std::shared_ptr < Clean::ShaderMapper > load(std::string const& file) const;
};

#endif // JSONMAPPERLOADER_LOADER_H
