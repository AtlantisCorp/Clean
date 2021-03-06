/** \file GlDriver/GlShaderManager.cpp
**/

#include "GlShaderManager.h"

std::shared_ptr < GlShader > GlShaderManager::findByHandle(std::size_t handle) const 
{
    std::scoped_lock < std::mutex > lck(managedListMutex);
    auto it = std::find_if(managedList.begin(), managedList.end(), [handle](auto shader) { return shader->getHandle() == handle; });
    if (it == managedList.end()) return nullptr;
    return *it;
}

std::shared_ptr < GlShader > GlShaderManager::findByPath(std::string const& origin) const
{
    std::scoped_lock < std::mutex > lck(managedListMutex);
    auto it = std::find_if(managedList.begin(), managedList.end(), [origin](auto shader) { return shader->getOriginPath() == origin; });
    if (it == managedList.end()) return nullptr;
    return *it;
}