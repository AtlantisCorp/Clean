/** \file Core/Mesh.cpp
**/

#include "Mesh.h"
#include "RenderPipeline.h"
#include "RenderCommand.h"
#include "NotificationCenter.h"
#include "Driver.h"

namespace Clean
{
    struct MeshTransactionAddBuffer 
    {
        std::shared_ptr < Buffer > buffer;
        std::uint8_t type;
    };
    
    typedef MeshTransactionAddBuffer MeshTransactionUpdateBuffer;
    
    struct MeshTransactionBatchAddBuffers 
    {
        std::map < std::shared_ptr < Buffer >, std::uint8_t > buffers;
    };
    
    struct MeshTransactionBatchUpdateBuffers 
    {
        std::vector < std::shared_ptr < Buffer > > buffers;
    };
    
    BufferAutorelease::BufferAutorelease(std::shared_ptr < Buffer > const& rhs) : std::shared_ptr<Buffer>(rhs)
    {
        rhs->retain();
    }
    
    BufferAutorelease::BufferAutorelease(BufferAutorelease const& rhs) : std::shared_ptr<Buffer>(rhs)
    {
        rhs->retain();
    }
    
    BufferAutorelease::BufferAutorelease(BufferAutorelease&& rhs) : std::shared_ptr<Buffer>(std::move(rhs))
    {
        
    }
    
    BufferAutorelease::~BufferAutorelease()
    {
        auto ptr = get();
        if (ptr) ptr->release();
    }
    
    Mesh::Mesh() : drawingMethod(kDrawingMethodFilled)
    {
        
    }
    
    std::vector < VertexDescriptor > Mesh::findAssociatedDescriptors(Driver const& driver) const
    {
        const std::uintptr_t driverOffset = reinterpret_cast < std::uintptr_t >(&driver);
        std::scoped_lock < std::mutex, std::mutex > lck(submeshesMutex, driverCachesMutex);
        auto const& cacheIt = driverCaches.find(driverOffset);
        std::vector < VertexDescriptor > result;
        
        for (auto& submesh : submeshes)
        {
            assert(submesh.buffer && "No GenBuffer associated to SubMesh.");
            auto const& descriptor = submesh.descriptor;
            
            VertexDescriptor finalDescriptor = descriptor;
            finalDescriptor.localSubmesh.buffer = submesh.buffer;
            finalDescriptor.localSubmesh.offset = submesh.offset;
            finalDescriptor.localSubmesh.elements = submesh.elements;
            finalDescriptor.indexInfos = IndexedInfos(submesh.indexOffset, submesh.indexCount, submesh.indexBuffer);
            
            if (cacheIt != driverCaches.end()) {
                auto hardBufferIt = cacheIt->second.buffers.find(submesh.buffer->getHandle());
                if (hardBufferIt != cacheIt->second.buffers.end()) {
                    finalDescriptor.localSubmesh.buffer = hardBufferIt->second;
                }
                
                if (submesh.indexBuffer) {
                    hardBufferIt = cacheIt->second.buffers.find(submesh.indexBuffer->getHandle());
                    if (hardBufferIt != cacheIt->second.buffers.end()) {
                        finalDescriptor.indexInfos.buffer = hardBufferIt->second;
                    }
                }
            }
            
            result.push_back(finalDescriptor);
        }
        
        return result;
    }
    
    std::vector < VertexDescriptor > Mesh::findDescriptors() const
    {
        std::scoped_lock < std::mutex > lck(submeshesMutex);
        std::vector < VertexDescriptor > result;
        
        for (auto& submesh : submeshes)
        {
            assert(submesh.buffer && "No GenBuffer associated to SubMesh.");
            auto const& descriptor = submesh.descriptor;
            
            VertexDescriptor finalDescriptor = descriptor;
            finalDescriptor.localSubmesh.buffer = submesh.buffer;
            finalDescriptor.localSubmesh.offset = submesh.offset;
            finalDescriptor.localSubmesh.elements = submesh.elements;
            finalDescriptor.indexInfos = IndexedInfos(submesh.indexOffset, submesh.indexCount, submesh.indexBuffer);
            result.push_back(finalDescriptor);
        }
        
        return result;
    }
    
