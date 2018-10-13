/** \file OBJMeshLoader/OBJLoader.cpp
**/

#include "OBJLoader.h"

#include <Clean/FileSystem.h>
#include <Clean/NotificationCenter.h>
#include <Clean/Core.h>
using namespace Clean;

static constexpr const char kOBJMarkerComment = '#';
static constexpr const char kOBJMarkerNewObject = 'o';
static constexpr const char kOBJMarkerVertice = 'v';
static constexpr const char* kOBJMarkerNormal = "vn";
static constexpr const char* kOBJMarkerTexture = "vt";
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
    result.ver = result.tex = result.nor = 0;
    
    result.ver = static_cast < std::size_t >(std::strtol(line + current, &end, 10));
    if (*end == '/' && std::isdigit(*(end + 1))) result.tex = static_cast < std::size_t >(std::strtol(end + 1, &end, 10));
    if (*end == '/' && std::isdigit(*(end + 1))) result.nor = static_cast < std::size_t >(std::strtol(end + 1, &end, 10));
    else if (*end == '/' && *(end + 1) == '/' && std::isdigit(*(end + 2))) result.nor = static_cast < std::size_t >(std::strtol(end + 2, &end, 10));
    
    current = static_cast < size_t >(end - line);
    return result;
}

static OBJVec4 OBJMakeVec4(char* line)
{
    // A OBJ Vec4 is for vertices only. It is defined as x, y, z and w. But w is optional and
    // by default is 1.0. So we have 4 or 3 floats, separated by a space.
    OBJVec4 result = { 0.0f, 0.0f, 0.0f, 1.0f };
    char* current = line + 1;
    char* end = NULL;
    
    result.x = std::strtof(current, &end);
    if (end && current != end) result.y = std::strtof(end, &current);
    if (current && current != end) result.z = std::strtof(current, &end);
    // if (end && current != end) result.w = std::strtof(end, &current);
    
    return result;
}

static OBJVec3 OBJMakeVec3(char* line)
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
        return nullptr;
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
    if (file.meshes.empty()) return nullptr;
    
    std::shared_ptr < Mesh > result = convertOBJFile(file);
    return result;
}

bool OBJLoader::isLoadable(std::string const& extension) const
{
    return extension == "obj" || extension == "OBJ";
}

FileLoaderInfos OBJLoader::getInfos() const
{
    return
    {
        .name = "OBJMeshLoader",
        .description = "Loads OBJ File format meshes.",
        .authors = "Luk2010",
        .version = Version::FromString("1.0")
    };
}

OBJFile OBJLoader::makeOBJFile(std::istream& stream) const 
{
    char line[256];
    stream.getline(line, 256);
    OBJFile result;
    
    OBJMesh defaultMesh = makeOBJMesh(stream, result);
    if (!defaultMesh.faces.empty())
        result.meshes.push_back(defaultMesh);
    
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
        char marker2 = line[1];
        
        if (processDefaultMarkers(marker, marker2, line, file))
        {
            stream.getline(line, 256);
            continue;
        }
        
        else if (marker == kOBJMarkerFace) 
        {
            std::size_t currentVertice = 0;
            std::size_t current = 1;
            std::size_t max = strlen(line);
            
            OBJFace newFace;
            
            while (current < max && currentVertice < 3)
            {
                OBJFaceTriplet triplet = OBJMakeFaceTriplet(line, current);
                
                OBJVertex newVertex;
                newVertex.pos = { 0.0f, 0.0f, 0.0f, 0.0f };
                newVertex.tex = { {{0.0f, 0.0f, 0.0f}} };
                newVertex.nor = { {{0.0f, 0.0f, 0.0f}} };
                
                if (!triplet.ver)
                    break;
                
                if (triplet.ver) {
                    newVertex.pos = file.globVerts[triplet.ver - 1];
                }
                
                if (triplet.nor) {
                    newVertex.nor = file.globNorms[triplet.nor - 1];
                }
                
                if (triplet.tex) {
                    newVertex.tex = file.globTexts[triplet.tex - 1];
                }
                
                file.vertexes.push_back(newVertex);
                newFace.idx[currentVertice] = file.vertexes.size() - 1;
                currentVertice++;
            }
            
            result.faces.push_back(newFace);
        }
        
        else if (marker == kOBJMarkerNewObject)
        {
            if (result.faces.size())
            {
                file.meshes.push_back(result);
                result.faces.clear();
            }
        }
        
        stream.getline(line, 256);
    }
    
    return result;
}

bool OBJLoader::processDefaultMarkers(char marker, char marker2, char* line, OBJFile& file) const
{
    assert(line && "Null line given.");
    
    if (marker == kOBJMarkerComment)
    {
        file.comments.append(line);
        return true;
    }
    
    else if (marker == kOBJMarkerVertice && std::isspace(marker2))
    {
        OBJVec4 vert = OBJMakeVec4(line + 1);
        file.globVerts.push_back(vert);
        return true;
    }
    
    else if (marker == kOBJMarkerNormal[0] && marker2 == kOBJMarkerNormal[1])
    {
        OBJVec3 norm = OBJMakeVec3(line + 1);
        file.globNorms.push_back(norm);
        return true;
    }
    
    else if (marker == kOBJMarkerTexture[0] && marker2 == kOBJMarkerTexture[1])
    {
        OBJVec3 text = OBJMakeVec3(line + 1);
        file.globTexts.push_back(text);
        return true;
    }
    
    return false;
}

std::shared_ptr < Mesh > OBJLoader::convertOBJFile(OBJFile &file) const
{
    // We will organize the mesh like so:
    // - All vertexes will be in one shared buffer object. This buffer will contain all vertexes
    //   for the whole mesh.
    // - For each OBJMesh, we create an index buffer. This buffer will hold only the indexes for the
    //   Clean::SubMesh we want to create. Data is taken from OBJMesh data and references always the
    //   shared buffer.
    
    std::vector < std::shared_ptr < GenBuffer > > buffers;
    std::vector < SubMesh > submeshes;
    
    // const std::size_t typesize = sizeof(OBJVertex);
    const std::size_t dataSize = sizeof(OBJVertex) * file.vertexes.size();
    std::shared_ptr < GenBuffer > vbuffer = AllocateShared < GenBuffer >(file.vertexes.data(), dataSize);
    buffers.push_back(vbuffer);
    
    VertexDescriptor descriptor;
    descriptor.addComponent(kVertexComponentPosition, 0, sizeof(OBJVertex));
    descriptor.addComponent(kVertexComponentNormal, sizeof(OBJVec4), sizeof(OBJVertex));
    descriptor.addComponent(kVertexComponentTexture, sizeof(OBJVec4)+sizeof(OBJVec3), sizeof(OBJVertex));
    
    for (auto& mesh : file.meshes)
    {
        SubMesh submesh;
        submesh.offset = 0;
        submesh.elements = mesh.faces.size() * 3;
        submesh.buffer = vbuffer;
        
        submesh.indexOffset = 0;
        submesh.indexCount = mesh.faces.size() * 3;
        
        std::shared_ptr < GenBuffer > ibuffer = AllocateShared < GenBuffer >(mesh.faces.data(), sizeof(OBJFace) * mesh.faces.size(),
                                                                             kBufferUsageDynamic, kBufferTypeIndex);
        
        submesh.indexBuffer = ibuffer;
        buffers.push_back(ibuffer);
        
        submesh.descriptor = descriptor;
        submeshes.push_back(submesh);
    }
    
    std::shared_ptr < Mesh > result = AllocateShared < Mesh >();
    result->addBuffers(buffers);
    result->addSubMeshes(submeshes);
    
    return result;
}
