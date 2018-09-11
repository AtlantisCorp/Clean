/** \file Core/RenderQueue.cpp
**/

#include "RenderQueue.h"

namespace Clean 
{
    RenderQueue::RenderQueue(std::uint8_t t) 
    {
        type.store(t);
    }
    
    std::uint8_t RenderQueue::getType() const 
    {
        return type.load();
    }
    
    RenderCommand RenderQueue::nextCommand() 
    {
        std::uint8_t t = type.load();
        
        if (t == kRenderQueueTypeStatic)
        {
            commandsMutex.lock();
            RenderCommand command = std::move(commands.front());
            commands.pop();
            commandsMutex.unlock();
            
            commands.push(command);
            return std::move(command);
        }
        
        else if (t == kRenderQueueTypeDynamic)
        {
            std::scoped_lock < std::mutex > lck(commandsMutex);
            RenderCommand command = std::move(commands.front());
            commands.pop();
            return std::move(command);
        }
        
        throw IllformedConstantException("kRenderQueueType* ill-formed.");
    }
    
    void RenderQueue::addCommand(RenderCommand&& command)
    {
        std::scoped_lock < std::mutex > lck(commandsMutex);
        commands.push(std::move(command));
    }
    
    bool RenderQueue::isEmpty() const 
    {
        std::scoped_lock < std::mutex > lck(commandsMutex);
        return commands.empty();
    }
}