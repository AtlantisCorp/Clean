/*! ================================================
 *  \file OBJMeshLoader/MtlLoader.h
 *  \date 10/17/2018
 *  \author luk2010
**/

#ifndef OBJMESHLOADER_MTLLOADER_H
#define OBJMESHLOADER_MTLLOADER_H

#include <Clean/FileLoader.h>
#include <Clean/Material.h>

/** @brief Mtl file loader for Clean::Material. 
 *
 * This Mtl loader is proposed to parse Mtl file with this module and OBJMeshLoader.
 * It tries to follow the Mtl Specification as found there:
 * http://paulbourke.net/dataformats/mtl/
 *
 * It has been adapted to follow Clean::Material parameters. Parameters may be added
 * in the future. The following keywords are supported:
 *
 * | Keyword |       Definition       | Format |
 * |:-------:|:----------------------:|:------:|
 * |  newmtl | Defines a new material |        |
 * |    Ka   |      Ambient color     |   RGB  |
 * |    Kd   |      Diffuse color     |   RGB  |
 * |    Ks   |     Specular color     |   RGB  |
 * |    Ke   |     Emissive color     |   RGB  |
 *
**/
class MtlLoader : public Clean::FileLoader < Clean::Material >
{
public:
    
    /*! @brief Must return true if the given extension is loadable by this loader. */
    bool isLoadable(std::string const& extension) const;
    
    /*! @brief May return a FileLoaderInfos structure, if provided by the implementation. */
    Clean::FileLoaderInfos getInfos() const;
    
    /*! @brief Loads a Material file and returns a list of Materials. */
    std::vector < std::shared_ptr < Clean::Material > > load(std::string const& filepath) const;
    
private:
    
    /*! @brief Finds next Material object and process it. */
    std::shared_ptr < Clean::Material > findNextMaterial(std::istream& stream, std::size_t& lineCount) const;
};

#endif // OBJMESHLOADER_MTLLOADER_H
