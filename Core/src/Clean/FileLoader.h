/** \file Core/FileLoader.h
**/

#ifndef CLEAN_FILELOADER_H
#define CLEAN_FILELOADER_H

#include "Version.h"

namespace Clean 
{
    /** @brief Defines some informations about a file loader. */
    struct FileLoaderInfos 
    {
        //! @brief Name for the loader. 
        std::string name;
        
        //! @brief Description/summary of this loader. 
        std::string description;
        
        //! @brief Authors that have written this loader. 
        std::string authors;
        
        //! @brief Version of this loader. 
        Version version;
    };
    
    /** @brief Interface for a file's loader. 
     *
     * A FileLoaderInterface derived is only a loader that can load from a file or from bytes in memory some
     * objects (which are not defined in this interface). When defining a new loadable object (like Mesh), you
     * mustn't directly derive from this class. Instead, use full specialization of FileLoader to specify your 
     * new type. (This is kind of like a metaclass wrapper.)
     *
    **/
    class FileLoaderInterface
    {
    public:
        
        /*! @brief Default constructor, does nothing. */
        virtual ~FileLoaderInterface() = default;
        
        /*! @brief Must return true if the given extension is loadable by this loader. */
        virtual bool isLoadable(std::string const& extension) const = 0;
        
        /*! @brief May return a FileLoaderInfos structure, if provided by the implementation. */
        virtual FileLoaderInfos getInfos() const { return FileLoaderInfos(); }
    };
    
    /** @brief Generic implementation of FileLoaderInterface. 
     *
     * Basically it is a null loader. When you want to create a new loader for your fancy new base class, fully
     * specialize this class and defines all your 'load' logic. This way, Core::findFileLoader will return your
     * specialized version of FileLoader for your class. 
     *
    **/
    template < typename ResultType > 
    class FileLoader : public FileLoaderInterface 
    {
    public:
        
        /*! @brief Does nothing. */
        ~FileLoader() = default;
        
        /*! @brief Always return false. */
        bool isLoadable(std::string const&) const { return false; }
    };
    
    /** Example
    
    template <>
    class FileLoader < Mesh > : public FileLoaderInterface
    {
        virtual ~FileLoader() = default;
        virtual std::shared_ptr < Mesh > load(std::string const& path) const = 0;
    };
    
    **/
}

#endif // CLEAN_FILELOADER_H