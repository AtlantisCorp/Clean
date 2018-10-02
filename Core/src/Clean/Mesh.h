/** \file Core/Mesh.h
**/

#ifndef CLEAN_MESH_H
#define CLEAN_MESH_H

#include "Buffer.h"
#include "ShaderAttribute.h"
#include "VertexDescriptor.h"
#include "GenBuffer.h"
#include "Transaction.h"
#include "Allocate.h"
#include "FileLoader.h"

#include <cstdint>
#include <cstddef>
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <queue>
#include <list>

namespace Clean
{
    class Driver;
    class Shader;
    struct RenderCommand;
    
    /*! @brief Automatically calls Buffer::release when the shared_ptr is destroyed. */
    class BufferAutorelease : public std::shared_ptr < Buffer >
    {
    public:
        using std::shared_ptr < Buffer >::shared_ptr;
        
        ~BufferAutorelease() {
            auto ptr = get();
            if (ptr) ptr->release();
        }
    };

    /** @brief Represents a single draw work for a Mesh.
    **/
    struct SubMesh
    {
        //! @brief Offset where the submesh begins, in number of elements.
        std::size_t offset = 0;

        //! @brief Number of elements.
        std::size_t elements = 0;
        
        //! @brief Handle to the buffer we store our submesh in. 
        std::shared_ptr < Buffer > buffer = nullptr;

        //! @brief Offset in the index buffer, if one.
        std::size_t indexOffset = 0;

        //! @brief Number of elements in the index buffer, if one.
        std::size_t indexCount = 0;

        //! @brief Index buffer if there is one. This buffer is only in RAM, but
        //! a VRAM buffer may be available for the current driver.
        std::shared_ptr < Buffer > indexBuffer = nullptr;
        
        //! @brief Descriptor for each components in the submesh.
        VertexDescriptor descriptor;
    };
    
    static constexpr const std::uint8_t kMeshTransactionAddSubMesh = 1;
    static constexpr const std::uint8_t kMeshTransactionRemoveSubMesh = 2;
    static constexpr const std::uint8_t kMeshTransactionAddBuffer = 3;
    static constexpr const std::uint8_t kMeshTransactionUpdateBuffer = 4;
    static constexpr const std::uint8_t kMeshTransactionBatchAddBuffers = 5;
    static constexpr const std::uint8_t kMeshTransactionBatchUpdateBuffers = 6;

    /** @brief Generic representation of a Mesh.
     *
     * A Mesh is basically divided in two parts. The 'software' part holds all buffers in RAM, and all informations
     * about submeshes. This part is accessible freely from the user. The 'hardware' part holds all buffers in VRAM,
     * and a cached ShaderAttributesMap for each Shader used. 
     *
     * As seen above, cache is organized by Driver AND by Shader used to render this Mesh. This lets the mesh caches
     * all the data any driver/shader pair may need to render it correctly. 
     *
     * Transactional update of cache
     * Whenever a user (or any kind of user/developer) needs to update the Mesh's buffers, the cache shouldn't be cleared
     * and rebuilt (this is a slow method). Instead, VRAM buffers should be updated normally with the new data. For all 
     * operations on the mesh (add/remove submeshes, add/update one or multiple buffers), a Transaction is commited into 
     * the Driver's cache queue. When the driver render the Mesh, it can use Mesh::update() to allocate the Mesh some time
     * to collect those Transactions and update its cache. This way, Mesh's cache update is done in the rendering thread,
     * asynchroneously from other Mesh's operations. 
     *
    **/
    class Mesh final : public Handled < Mesh >
    {
        //! @brief Stores all Vertex buffers for our mesh. Those are Generic buffers. See 
        //! driverBuffers for VRAM buffers. 
        std::map < std::size_t, std::shared_ptr < GenBuffer > > vertexBuffers;
        
        //! @brief Stores all Index buffers for our mesh. Those are Generic buffers. See
        //! driverBuffers for VRAM buffers. 
        std::map < std::size_t, std::shared_ptr < GenBuffer > > indexBuffers;
        
        //! @brief Protects all operations on Generic buffers.
        mutable std::mutex buffersMutex;
        
        //! @brief Stores all submeshes for this mesh.
        std::vector < SubMesh > submeshes;
        
        //! @brief Protects submeshes.
        mutable std::mutex submeshesMutex;
        
