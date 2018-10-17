/** \file Core/Platform.h
**/

#ifndef CLEAN_PLATFORM_H
#define CLEAN_PLATFORM_H

#include <cstdint>
#include <string>
#include <list>

namespace Clean 
{
    /** @brief Utility namespace that groups some useful functions. */
    namespace Platform 
    {
        //! @brief Option for Split. 
        static constexpr const std::uint32_t kSplitNoEmpties = 0 << 0;
        //! @brief Option for Split. 
        static constexpr const std::uint32_t kSplitIncludesEmpties = 1 << 0;
        
        //! @brief Options for FindFromFiles.
        static constexpr const std::uint32_t kFindFilesNotRecursive = 0 << 0;
        static constexpr const std::uint32_t kFindFilesRecursive = 1 << 0;
        
        /*! @brief Splits a string in multiple substring. 
         * 
         * \param format Origin string to be splitted. 
         * \param ch Character to split the string with. 
         * \param ops Options to split the string. 
         *
         * Different options are available: 
         * - kSplitNoEmpties: If an empty substring is encountered, it will
         *      not be added to the resulting list. 
         * - kSplitIncludesEmpties: If an empty substring is encountered, it will
         *      be added to the resulting list. 
         *
         * \return A list of string. 
        **/
        std::list < std::string > Split(std::string const& format, char ch, std::uint32_t ops = 0);
        
        /*! @brief Concatenates two pathes. 
         * 
         * If lhs ends up with kPathSeparator, or rhs begins with kPathSeparator, returns lhs + rhs. If 
         * not, returns lhs + kPathSeparator + rhs. If kPathSeparator is found in both pathes, erase the one
         * in lhs (erasing end is faster than begin).
         *
        **/
        std::string PathConcatenate(std::string const& lhs, std::string const& rhs);
        
#       ifdef CLEAN_PLATFORM_WIN32
        //! @brief Path separator on Windows. 
        static constexpr const char* kPathSeparator = "\\";
        
#       else
        //! @brief On all other platforms, kPathSeparator is '/'.
        static constexpr const char* kPathSeparator = "/";
        
#       endif
        
        /*! @brief Find all files with given pattern in the current directory.
         * 
         * \param pattern Pattern to find. Based on analyse of the file's name. 
         * \param ops One of kFindFilesRecursive, kFindFilesNotRecursive. 
         * 
         * \return A list of files found. 
        **/
        std::list < std::string > FindFiles(std::string const& pattern, std::uint32_t const& ops = kFindFilesNotRecursive);
        
        /*! @brief Returns filename for the given path. Basically it is the last element after the last kPathSeparator 
         * character, and nothing less. */
        std::string PathGetFilename(std::string const& rhs);
        
        /*! @brief Returns true if the given path matches given pattern. */
        bool PathPatternMatches(std::string const& path, std::string const& pattern);
        
        /*! @brief Returns the given file extension. */
        std::string PathGetExtension(std::string const& path);
        
        /*! @brief Reads the whole stream into a std string. */
        void StreamGetContent(std::istream& stream, std::string& out);
    }
}

#endif // CLEAN_PLATFORM_H