    void Mesh::populateRenderCommand(Driver const& driver, RenderCommand& command)
    {
        // When we must submit a render command, first try to see if we have pregenerated ShaderAttributesMap
        // for the pair driver/shader. 
        
        RenderPipeline const& shader = *(command.pipeline);
        shader.bind(driver);
        auto attribs = findShaderAttributesMap(driver, shader);
        
        if (!attribs.empty())
        {
            // Populate our RenderSubCommand with those infos. 
            command.batchSub(drawingMethod.load(), attribs);
            return;
        }
        
        // Tries to generate ShaderAttributesMap for each submesh. 
        
        auto descriptors = findAssociatedDescriptors(driver);
        attribs = shader.map(descriptors);
        
        if (attribs.empty())
        {
            // ShaderMapper couldn't map our VertexDescriptors to ShaderAttribute. This is an error
            // and we must log it. However, this may be sent too much times, so do this only once.
            static std::once_flag flag;
            std::call_once(flag, [this](){
                Notification errNotif = BuildNotification(kNotificationLevelError,
                    "Mesh #%i: ShaderMapper::map() couldn't generate ShaderAttributesMap.",
                    getHandle());
                NotificationCenter::GetDefault()->send(errNotif);
            });
            
            return;
        }
        
        // Stores our new ShaderCache. 
        ShaderCache newCache = { attribs };
        shaderCacheStore(driver, shader, newCache);
        
        // Populate our RenderSubCommand with those infos. 
        command.batchSub(drawingMethod.load(), attribs);
    }
    
    std::vector < ShaderAttributesMap > Mesh::findShaderAttributesMap(Driver const& driver, RenderPipeline const& shader)
    {
        const std::uintptr_t driverOffset = reinterpret_cast < std::uintptr_t >(&driver);
        const std::uintptr_t shaderOffset = reinterpret_cast < std::uintptr_t >(&shader);
        std::vector < ShaderAttributesMap > result;
        
        std::scoped_lock < std::mutex > lck(driverCachesMutex);
        auto driverIt = driverCaches.find(driverOffset);
        if (driverIt == driverCaches.end()) return result;
        
        auto shaderIt = driverIt->second.shaderCaches.find(shaderOffset);
        if (shaderIt == driverIt->second.shaderCaches.end()) return result;
        
        return shaderIt->second.shaderAttribs;
    }
    
    void Mesh::shaderCacheStore(Driver const& driver, RenderPipeline const& shader, ShaderCache& cache)
    {
        const std::uintptr_t driverOffset = reinterpret_cast < std::uintptr_t >(&driver);
        const std::uintptr_t shaderOffset = reinterpret_cast < std::uintptr_t >(&shader);
        std::vector < ShaderAttributesMap > result;
        
        std::scoped_lock < std::mutex > lck(driverCachesMutex);
        auto driverIt = driverCaches.find(driverOffset);
        if (driverIt != driverCaches.end()) driverIt->second.shaderCaches[shaderOffset] = cache;
    }
    
