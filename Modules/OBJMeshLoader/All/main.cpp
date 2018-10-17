/** \file OBJMeshLoader/main.cpp
**/

#include "OBJLoader.h"
#include "MtlLoader.h"

#include <Clean/Module.h>
#include <Clean/Core.h>
#include <Clean/Allocate.h>

void OBJLoaderStartModule(void)
{
    Clean::Core& core = Clean::Core::Get();
    core.addFileLoader < Clean::Mesh, OBJLoader >(Clean::AllocateShared < OBJLoader >());
    core.addFileLoader < Clean::Material, MtlLoader >(Clean::AllocateShared < MtlLoader >());
}

void OBJLoaderStopModule(void)
{
    Clean::Core& core = Clean::Core::Get();
    
    auto loader = core.findFileLoaderByName < Clean::Mesh >("OBJMeshLoader");
    if (loader) core.removeFileLoader < Clean::Mesh >(loader);
    
    auto mtlloader = core.findFileLoaderByName < Clean::Material >("MtlLoader");
    if (mtlloader) core.removeFileLoader < Clean::Material >(mtlloader);
}

Clean::ModuleInfos OBJLoaderModuleInfos = {
    .name = "OBJMeshLoader",
    .description = "Clean::MeshLoader for OBJ and MTL File formats.",
    .author = "Luk2010",
    .version = Clean::Version::FromString("1.0"),
    
    .startCallback = &OBJLoaderStartModule,
    .stopCallback = &OBJLoaderStopModule
};

extern "C" Clean::ModuleInfos* GetFirstModuleInfos(void) 
{
    return &OBJLoaderModuleInfos;
}