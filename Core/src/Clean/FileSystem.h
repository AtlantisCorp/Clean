/** \file Core/FileSystem.h
**/

#ifndef CLEAN_FILESYSTEM_H
#define CLEAN_FILESYSTEM_H

#include "Singleton.h"

#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <list>

namespace Clean 
{
    /** @brief Represents a Virtual Directory in our FileSystem. */
    struct VirtualDirectory 
    {
        //! @brief Name of our directory. 
        std::string name;
        
        //! @brief List of pathes to directories.
        std::vector < std::string > realPathes;
        
        /*! @brief Constructs a VirtualDirectory. */
        VirtualDirectory(std::string const& name = std::string());
    };
    
    /** @brief Represents a virtual FileSystem to store Resources in objectives directories.
     *
     * FileSystem is used to sort all physical resources to virtual directories and groups. This make resource's 
     * search and indexing easier and faster. FileSystem can group your directories from the actual working directory
     * but also from absolute pathes. 
     *
     * A Clean Resource Path always start with 'Clean://'. Then, the resource's name is set, for example 'Mesh' for 
     * a resource of type Clean::Mesh. 'Mesh' is actually the virtual resource group. In this group, real pathes to 
     * physical directories are stored for where to look for meshes. For example, 'Clean://Mesh/Example.obj' will look
     * into all registered directories for the group 'Mesh' where a file is 'Example.obj'. Notes that multiple files may
     * be returned. 
     *
    **/
    class FileSystem : public Singleton < FileSystem >
    {   
        //! @brief List all Virtual Directories registered.
        std::vector < VirtualDirectory > virtualDirectories;
        
        //! @brief Protects virtualDirectories.
        mutable std::mutex virtualDirectoriesMutex;
        
    public:
        
        /*! @brief Constructs the default FileSystem. */
        FileSystem() = default;
        
        /*! @brief Destructs the default FileSystem. */
        ~FileSystem() = default;
        
        /*! @brief Registers a new Virtual Directory. */
        void makeVirtualDirectory(std::string const& name);
        
        /*! @brief Registers a path for the given virtual directory. */
        void addRealPath(std::string const& directory, std::string const& realPath);
        
        /*! @brief Returns a copy of the Virtual Directory. */
        VirtualDirectory findVirtualDirectory(std::string const& name) const;
        
        /*! @brief Returns true if the given path is a Clean's Resource Path. */
        bool isVirtualPath(std::string const& path) const;
        
        /*! @brief Tries to get the real path for the given Clean's Resource Path. 
         *
         * If path is not a Clean's Resource Path, then the given path is returned without modifying
         * it. If it is, the file is searched in all real pathes for the virtual directory, untill
         * the file is found. If no file is found, it returns an empty string. 
         *
         * If more than one file correspond to the given path, it returns the first found file.
         * \see findAllRealPathes for getting a list of files.
         *
        **/
        std::string findRealPath(std::string const& path) const;
        
        /*! @brief Tries to open a Clean's Resource Path. 
         *
         * If path is not a Clean's Resource Path, it returns a std::ifstream(path, mode) without
         * modifying the path. If it is, it returns std::ifstream(findRealPath(path), mode).
         *
         * \param[in] path Path we want to open a stream. 
         * \param[in] mode The opening mode for the file. 
         * \param[out] realPath If not null, holds the real path used to open the file. 
         *
        **/
        std::fstream open(std::string const& path, std::ios_base::openmode mode, std::string* realPath = nullptr) const;
        
        /*! @brief Extracts the Virtual Directory's name from the given Clean path. */
        std::string findVirtualDirectoryName(std::string const& path) const;
        
        /*! @brief Extracts the Virtual Filename from the given Clean path. */
        std::string findVirtualFileName(std::string const& path) const;
        
        /*! @brief Finds all files that matches the given Clean Resource Path. */
        std::list < std::string > findAllRealPathes(std::string const& path) const;
    };
}

#endif // CLEAN_FILESYSTEM_H
