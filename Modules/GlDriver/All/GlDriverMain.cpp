/** GlDriver/All/GlDriverMain.cpp
**/

#include "GlDriver.h"

#include <Clean/Module.h>
#include <Clean/Core.h>
#include <Clean/Allocate.h>

void GlDriverStartModule()
{
    Clean::Core& core = Clean::Core::Get();
    core.addDriver(Clean::AllocateShared < GlDriver >());
}

void GlDriverStopModule()
{
    
}

Clean::ModuleInfos GlDriverModuleInfos = {
    .name = "Clean.GlDriver",
    .description = "Clean::Driver implementation for OpenGL detected API.",
    .author = "Luk2010",
    .version = Clean::Version::FromString("1.0"),
    
    .startCallback = &GlDriverStartModule,
    .stopCallback = &GlDriverStopModule
};

extern "C" Clean::ModuleInfos* GetFirstModuleInfos() 
{
    return &GlDriverModuleInfos;
}