    void Mesh::associate(Driver& driver)
    {
        const std::uintptr_t driverOffset = reinterpret_cast < std::uintptr_t >(&driver);
        
        {
            std::lock_guard < std::mutex > lck(driverCachesMutex);
            auto driverCacheIt = driverCaches.find(driverOffset);
        
            if (driverCacheIt != driverCaches.end()) {
                Notification warnNotif = BuildNotification(kNotificationLevelWarning,
                    "Driver %s was already associated to Mesh #%i.", 
                    driver.getName().data(), getHandle());
                NotificationCenter::GetDefault()->send(warnNotif);
                return;
            }
        }
        
        DriverCache cache;
        
        // For each buffer, do the same as kMeshTransactionAddBuffer.
        
        {
            std::lock_guard < std::mutex > lck(buffersMutex);
            
            for (auto const& buffer : vertexBuffers)
            {
                auto hardBuffer = driver.makeBuffer(buffer.second->getType(), buffer.second);
                if (!hardBuffer) {
                    Notification softNotif = BuildNotification(kNotificationLevelWarning, 
                        "Driver %s can't make Hardware Buffer of size %i.", 
                        driver.getName().data(), buffer.second->getSize());
                    NotificationCenter::GetDefault()->send(softNotif);
                    hardBuffer = buffer.second;
                }
                
                // hardBuffer->retain();
                cache.buffers.insert(std::make_pair(buffer.first, BufferAutorelease(hardBuffer)));
            }
            
            for (auto const& buffer : indexBuffers)
            {
                auto hardBuffer = driver.makeBuffer(buffer.second->getType(), buffer.second);
                if (!hardBuffer) {
                    Notification softNotif = BuildNotification(kNotificationLevelWarning, 
                        "Driver %s can't make Hardware Buffer of size %i.", 
                        driver.getName().data(), buffer.second->getSize());
                    NotificationCenter::GetDefault()->send(softNotif);
                    hardBuffer = buffer.second;
                }
                
                // hardBuffer->retain();
                cache.buffers.insert(std::make_pair(buffer.first, BufferAutorelease(hardBuffer)));
            }
        }
        
        {
            std::lock_guard < std::mutex > lck(driverCachesMutex);
            driverCaches.insert(std::make_pair(driverOffset, std::move(cache)));
        }
    }
    
