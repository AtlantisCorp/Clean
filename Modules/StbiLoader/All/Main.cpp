/** 
 *  \file StbiLoader/Main.cpp
 *  \date 11/02/2018
**/

#include "STBILoader.h"

#include <Clean/Module.h>
#include <Clean/Core.h>
#include <Clean/Allocate.h>

void STBILoaderStartModule(void)
{
    Clean::Core& core = Clean::Core::Get();
    core.addFileLoader < Clean::Image, STBILoader >(Clean::AllocateShared < STBILoader >());
}

void STBILoaderStopModule(void)
{
    Clean::Core& core = Clean::Core::Get();
    
    auto loader = core.findFileLoaderByName < Clean::Image >("STBILoader");
    if (loader) core.removeFileLoader < Clean::Image >(loader);
}

Clean::ModuleInfos STBILoaderModuleInfos = {
    .name = "STBILoader",
    .description = "FileLoader for any Image File Formats.",
    .author = "Luk2010",
    .version = Clean::Version::FromString("1.0"),
    
    .startCallback = &STBILoaderStartModule,
    .stopCallback = &STBILoaderStopModule
};

extern "C" Clean::ModuleInfos* GetFirstModuleInfos(void) 
{
    return &STBILoaderModuleInfos;
}
