/** \file OBJMeshLoader/main.cpp
**/

#include "OBJLoader.h"

#include <Clean/Module.h>
#include <Clean/Core.h>
#include <Clean/Allocate.h>

void OBJLoaderStartModule(void)
{
    Clean::Core& core = Clean::Core::Get();
    core.addFileLoader < Clean::Mesh, OBJLoader >(Clean::AllocateShared < OBJLoader >());
}

void OBJLoaderStopModule(void)
{
    Clean::Core& core = Clean::Core::Get();
    auto loader = core.findFileLoaderByName < Clean::Mesh >("OBJMeshLoader");
    if (loader) core.removeFileLoader < Clean::Mesh >(loader);
}

Clean::ModuleInfos OBJLoaderModuleInfos = {
    .name = "OBJMeshLoader",
    .description = "Clean::MeshLoader for OBJ File format.",
    .author = "Luk2010",
    .version = Clean::Version::FromString("1.0"),
    
    .startCallback = &OBJLoaderStartModule,
    .stopCallback = &OBJLoaderStopModule
};

extern "C" Clean::ModuleInfos* GetFirstModuleInfos(void) 
{
    return &OBJLoaderModuleInfos;
}