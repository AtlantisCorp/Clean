/** **********************************************
 *  \file JSONMapperLoader/Main.cpp
 *  \date 10/23/2018
 *  \author Luk2010
**/

#include <Clean/Core.h>
#include <Clean/Module.h>

void RegisterLoader(void)
{
    Clean::Core& core = Clean::Core::Get();
    // core.addFileLoader < Clean::ShaderMapper, JSONMapperLoader >(Clean::AllocateShared < JSONMapperLoader >());
}

void UnregisterLoader(void)
{
    Clean::Core& core = Clean::Core::Get();
    
    auto loader = core.findFileLoaderByName < Clean::ShaderMapper >("JSONMapperLoader");
    assert(loader && "Null FileLoader found.");
    core.removeFileLoader(loader);
}

Clean::ModuleInfos JSONMapperModule = {
    .name = "JSONMapperLoader",
    .description = "JSON file to Clean::ShaderMapper.",
    .author = "Luk2010",
    .version = Clean::Version::FromString("1.0"),
    
    .startCallback = &RegisterLoader,
    .stopCallback = &UnregisterLoader
};

extern "C" Clean::ModuleInfos* GetFirstModuleInfos(void)
{
    return &JSONMapperModule;
}
