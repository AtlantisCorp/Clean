/** \file GlDriver/GlBufferManager.cpp
**/

#include "GlBufferManager.h"

std::shared_ptr < GlBuffer > GlBufferManager::findByHandle(std::size_t handle)
{
    std::scoped_lock < std::mutex > lck(managedListMutex);
    auto it = std::find_if(managedList.begin(), managedList.end(), [handle](auto buffer) { return buffer->getHandle() == handle; });
    if (it != managedList.end()) return *it;
    return nullptr;
}