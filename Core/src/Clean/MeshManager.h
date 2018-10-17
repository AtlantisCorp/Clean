/** =======================================================
 *  \file Core/MeshManager.h
 *  \date 10/16/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_MESHMANAGER_H
#define CLEAN_MESHMANAGER_H

#include "Manager.h"
#include "Mesh.h"

namespace Clean 
{
    class Core;
    
    class MeshManager : public Manager < Mesh >
    {
        //! @brief Current pointer to the global MeshManager. 
        static std::atomic < MeshManager* > currentManager;
        
        //! @brief Makes our Core class a friend. 
        friend class Core;
        
    public:
        
        /*! @brief Returns the current manager or throw an exception if not found. */
        static MeshManager& Current();
        
    public:
        
        /*! @brief Default constructor. */
        MeshManager() = default;
        
        /*! @brief Loads a file. 
         *
         * First it tries to find the correct loader by using the file extension. If a loader is 
         * found for your extension, and your callback returns true, then it is used to load the
         * file. Secondly, it calls FileLoader < Mesh >::load to load the file and returns it. The
         * mesh is added to this manager if not null. 
         *
        **/
        std::shared_ptr < Mesh > load(std::string const& filepath, std::function < bool(FileLoader<Mesh>const&) > checker = nullptr);
        
        /*! @brief Finds a Mesh loaded from a file. 
         *
         * If a Mesh holds the original file it comes from, this function will find the first Mesh 
         * corresponding to the given filepath. Notes that it is the responsability of the loader 
         * to fill in this field while creating the Mesh. The original file is never a Clean's 
         * Resource Path, and thus the path must be converted before trying to look for it. This
         * is done in this function.  
         *
        **/
        std::shared_ptr < Mesh > findByFile(std::string const& filepath) const;
    };
}

#endif // CLEAN_MESHMANAGER_H