        typedef std::uintptr_t ShaderKey;
        typedef std::uintptr_t DriverKey;
        
        //! @brief Caches data for a shader. 
        struct ShaderCache 
        {
            //! @brief Stores all ShaderAttributesMap generated. (1 ShaderAttributesMap for 1 VertexDescriptor)
            std::vector < ShaderAttributesMap > shaderAttribs;
        };
        
        //! @brief Caches data for the driver. 
        struct DriverCache
        {   
            //! @brief Stores all buffers stored for a driver. (Key is handle of soft buffer, value is hard buffer)
            std::map < std::uintptr_t, BufferAutorelease > buffers;
            
            //! @brief Stores all caches for each shader for this driver. (1 ShaderCache for 1 Shader)
            std::map < ShaderKey, ShaderCache > shaderCaches;
            
            //! @brief Stores all transactions pending for this driver. 
            std::queue < Transaction, std::list < Transaction > > transactions;
        };
        
        //! @brief Stores caches for each driver. 
        mutable std::map < DriverKey, DriverCache > driverCaches;
        
        //! @brief Protects all caches entrees.
        mutable std::mutex driverCachesMutex;
        
        //! @brief Drawing method used. 
        std::atomic < std::uint8_t > drawingMethod;

    public:
        
        /*! @brief Constructs an empty Mesh. */
        Mesh();
        
        /*! @brief Moves the mesh. */
        Mesh(Mesh&&);
        
        /*! @brief A Mesh is not copyiable. Delete this function. */
        Mesh(Mesh const&) = delete;
        
        /*! @brief A Mesh is not copyiable. Delete this function. */
        Mesh operator = (Mesh const&) = delete;

        /*! @brief Returns all VertexDescriptor for each SubMesh.
         *
         * Returns all VertexDescriptor for each SubMesh where all buffers are emplaced with cached data for 
         * the selected Driver. If no cached data is available, the software buffer is always returned and 
         * a notification is sent. 
         *
         * \note Ensure \ref associate is called for the selected driver before using this function. An 
         * unsassociated driver will never allocate caches in a Mesh. 
         *
        **/
        std::vector < VertexDescriptor > findAssociatedDescriptors(Driver const& driver) const;

        /*! @brief Returns a list of VertexDescriptor for each submeshes. */
        std::vector < VertexDescriptor > findDescriptors() const;

        /*! @brief Populates a RenderCommand with RenderSubCommands generated by this mesh.
         *
         * Each RenderSubCommand is generated for one submesh from:
         * - cached ShaderAttributesMap for driver and shader if already available,
         * - cached VertexDescriptor for driver. A new ShaderAttributesMap is generated for
         *      the shader and is cached if \ref liveCache is true.
         * - default VertexDescriptor if none of the above is available.
         *
         * It is highly advised to call Mesh::associate for the driver before drawing it to caches
         * data for the driver and to create hardware buffers. Shader caches are stored when this 
         * function is called. Uses this function in your rendering thread.
         *
        **/
        void populateRenderCommand(Driver const& driver, Shader const& shader, RenderCommand& command);
        
        /*! @brief Finds ShaderAttributesMap cached for given Driver and Shader.
         *
         * If ShaderCache is not available, it returns only an empty vector. Use shaderCacheGenerate()
         * to explicitly generate the ShaderCache, or populateRenderCommand() to generate the cache and
         * populate your RenderCommand. 
         *
        **/
        std::vector < ShaderAttributesMap > findShaderAttributesMap(Driver const& driver, Shader const& shader);
        
        /*! @brief Stores the given ShaderCache. */
        void shaderCacheStore(Driver const& driver, Shader const& shader, ShaderCache& cache);

        /*! @brief Associates this mesh to a driver.
         *
         * Associating a Mesh to a Driver creates a DriverCache entree for this Driver. It also 
         * creates all caches for available buffers. Uses this function after creating the Mesh
         * to caches all hardware buffers. This function should be used in your main/rendering 
         * thread. 
         *
        **/
        void associate(Driver& driver);
        
