/** \file OBJMeshLoader/OBJLoader.cpp
**/

#include "OBJLoader.h"

#include <Clean/FileSystem.h>
#include <Clean/NotificationCenter.h>
using namespace Clean;

static constexpr const char kOBJMarkerComment = '#';
static constexpr const char kOBJMarkerNewObject = 'o';
static constexpr const char kOBJMarkerVertice = 'v';
static constexpr const char kOBJMarkerNormal = 'vn';
static constexpr const char kOBJMarkerTexture = 'vt';
static constexpr const char kOBJMarkerFace = 'f';

struct OBJFaceTriplet 
{
    std::size_t ver, tex, nor;
};

static OBJFaceTriplet OBJMakeFaceTriplet(const char* line, std::size_t& current) 
{
    // Computes our line's length.
    const std::size_t length = strlen(line);
    
    // First, skip all spaces. 
    while (current < length && std::isspace(line[current])) 
        current++;
    
    if (current == length)
        return { 0, 0, 0 };
    
    // Next, we have a triplet in the following formats: V, V/T, V/T/N or V//N. The three are valids
    // and used, so we must support them all. This algorithm must work because if first if is false, 
    // we are in case 1 or 4. If first if is true, we are in case 2 or 3. So, second if is true only 
    // in case 3, and third if (else if) is true only in case 4. So we have all our cases.
    
    char* end;
    OBJFaceTriplet result;
    result.pos = static_cast < std::size_t >(std::strtol(line + current, end, 10));
    if (*end == '/' && std::isdigit(*(end + 1))) result.tex = static_cast < std::size_t >(std::strtol(end + 1, end, 10));
    if (*end == '/' && std::isdigit(*(end + 1))) result.nor = static_cast < std::size_t >(std::strtol(end + 1, end, 10));
    else if (*end == '/' && *(end + 1) == '/' && std::isdigit(*(end + 2))) result.nor = static_cast < std::size_t >(std::strtol(end + 2, end, 10));
    
    current = static_cast < size_t >(end - line);
    return result;
}

static OBJVec4 OBJMakeVec4(const char* line) 
{
    // A OBJ Vec4 is for vertices only. It is defined as x, y, z and w. But w is optional and
    // by default is 1.0. So we have 4 or 3 floats, separated by a space.
    OBJVec4 result = { 0.0f, 0.0f, 0.0f, 0.0f };
    char* current = line + 1;
    char* end = NULL;
    
    result.x = std::strtof(current, &end);
    if (end && current != end) result.y = std::strtof(end, &current);
    if (current && current != end) result.z = std::strtof(current, &end);
    if (end && current != end) result.w = std::strtof(end, &current);
    
    return result;
}

static OBJVec3 OBJMakeVec3(const char* line)
{
    // A OBJ Vec3 is either a texture or a normal value. 
    OBJVec3 result; result.i = result.j = result.k = 0.0f;
    char* current = line + 1;
    char* end = NULL;
    
    result.i = std::strtof(current, &end);
    if (end && current != end) result.j = std::strtof(end, &current);
    if (current && current != end) result.k = std::strtof(current, &end);
    
    return result;
}

