/** \file OBJMeshLoader/OBJLoader.h
**/

#ifndef OBJMESHLOADER_OBJLOADER_H
#define OBJMESHLOADER_OBJLOADER_H

#include <Clean/FileLoader.h>
#include <Clean/Mesh.h>

struct OBJVec3 
{
    union {
        struct { float i, j, k; };
        struct { float u, v, w; };
    };
};

struct OBJVec4
{
    float x, y, z, w;
};

struct OBJVertex 
{
    OBJVec4 pos;
    OBJVec3 nor;
    OBJVec3 tex;
};

struct OBJFace 
{
    std::vector < std::size_t > idx;
};

struct OBJMesh 
{
    std::vector < OBJFace > faces;
};

struct OBJFile 
{
    std::vector < OBJVertex > vertexes;
    std::vector < OBJMesh > meshes;
    
    std::vector < OBJVec4 > globVerts;
    std::vector < OBJVec3 > globNorms;
    std::vector < OBJVec3 > globTexts;
};

/** @brief Implements Clean::FileLoader < Clean::Mesh > for OBJ File format.
**/
class OBJLoader : public Clean::FileLoader < Clean::Mesh >
{
public:
    
    /*! @brief Default constructor. */
    OBJLoader() = default;
    
    /*! @brief Default destructor. */
    ~OBJLoader() = default;
    
    /*! @brief Loads a Mesh from the given existing path. */
    std::shared_ptr < Clean::Mesh > load(std::string const& path) const;
    
    /*! @brief Must return true if the given extension is 'obj'. */
    bool isLoadable(std::string const& extension) const;
    
    /*! @brief Returns informations about this loader. */
    Clean::FileLoaderInfos getInfos() const;
    
protected:
    
    /*! @brief Process the given file to produce an OBJFile structure. */
    OBJFile makeOBJFile(std::istream& stream) const;
    
    /*! @brief Process the stream for a new OBJ Mesh. */
    OBJMesh makeOBJMesh(std::istream& stream, OBJFile& file) const;
    
    /*! @brief Process all global markers. */
    bool processDefaultMarkers(char marker, char* line, OBJFile& file) const;
};

#endif // OBJMESHLOADER_OBJLOADER_H