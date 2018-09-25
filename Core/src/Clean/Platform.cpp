/** \file Core/Platform.cpp
**/

#include "Platform.h"
#include <sstream>

#ifdef CLEAN_PLATFORM_WIN32
#include <windows.h>

#else 
#include <sys/types.h>
#include <dirent.h>

#endif

namespace Clean 
{
    namespace Platform
    {
        std::list < std::string > Split(std::string const& format, char ch, std::uint32_t ops)
        {
            std::list < std::string > tokens;
            std::string token;
            std::istringstream stream(format);
            
            while (std::getline(stream, token, ch))
            {
                if (!token.empty() || (ops & kSplitIncludesEmpties))
                {
                    tokens.push_back(token);
                }
            }
            
            return tokens;
        }
        
        std::string PathConcatenate(std::string const& lhs, std::string const& rhs)
        {
            if (lhs.empty()) return rhs;
            if (rhs.empty()) return lhs;
            
            char lhsEnd = lhs.back();
            char rhsBeg = rhs.front();
            
            if (lhsEnd == kPathSeparator[0] && rhsBeg != kPathSeparator[0]) return lhs + rhs;
            if (lhsEnd != kPathSeparator[0] && rhsBeg == kPathSeparator[0]) return lhs + rhs;
            if (lhsEnd != kPathSeparator[0] && rhsBeg != kPathSeparator[0]) return lhs + kPathSeparator + rhs;
            
            std::string result = lhs;
            result.pop_back();
            return lhs + rhs;
        }
        
        std::string PathGetDirectory(std::string const& rhs)
        {
            return rhs.substr(0, rhs.find_last_of(kPathSeparator));
        }
        
        std::list < std::string > FindFiles(std::string const& pattern, std::uint32_t const& ops)
        {
            std::list < std::string > result;
            std::string dir = PathGetDirectory(pattern);
            
#           ifdef CLEAN_PLATFORM_WIN32
            WIN32_FIND_DATA data;
            HANDLE handle;
            
            if ((handle = FindFirstFile(pattern.data(), &data)) != INVALID_HANDLE_VALUE)
            {
                do 
                {
                    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (ops & kFindFilesRecursive))
                    {
                        auto patternSolo = PathGetFilename(pattern);
                        auto patternRecursive = PathConcatenate(PathConcatenate(dir, data.cFilename), patternSolo);
                        auto result2 = FindFiles(patternRecursive, ops);
                        result.splice(result.end(), result2);
                    }
                    
                    else if (data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
                    {
                        std::string filepath = PathConcatenate(dir, data.cFileName);
                        result.push_back(filepath);
                    }
                    
                } while(FindNextFile(handle, &data) != 0);
                
                FindClose(handle);
            }
            
            return result;
            
#           else
            std::string name = PathGetFilename(pattern);
            DIR* dirp = opendir(dir.data());
            struct dirent* entry;
            
            if (dirp)
            {
                while ((entry = readdir(dirp)) != NULL)
                {
                    if ((entry->d_type == DT_DIR) && (ops & kFindFilesRecursive))
                    {
                        auto patternSolo = PathGetFilename(pattern);
                        auto patternRecursive = PathConcatenate(PathConcatenate(dir, entry->d_name), patternSolo);
                        auto result2 = FindFiles(patternRecursive, ops);
                        result.splice(result.end(), result2);
                    }
                    
                    else if (entry->d_type == DT_REG)
                    {
                        std::string filename = entry->d_name;
                    
                        if (PathPatternMatches(filename, name))
                        {
                            filename = PathConcatenate(dir, filename);
                            result.push_back(filename);
                        }
                    }
                }
            }
            
            return result;
            
#           endif
        }
        
        std::string PathGetFilename(std::string const& rhs)
        {
            return rhs.substr(rhs.find_last_of(kPathSeparator) + 1, std::string::npos);
        }
        
        bool PathPatternMatches(std::string const& name, std::string const& pattern)
        {
            std::list < std::string > matches = Split(pattern, '*');
            std::size_t it = 0; 
            bool findNext = false;
            
            for (auto& c : pattern)
            {
                if (c != '*' && !findNext)
                {
                    if (name.at(it) == c)
                    {
                        it++; 
                        continue;
                    }
                    
                    else 
                    {
                        return false;
                    }
                }
                
                else if (c != '*' && findNext)
                {
                    if (name.find_first_of(c, it) != std::string::npos)
                    {
                        it++;
                        findNext = false;
                        continue;
                    }
                    
                    else 
                    {
                        return false;
                    }
                }
                
                else 
                {
                    findNext = true;
                }
            }
            
            return true;
        }
    }
}