std::shared_ptr < Mesh > OBJLoader::load(std::string const& path) const
{
    // Path must be verified by using the Core's current filesystem. Our mesh could be in any of 
    // our resource's directories, but under the localized Mesh trees.
    FileSystem& currentFilesystem = Clean::Core::Get().getCurrentFileSystem();
    std::fstream stream = currentFilesystem.open(path, std::ios_base::in);
    
    if (!stream) {
        Notification notif = BuildNotification(kNotificationLevelError, 
            "File '%s' not found.", path.data());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    // Now we must go through the file character by character to have our Mesh object. 
    // A OBJ Mesh is constitued by: 
    // - v for Position Vertices.
    // - n for Normal Vertices.
    // - t for Texture Vertices.
    // - f for Faces. Faces includes an absolute position for the indexed vertexes. 
    
    // Materials should be loaded apart with another loader, MtlLoader included in this module.
    // Materials loads Textures, and default constants for the mesh. It may be used by any external
    // node manager to defines constants in the shader appropriately. 
    
    // An OBJ File may holds more than one object. Each object is converted to a Clean::SubMesh structure
    // so all OBJ File holds only one Clean::Mesh. However, those SubMesh will have the same shared buffers,
    // with indexed data pointing to the correct vertexes. 
    
    OBJFile file = makeOBJFile(stream);
    if (file.empty()) return nullptr;
    
    std::shared_ptr < Mesh > result = convertOBJFile(file);
    return result;
}

OBJFile OBJLoader::makeOBJFile(std::istream& stream) const 
{
    char line[256];
    stream.getline(line, 256);
    OBJFile result;
    
    while (!stream.eof())
    {
        char marker = line[0];
        
        if (processDefaultMarkers(marker, line, result))
        {
            stream.getline(line, 256);
            continue;
        }
        
        else if (marker == kOBJMarkerNewObject)
        {
            OBJMesh newMesh = makeOBJMesh(stream, result.vertexes);
            if (!newMesh.empty()) result.meshes.push_back(newMesh);
        }
        
        stream.getline(line, 256);
    }
    
    return result;
}

OBJMesh OBJLoader::makeOBJMesh(std::istream& stream, OBJFile& file) const
{
    // An OBJ Mesh is defined by multiple faces. When 'o' marker is encountered, all faces are given
    // to this mesh. However, v, vn and vt are still encountered and must be processed.
    
    char line[256];
    stream.getline(line, 256);
    OBJMesh result;
    
    while (!stream.eof())
    {
        char marker = line[0];
        
        if (processDefaultMarkers(marker, line, file))
        {
            stream.getline(line, 256);
            continue;
        }
        
        else if (marker == kOBJMarkerFace) 
        {
            std::size_t current = 1;
            std::size_t max = strlen(line);
            
            OBJFace newFace;
            
            while (current < max)
            {
                OBJFaceTriplet triplet = OBJMakeFaceTriplet(line, current);
                OBJVertex newVertex;
                
                if (!triplet.pos)
                    break;
                
                newVertex.pos = triplet.pos > 0 ? file.globVerts[triplet.pos] : file.globVerts[file.globVerts.size() - triplet.pos];
                newVertex.nor = triplet.nor > 0 ? file.globNorms[triplet.nor] : file.globNorms[file.globNorms.size() - triplet.nor];
                newVertex.tex = triplet.tex > 0 ? file.globTexts[triplet.tex] : file.globTexts[file.globTexts.size() - triplet.tex];
                
                file.vertexes.push_back(newVertex);
                newFace.idx.push_back(file.vertexes.size() - 1);
            }
            
            result.faces.push_back(newFace);
        }
        
        else if (marker == kOBJMarkerNewObject)
        {
            file.meshes.push_back(result);
            result.faces.clear();
        }
    }
}

bool OBJLoader::processDefaultMarkers(char marker, char* line, OBJFile& file) const
{
    assert(line && "Null line given.");
    
    if (marker == kOBJMarkerComment)
    {
        file.comments.append(line);
        return true;
    }
    
    else if (marker == kOBJMarkerVertice)
    {
        OBJVec4 vert = OBJMakeVec4(line + 1);
        file.globVerts.push_back(vert);
        return true;
    }
    
    else if (marker == kOBJMarkerNormal)
    {
        OBJVec3 norm = OBJMakeVec3(line + 1);
        file.globNorms.push_back(norm);
        return true;
    }
    
    else if (marker == kOBJMarkerTexture)
    {
        OBJVec3 text = OBJMakeVec3(line + 1);
        file.globTexts.push_back(text);
        return true;
    }
    
    return false;
}