    void Mesh::update(Driver& driver, std::chrono::milliseconds maxTime)
    { 
        using namespace std::chrono; 
        auto tnow = high_resolution_clock::now();
        milliseconds elapsed = duration_cast < milliseconds >(high_resolution_clock::now() - tnow);
        
        std::scoped_lock < std::mutex > lck0(driverCachesMutex);
        const std::uintptr_t driverOffset = reinterpret_cast < std::uintptr_t >(&driver);
        auto cacheIt = driverCaches.find(driverOffset);
        
        if (cacheIt == driverCaches.end())
        {
            Notification errorNotif = BuildNotificationAll(kNotificationLevelWarning, __FUNCTION__, __FILE__,
                "Mesh::update() called from a Driver that is not associated to this mesh.");
            NotificationCenter::GetDefault()->send(errorNotif);
        }
        
        auto& driverQueue = cacheIt->second.transactions;
        auto& cache = cacheIt->second;
        
        while (!driverQueue.empty() || elapsed < maxTime)
        {
            Transaction tr = std::move(driverQueue.front());
            driverQueue.pop();
            
            if (!tr.valid())
                continue;
        
            // Adding/Removing a submesh means we only needs to clear the Shader cache associated to those submeshes.
            // Cache will be rebuilt just after when building RenderCommands or using ::findShaderAttributesMap(driver, shader).
            
            if (tr.type() == kMeshTransactionAddSubMesh || tr.type() == kMeshTransactionRemoveSubMesh) 
            {
                cache.shaderCaches.clear();
            }
            
            // Adding a buffer makes us create a new buffer from Driver and insert it inside DriverCache.
        
            else if (tr.type() == kMeshTransactionAddBuffer)
            {
                MeshTransactionAddBuffer* data = static_cast < MeshTransactionAddBuffer* >(tr.data());
                assert(data && "Null MeshTransactionAddBuffer.");
                assert(data->buffer && "Null GenBuffer for MeshTransactionAddBuffer.");
                    
                auto hardBuffer = driver.makeBuffer(data->type, data->buffer);
                if (!hardBuffer) {
                    Notification softNotif = BuildNotification(kNotificationLevelWarning, 
                        "Driver %s can't make Hardware Buffer of size %i.", 
                        driver.getName().data(), data->buffer->getSize());
                    NotificationCenter::GetDefault()->send(softNotif);
                    hardBuffer = data->buffer;
                }
                
                hardBuffer->retain();
                cache.buffers.insert(std::make_pair(data->buffer->getHandle(), hardBuffer));
            }
            
            // Updating a buffer is only updating with new data pre-existing buffer. 
        
            else if (tr.type() == kMeshTransactionUpdateBuffer)
            {
                MeshTransactionUpdateBuffer* data = static_cast < MeshTransactionUpdateBuffer* >(tr.data());
                assert(data && "Null MeshTransactionUpdateBuffer.");
                assert(data->buffer && "Null GenBuffer for MeshTransactionUpdateBuffer.");
                
                std::scoped_lock < std::mutex > lck(driverCachesMutex);
                auto hardBuffer = cache.buffers.find(data->buffer->getHandle())->second;
                
                if (!hardBuffer) {
                    Notification errNotif = BuildNotification(kNotificationLevelError,
                        "Buffer handle %i can't be found in Mesh's cache.",
                        data->buffer->getHandle());
                    NotificationCenter::GetDefault()->send(errNotif);
                }
                
                else if (hardBuffer != data->buffer) {
                    hardBuffer->update(data->buffer->getData(), data->buffer->getSize(), data->buffer->getUsage());
                }
            }
            
            // kMeshTransactionBatchAddBuffers adds multiple buffers in once. 
            
            else if (tr.type() == kMeshTransactionBatchAddBuffers)
            {
                MeshTransactionBatchAddBuffers* data = static_cast < MeshTransactionBatchAddBuffers* >(tr.data());
                assert(data && "Null MeshTransactionBatchAddBuffers.");
                
                std::scoped_lock < std::mutex > lck(driverCachesMutex);
                
                for (auto const& pair : data->buffers)
                {
                    assert(pair.first && "Null Buffer given.");
                    auto hardBufferIt = cache.buffers.find(pair.first->getHandle());
                    
                    if (hardBufferIt == cache.buffers.end()) {
                        auto hardBuffer = driver.makeBuffer(pair.second, pair.first);
                        if (!hardBuffer) {
                            Notification softNotif = BuildNotification(kNotificationLevelWarning, 
                                "Driver %s can't make Hardware Buffer of size %i.", 
                                driver.getName().data(), pair.first->getSize());
                            NotificationCenter::GetDefault()->send(softNotif);
                            hardBuffer = pair.first;
                        }
                        
                        hardBuffer->retain();
                        cache.buffers.insert(std::make_pair(pair.first->getHandle(), hardBuffer));
                    }
                    
                    // If hardBufferIt is not end(), this means we already created this buffer. So
                    // we do nothing. CQFD
                }
            }
            
            else if (tr.type() == kMeshTransactionBatchUpdateBuffers)
            {
                MeshTransactionBatchUpdateBuffers* data = static_cast < MeshTransactionBatchUpdateBuffers* >(tr.data());
                assert(data && "Null MeshTransactionBatchUpdateBuffers.");
                
                std::scoped_lock < std::mutex > lck(driverCachesMutex);
                
                for (auto const& buffer : data->buffers)
                {
                    assert(buffer && "Null Buffer given for update of Mesh.");
                    std::uint8_t const handle = buffer->getHandle();
                    
                    auto hardBuffer = cache.buffers.find(handle)->second;
                
                    if (!hardBuffer) {
                        Notification errNotif = BuildNotification(kNotificationLevelError,
                            "Buffer handle %i can't be found in Mesh's cache.",
                            handle);
                        NotificationCenter::GetDefault()->send(errNotif);
                    }
                
                    else if (hardBuffer != buffer) {
                        hardBuffer->update(buffer->getData(), buffer->getSize(), buffer->getUsage());
                    }
                }
            }
            
            elapsed = duration_cast < milliseconds >(high_resolution_clock::now() - tnow);
        }
        
        // If we are here, this means either TransactionQueue is empty or elapsedTime is > to maxTime. 
        // Notes that Transactions may have a maxTime which will make them ignored if this time is
        // reached. Some updates might fail if those Transactions are ignored.
    }
    
    void Mesh::addSubMesh(SubMesh&& submesh)
    {
        std::scoped_lock < std::mutex > lck(submeshesMutex);
        submeshes.push_back(std::move(submesh));
        submitTransaction(kMeshTransactionAddSubMesh);
    }
    
