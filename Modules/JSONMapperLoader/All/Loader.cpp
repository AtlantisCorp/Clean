/** **********************************************
 *  \file JSONMapperLoader/Loader.cpp
 *  \date 10/23/2018
 *  \author Luk2010
**/

#include "Loader.h"

#include <rapidjson/document.h>

#include <Clean/BuildableShaderMapper.h>
#include <Clean/Version.h>
#include <Clean/FileSystem.h>
#include <Clean/Platform.h>
#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
#include <Clean/Hash.h>

#include <Clean/VertexDescriptor.h>
#include <Clean/ShaderAttribute.h>
#include <Clean/EffectParameter.h>

using namespace Clean;

bool JSONMapperLoader::isLoadable(std::string const& extension) const 
{
    return extension == "json";
}

Clean::FileLoaderInfos JSONMapperLoader::getInfos() const 
{
    return 
    {
        .name = "JSONMapperLoader",
        .description = "JSON to ShaderMapper loader.",
        .authors = "Luk2010",
        .version = Version::FromString("1.0")
    };
}

std::shared_ptr < Clean::ShaderMapper > JSONMapperLoader::load(std::string const& file) const
{
    std::fstream stream = FileSystem::Current().open(file, std::ios::in);
    std::string fileContent; Platform::StreamGetContent(stream, fileContent);
    stream.close();
    
    if (fileContent.empty()) {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "File %s not found or empty.", file.data()));
        return nullptr;
    }
    
    std::shared_ptr < BuildableShaderMapper > result = AllocateShared < BuildableShaderMapper >();
    rapidjson::Document document;
    
    document.Parse(fileContent.data());
    assert(document.IsObject() && "JSON Document is not an object.");
    
    for (auto it = document.MemberBegin(); it != document.MemberEnd(); it++)
    {
        auto& member = *it;
        
        if (member.name == "Name" && member.value.IsString())
        {
            result->setName(member.value.GetString());
            continue;
        }
        
        else if (member.name == "Version" && member.value.IsString())
        {
            assert(kCurrentJSONMapperVersion == Version::FromString(member.value.GetString()) && "Invalid Version.");
            continue;
        }
        
        else if (member.name == "Attributes")
        {
            assert(member.value.IsObject() && "'Attributes' must be a JSON object.");
            
            for (auto attrib = member.value.MemberBegin(); attrib != member.value.MemberEnd(); attrib++)
            {
                auto& attribMember = *attrib;
                BuildableShaderMapper::Attribute attribute;
                attribute.name = attribMember.name.GetString();
                
                std::string meaningComponent = attribMember.value.FindMember("name")->value.GetString();
                attribute.vertexComponent = VertexComponentFromString(meaningComponent);
                assert(attribute.vertexComponent && "Attribute is not bound to any VertexComponent.");
                
                std::string type = attribMember.value.FindMember("type")->value.GetString();
                attribute.type = (type.empty()) ? VertexComponentGetShaderAttribType(attribute.vertexComponent) : ShaderAttribTypeFromString(type);
                
                std::int32_t index = (attribMember.value.HasMember("index")) ? attribMember.value.FindMember("index")->value.GetInt() : -1;
                attribute.index = static_cast < std::int8_t >(index);
                
                std::uint32_t components = (attribMember.value.HasMember("components")) ? attribMember.value.FindMember("components")->value.GetInt() : 0;
                attribute.components = static_cast < std::uint8_t >(components);
                
                if (!attribute.components) 
                    attribute.components = VertexComponentCount(attribute.vertexComponent);
                
                if (attribute.components && attribute.vertexComponent)
                    result->addAttribute(attribute);
            }
        }
        
        else if (member.name == "Constants")
        {
            assert(member.value.IsObject() && "'Constants' must be a JSON object.");
            
            for (auto cst = member.value.MemberBegin(); cst != member.value.MemberEnd(); cst++)
            {
                auto& cstMember = *cst;
                BuildableShaderMapper::Constant constant;
                constant.name = cstMember.name.GetString();
                
                std::string param = cstMember.value.FindMember("name")->value.GetString();
                assert(!param.empty() && "Constant must have a 'name' field.");
                std::uint64_t hash = Hash64(param.data());
                constant.hash = hash;
                
                if (!cstMember.value.HasMember("type"))
                    constant.type = EffectParameterGetTypeFromHash(hash);
                else 
                    constant.type = ShaderParameterFromString(cstMember.value.FindMember("type")->value.GetString());
                
                if (!cstMember.value.HasMember("index"))
                    constant.index = -1;
                else 
                    constant.index = static_cast < std::int8_t >(cstMember.value.FindMember("index")->value.GetInt());
                
                assert(constant.type && "Invalid Constant Type.");
                result->addConstant(constant);
            }
        }
    }
    
    return result;
}
