/** \file Core/FileSystem.cpp
**/

#include "FileSystem.h"
#include "Platform.h"

namespace Clean 
{
    VirtualDirectory::VirtualDirectory(std::string const& n) : name(n)
    {
        
    }
    
    void FileSystem::makeVirtualDirectory(std::string const& name)
    {
        std::lock_guard < std::mutex > lck(virtualDirectoriesMutex);
        auto it = std::find_if(virtualDirectories.begin(), virtualDirectories.end(), [name](auto dir){ return dir.name == name; });
        if (it == virtualDirectories.end()) virtualDirectories.push_back(VirtualDirectory(name));
    }
    
    void FileSystem::addRealPath(std::string const& directory, std::string const& realPath) 
    {
        std::lock_guard < std::mutex > lck(virtualDirectoriesMutex);
        auto it = std::find_if(virtualDirectories.begin(), virtualDirectories.end(), [directory](auto dir){ return dir.name == directory; });
        
        if (it != virtualDirectories.end()) {
            (*it).realPathes.push_back(realPath);
        } else {
            VirtualDirectory vdir(directory);
            vdir.realPathes.push_back(realPath);
            virtualDirectories.push_back(vdir);
        }
    }
    
    VirtualDirectory FileSystem::findVirtualDirectory(std::string const& name) const 
    {
        std::lock_guard < std::mutex > lck(virtualDirectoriesMutex);
        auto it = std::find_if(virtualDirectories.begin(), virtualDirectories.end(), [name](auto dir){ return dir.name == name; });
        if (it != virtualDirectories.end()) return *it;
        else return VirtualDirectory();
    }
    
    bool FileSystem::isVirtualPath(std::string const& path) const 
    {
        return path.substr(0, strlen("Clean://")) == "Clean://";
    }
    
    std::string FileSystem::findRealPath(std::string const& path) const 
    {
        if (!isVirtualPath(path))
            return path;
        
        std::string dirname = findVirtualDirectoryName(path);
        std::string filename = findVirtualFileName(path);
        
        if (dirname.empty() || filename.empty())
            return std::string();
        
        VirtualDirectory directory = findVirtualDirectory(dirname);
        if (directory.name.empty())
            return std::string();
        
        for (std::string const& realPath : directory.realPathes)
        {
            std::string realFile = Platform::PathConcatenate(realPath, filename);
            std::list < std::string > foundFiles = Platform::FindFiles(realFile);
            
            if (!foundFiles.empty())
                return foundFiles.front();
        }
        
        return std::string();
    }
    
    std::fstream FileSystem::open(std::string const& path, std::ios_base::openmode mode) const
    {
        std::string realPath = findRealPath(path);
        return std::fstream(realPath.data(), mode);
    }
    
    std::string FileSystem::findVirtualDirectoryName(std::string const& path) const 
    {
        std::size_t firstPos = strlen("Clean://");
        std::size_t secondSlash = path.find_first_of('/', firstPos) - firstPos;
        return path.substr(firstPos, secondSlash);
    }
    
    std::string FileSystem::findVirtualFileName(std::string const& path) const
    {
        std::string dirname = findVirtualDirectoryName(path);
        std::size_t length = strlen("Clean://") + dirname.size();
        return path.substr(length, std::string::npos);
    }
    
    std::list < std::string > FileSystem::findAllRealPathes(std::string const& path) const
    {
        if (!isVirtualPath(path))
            return { path };
        
        std::string dirname = findVirtualDirectoryName(path);
        std::string filename = findVirtualFileName(path);
        
        if (dirname.empty() || filename.empty())
            return {};
        
        VirtualDirectory directory = findVirtualDirectory(dirname);
        if (directory.name.empty())
            return {};
        
        std::list < std::string > results;
        for (std::string const& realPath : directory.realPathes)
        {
            std::string realFile = Platform::PathConcatenate(realPath, filename);
            std::list < std::string > foundFiles = Platform::FindFiles(realFile);
            
            if (!foundFiles.empty())
                results.insert(results.end(), foundFiles.begin(), foundFiles.end());
        }
        
        return results;
    }
}
