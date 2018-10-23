/*! ================================================
 *  \file OBJMeshLoader/MtlLoader.cpp
 *  \date 10/17/2018
 *  \author luk2010
**/

#include "MtlLoader.h"

#include <Clean/NotificationCenter.h>
#include <Clean/Core.h>
#include <Clean/Allocate.h>
#include <Clean/Platform.h>
using namespace Clean;

bool MtlLoader::isLoadable(std::string const& extension) const 
{
    return extension == "mtl" || extension == "MTL";
}

FileLoaderInfos MtlLoader::getInfos() const 
{
    return {
        .name = "MtlLoader",
        .description = "Mtl Lightwave File Loader",
        .authors = "luk2010",
        .version = Version::FromString("1.0")
    };
}

std::vector < std::shared_ptr < Material > > MtlLoader::load(std::string const& filepath) const
{
    if (filepath.empty())
        return {};
    
    std::string realPath;
    std::fstream stream = Core::Get().getCurrentFileSystem().open(filepath, std::ios::in, &realPath);
    
    if (!stream) {
        Notification notif = BuildNotification(kNotificationLevelError, "File '%s' not found.", filepath.data());
        NotificationCenter::GetDefault()->send(notif);
        return {};
    }
    
    std::string fileContent;
    Platform::StreamGetContent(stream, fileContent);
    stream.close();
    
    if (fileContent.empty()) {
        Notification notif = BuildNotification(kNotificationLevelError, "File '%s' is empty.", filepath.data());
        NotificationCenter::GetDefault()->send(notif);
        return {};
    }
    
    std::stringstream cstream(fileContent);
    std::vector < std::shared_ptr < Material > > result;
    std::size_t lineCount = 0;
    
    do 
    {
        auto material = findNextMaterial(cstream, lineCount);
        if (!material) break;
        
        result.push_back(material);
        
    } while(!cstream.eof());
    
    return result;
}

std::shared_ptr < Material > MtlLoader::findNextMaterial(std::istream& stream, std::size_t& lineCount) const 
{
    static const char* kMtlMarkerMaterial = "newmtl";
    static const char* kMtlMarkerKa = "Ka";
    static const char* kMtlMarkerKd = "Kd";
    static const char* kMtlMarkerKs = "Ks";
    static const char* kMtlMarkerKe = "Ke";
    
    std::size_t oldPosition = stream.tellg();
    std::shared_ptr < Material > material = nullptr;
    
    for (std::string line; std::getline(stream, line); lineCount++)
    {
        std::stringstream lineStream(line);
        
        for (std::string word; lineStream >> word;)
        {
            if (!strcmp(word.data(), kMtlMarkerMaterial))
            {
                if (material) 
                {
                    // We encountered a new material so break this function and return our material.
                    // Because this function only fill one material.
                    stream.seekg(oldPosition, std::ios::beg);
                    lineCount--;
                    return material;
                }
                
                // We encountered a new material and we havn't one yet so create it.
                lineStream >> word;
                material = AllocateShared < Material >(word);
            }
            
            else if (!strcmp(word.data(), kMtlMarkerKa))
            {
                if (!material) {
                    Notification notif = BuildNotification(kNotificationLevelError, 
                        "Error parsing Mtl file at line %i: %s was used out of a Material context.", 
                        lineCount, word.data());
                    NotificationCenter::GetDefault()->send(notif);
                    continue;
                }
                
                // Ambient color format: Ka r g b
                
                float r, g, b;
                lineStream >> r >> g >> b;
                material->setAmbientColor({r, g, b, 1.0f});
            }
            
            else if (!strcmp(word.data(), kMtlMarkerKd))
            {
                if (!material) {
                    Notification notif = BuildNotification(kNotificationLevelError, 
                        "Error parsing Mtl file at line %i: %s was used out of a Material context.", 
                        lineCount, word.data());
                    NotificationCenter::GetDefault()->send(notif);
                    continue;
                }
                
                // Diffuse color format: Kd r g b
                
                float r, g, b;
                lineStream >> r >> g >> b;
                material->setDiffuseColor({r, g, b, 1.0f});
            }
            
            else if (!strcmp(word.data(), kMtlMarkerKs))
            {
                if (!material) {
                    Notification notif = BuildNotification(kNotificationLevelError, 
                        "Error parsing Mtl file at line %i: %s was used out of a Material context.", 
                        lineCount, word.data());
                    NotificationCenter::GetDefault()->send(notif);
                    continue;
                }
                
                // Specular color format: Ks r g b
                
                float r, g, b;
                lineStream >> r >> g >> b;
                material->setSpecularColor({r, g, b, 1.0f});
            }
            
            else if (!strcmp(word.data(), kMtlMarkerKe))
            {
                if (!material) {
                    Notification notif = BuildNotification(kNotificationLevelError, 
                        "Error parsing Mtl file at line %i: %s was used out of a Material context.", 
                        lineCount, word.data());
                    NotificationCenter::GetDefault()->send(notif);
                    continue;
                }
                
                // Emissive color format: Ke r g b
                
                float r, g, b;
                lineStream >> r >> g >> b;
                material->setEmissiveColor({r, g, b, 1.0f});
            }
            
            else 
            {
                Notification notif = BuildNotification(kNotificationLevelWarning, 
                    "Error parsing Mtl file at line %i: %s keyword is not interpreted by this loader.", 
                    lineCount, word.data());
                NotificationCenter::GetDefault()->send(notif);
                continue;
            }
        }
        
        oldPosition = stream.tellg();
    }
    
    return material;
}