    void Mesh::addBuffers(std::vector < std::shared_ptr < GenBuffer > > const& buffers)
    {
        MeshTransactionBatchAddBuffers tr;
        
        {
            std::lock_guard < std::mutex > lck(buffersMutex);
        
            for (auto& buffer : buffers)
            {
                if (buffer)
                {
                    if (buffer->getType() == kBufferTypeVertex)
                    {
                        auto buf = std::static_pointer_cast<Buffer>(buffer);
                        vertexBuffers.insert(std::make_pair(buffer->getHandle(), buffer));
                        tr.buffers.insert(std::make_pair(buf, kBufferTypeVertex));
                    }
                
                    else if(buffer->getType() == kBufferTypeIndex)
                    {
                        auto buf = std::static_pointer_cast<Buffer>(buffer);
                        indexBuffers.insert(std::make_pair(buffer->getHandle(), buffer));
                        tr.buffers.insert(std::make_pair(buf, kBufferTypeIndex));
                    }
                
                    else 
                    {
                        Notification warnNotif = BuildNotification(kNotificationLevelWarning,
                            "Buffer type #%i not supported in Mesh.", buffer->getType());
                        NotificationCenter::GetDefault()->send(warnNotif);
                    }
                }
            }
        }
        
        submitTransaction(kMeshTransactionBatchAddBuffers, tr);
    }
    
    void Mesh::addVertexBuffer(std::shared_ptr < GenBuffer > const& buffer)
    {
        assert(buffer && "Null GenBuffer given.");
        MeshTransactionAddBuffer tr;
        
        {
            std::lock_guard < std::mutex > lck(buffersMutex);
            vertexBuffers.insert(std::make_pair(buffer->getHandle(), buffer));
        }
        
        tr.buffer = buffer;
        tr.type = kBufferTypeVertex;
        submitTransaction(kMeshTransactionAddBuffer, tr);
    }
    
    void Mesh::addIndexBuffer(std::shared_ptr < GenBuffer > const& buffer)
    {
        assert(buffer && "Null GenBuffer given.");
        MeshTransactionAddBuffer tr;
        
        {
            std::lock_guard < std::mutex > lck(buffersMutex);
            indexBuffers.insert(std::make_pair(buffer->getHandle(), buffer));
        }
        
        tr.buffer = buffer;
        tr.type = kBufferTypeIndex;
        submitTransaction(kMeshTransactionAddBuffer, tr);
    }
    
    void Mesh::submitTransaction(std::uint8_t type, Transaction::Clock::time_point const& tp)
    {
        std::scoped_lock < std::mutex > lck(driverCachesMutex);
        
        // For a Transaction to be valid, we must adds our Transaction to every driverCaches.transactions queue
        // (each driver will then process its transaction the way it wants to).
        
        for (auto& pair : driverCaches)
        {
            Transaction transaction(type, nullptr, tp);
            pair.second.transactions.push(std::move(transaction));
        }
    }
    
    void Mesh::addSubMeshes(std::vector < SubMesh > const& sm)
    {
        std::scoped_lock < std::mutex > lck(submeshesMutex);
        submeshes.insert(submeshes.end(), sm.begin(), sm.end());
        submitTransaction(kMeshTransactionAddSubMesh);
    }
    
    /*
    {
        Buffer buf = new GenBuffer(data, size*sizeof(Data), kBufferUsageStatic);
        Buffer ibuf = new GenBuffer(data, size*sizeof(Data), kBufferUsageDynamic);
        
        VertexDescriptor descriptor;
        descriptor.addComponent(kVertexComponentPosition, 0, sizeof(Data));
        descriptor.addComponent(kvertexComponentColor, sizeof(float)*4, sizeof(Data));
        
        SubMesh submesh;
        submesh.elements = size;
        submesh.buffer = buf;
        submesh.indexCount = idxSize;
        submesh.indexBuffer = ibuf;
        submesh.descriptor = descriptor;
        
        Mesh result;
        result.addSubMesh(submesh);
        result.addBuffers({buf, ibuf});
        
        result.associate(driver);
    }
    */
}