        /*! @brief Updates the cache associated to one driver. 
         *
         * Generally called by a SceneManager, or any kind of manager that draws all objects and update
         * their RenderQueues (and RenderCommands), this function updates all caches for the associated
         * Driver, executing asynchrneously Driver's related functions like updates/creation/deletion of
         * hardware buffers, or clearing of the Shader's Attributes caches.
         *
         * \param[in] driver A reference to the driver that we must update our cache from. 
         * \param[in] maxTime Maximum time this function can takes place. 
         *
         * Giving a maximum time to this function makes it ignore all Transactions objects that are invalid
         * AND not commiting all pending Transactions if the maximum time is reached. This can lead to visual 
         * defaults, but lets the driver manage its frame time. 
         *
        **/
        void update(Driver& driver, std::chrono::milliseconds maxTime);
        
        /*! @brief Adds a SubMesh to this Mesh. 
         *
         * Submits a MeshTransactionAddSubMesh transaction after the process. The SubMesh must be valid,
         * and already filled with valid data. The given submesh is reset as this is a move process.
         *
         * \note Adding a SubMesh clears the ShaderCache for each driver. Thus, ShaderAttributesMap must be
         * rebuilt for each Shader used with this Mesh. This might be a slow operation and updating buffers
         * with new data must be preffered to modify the Mesh data. 
         *
        **/
        void addSubMesh(SubMesh&& submesh);
        
        /*! @brief Adds multiple buffers and submit a kMeshTransactionBatchAddBuffers transaction. 
         *
         * Use this function when you must add multiple buffers at once. One transaction will be 
         * submitted for the whole batch instead of one transaction by buffer. Buffer's type (Vertex
         * or Index) must be given for every GenBuffer given, or the buffer will be ignored.
         *
         * \note Think not to add twice the same buffer! No check is done to assert the GenBuffer 
         * is not already in the Mesh. However, as hardware buffer are computed from handles, if
         * the same GenBuffer is added twice in a Mesh, only one hardware buffer will be created. 
         *
        **/
        void addBuffers(std::vector < std::shared_ptr < GenBuffer > > const& buffers);
        
        /*! @brief Adds the given buffer to vertex buffers. 
         *
         * Buffer is added to vertexBuffers's list. A MeshTransactionAddBuffer transaction is submitted
         * to ensure drivers will create the hardware buffer related to this buffer. However, adding a
         * not used buffer will lead to hardware buffer created but not used. 
        **/
        void addVertexBuffer(std::shared_ptr < GenBuffer > const& buffer);
        
        /*! @brief Adds the given buffer to index buffers. 
         *
         * Buffer is added to indexBuffers's list. A MeshTransactionAddBuffer transaction is submitted
         * to ensure drivers will create the hardware buffer related to this buffer. However, adding a
         * not used buffer will lead to hardware buffer created but not used. 
         *
        **/
        void addIndexBuffer(std::shared_ptr < GenBuffer > const& buffer);
        
        /*! @brief Submits a transaction to every associated Drivers. 
         *
         * \param[in] type Transaction's type. This parameter is constantly defined by Mesh. 
         * \param[in] data A reference to the custom transaction's data type. This type must be copiable and 
         *      will be copied in each Transaction object submitted to each driver queue. 
         * \param[in] tp Maximum time_point untill which the Transaction is valid. 
         *
        **/
        template < class TransactionData > 
        void submitTransaction(std::uint8_t type, TransactionData& data, 
                Transaction::Clock::time_point const& tp = Transaction::Clock::time_point::max())
        {
            std::scoped_lock < std::mutex > lck(driverCachesMutex);
            
            // For a Transaction to be valid, we must adds our Transaction to every driverCaches.transactions queue
            // (each driver will then process its transaction the way it wants to). 
            
            for (auto& pair : driverCaches)
            {
                TransactionData* trData = Allocate < TransactionData >(1, data);
                Transaction transaction(type, (void*) trData, tp);
                pair.second.transactions.push(std::move(transaction));
            }
        }
        
        /*! @brief Submits a transaction with no data. */
        void submitTransaction(std::uint8_t type, Transaction::Clock::time_point const& tp = Transaction::Clock::time_point::max());
    };
    
    /** @brief Interface for all Mesh loaders. */
    template <>
    class FileLoader < Mesh > : public FileLoaderInterface
    {
    public:
        virtual ~FileLoader() = default;
        
        /*! @brief Loads a Mesh from the given existing path. */
        virtual std::shared_ptr < Mesh > load(std::string const& path) const = 0;
    };
};

#endif // CLEAN_MESH_H
