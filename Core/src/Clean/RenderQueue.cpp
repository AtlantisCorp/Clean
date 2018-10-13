/** \file Core/RenderQueue.cpp
**/

#include "RenderQueue.h"
#include "Exception.h"

namespace Clean 
{
    RenderQueue::RenderQueue(std::uint8_t t) 
    {
        type.store(t);
        commitedCommands.store(0);
    }
    
    std::uint8_t RenderQueue::getType() const 
    {
        return type.load();
    }
    
    RenderCommand RenderQueue::nextCommand() 
    {
        std::uint8_t t = type.load();
        
        if (t == kRenderQueueStatic)
        {
            // NOTES: In static mode, the RenderCommand is removed and re-inserted. commitedCommands
            // does not change during the operation to economize two atomic operations (-1 +1).
            
            commandsMutex.lock();
            RenderCommand command = commands.front();
            commands.pop();
            commands.push(command);
            commandsMutex.unlock();
            
            return command;
        }
        
        else if (t == kRenderQueueDynamic)
        {
            std::scoped_lock < std::mutex > lck(commandsMutex);
            commitedCommands.fetch_sub(1);
            RenderCommand command = std::move(commands.front());
            commands.pop();
            return command;
        }
        
        throw IllformedConstantException("kRenderQueueType* ill-formed.");
    }
    
    void RenderQueue::addCommand(RenderCommand const& command)
    {
        std::scoped_lock < std::mutex > lck(commandsMutex);
        commands.push(command);
        commitedCommands.fetch_add(1);
    }
    
    bool RenderQueue::isEmpty() const 
    {
        std::scoped_lock < std::mutex > lck(commandsMutex);
        return commands.empty();
    }
    
    std::size_t RenderQueue::getCommitedCommands() const 
    {
        return commitedCommands.load();
    }
}
