/** \file Core/RenderQueue.h
**/

#ifndef CLEAN_RENDERQUEUE_H
#define CLEAN_RENDERQUEUE_H

#include "Handled.h"
#include "RenderCommand.h"

#include <cstdint>
#include <atomic>
#include <queue>
#include <mutex>

namespace Clean 
{
    static constexpr const std::uint8_t kRenderQueueTypeStatic = 0;
    static constexpr const std::uint8_t kRenderQueueTypeDynamic = 1;
    
    /** @brief Defines a queue of RenderCommand to be executed by a Driver. 
     *
     * A RenderQueue is only a list of commands pushed by one or more objects to be executed by the driver. If
     * the queue is static, those commands will be executed in the same order untill the driver clears the queue. 
     * If the queue is dynamic, for each frame, objects must re-push theirs RenderCommands in the queue. 
     *
     * A RenderQueue can group any RenderCommand. However, a good manager may group those commands in queues that
     * will be draw under different priorities. For example, transparent objects should be draw after opaque objects,
     * so their RenderCommand objects must be pushed in a queue that will have a lower priority than opaque objects. 
     *
    **/
    class RenderQueue : public Handled < RenderQueue >
    {
        //! @brief Queue type. Can be static (RenderCommands are not flushed when used) or dynamic
        //! (RenderCommands are flushed at each utilisation). A static RenderQueue is used to draw 
        //! objects that will never change. Dynamic queue are used to draw objects which will change
        //! in time, like animated objects. 
        std::atomic < std::uint8_t > type;
        
        //! @brief A queue of RenderCommand. When in static mode, each popped command is immediately
        //! pushed to the back of the queue. 
        std::queue < RenderCommand > commands;
        
        //! @brief Protects commands.
        std::mutex commandsMutex;
        
        //! @brief Number of commands commited to the queue but not already rendered by the Driver. 
        //! When a RenderCommand is added, commitedCommands is incremented by one. When \ref nextCommand
        //! is used, commitedCommands is decreased by one. When a Driver commits a RenderQueue, it will 
        //! process only this current number of commands. 
        std::atomic < std::size_t > commitedCommands;
        
    public:
        
        /*! @brief Constructs the queue. */
        RenderQueue(std::uint8_t type);
        
        /*! @brief Destructs the queue. */
        virtual ~RenderQueue() = default;
        
        /*! @brief Returns the queue's type. */
        std::uint8_t getType() const;
        
        /*! @brief Returns next RenderCommand in queue. */
        RenderCommand nextCommand();
        
        /*! @brief Adds a RenderCommand to the back of the queue. */
        virtual void addCommand(RenderCommand&& command);
        
        /*! @brief Releases queue's internal resource. */
        virtual void release() = 0;
        
        /*! @brief Returns true if queue is empty. */
        bool isEmpty() const;
        
        /*! @brief Returns commitedCommands. */
        std::size_t getCommitedCommands() const;
    };
}

#endif // CLEAN_RENDERQUEUE_H