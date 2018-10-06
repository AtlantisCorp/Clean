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
            // Algorithm from Wildcard Pattern Matching, GeeksForGeeks.
            // https://www.geeksforgeeks.org/wildcard-pattern-matching/
            std::size_t n = name.size(), m = pattern.size();
            
            // empty pattern can only match with empty string
            if (m == 0)
                return (n == 0);
            
            // lookup table for storing results of subproblems
            bool lookup[n + 1][m + 1];
            
            // initailze lookup table to false
            memset(lookup, false, sizeof(lookup));
            
            // empty pattern can match with empty string
            lookup[0][0] = true;
            
            // Only '*' can match with empty string
            for (int j = 1; j <= m; j++)
                if (pattern[j - 1] == '*')
                    lookup[0][j] = lookup[0][j - 1];
            
            // fill the table in bottom-up fashion
            for (int i = 1; i <= n; i++)
            {
                for (int j = 1; j <= m; j++)
                {
                    // Two cases if we see a '*'
                    // a) We ignore ‘*’ character and move
                    //    to next  character in the pattern,
                    //     i.e., ‘*’ indicates an empty sequence.
                    // b) '*' character matches with ith
                    //     character in input
                    if (pattern[j - 1] == '*')
                        lookup[i][j] = lookup[i][j - 1] ||
                        lookup[i - 1][j];
                    
                    // Current characters are considered as
                    // matching in two cases
                    // (a) current character of pattern is '?'
                    // (b) characters actually match
                    else if (pattern[j - 1] == '?' ||
                             name[i - 1] == pattern[j - 1])
                        lookup[i][j] = lookup[i - 1][j - 1];
                    
                    // If characters don't match
                    else lookup[i][j] = false;
                }
            }
            
            return lookup[n][m];
        }
    }
}
