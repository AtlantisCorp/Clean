/** \file Core/Version.h
**/

#ifndef CLEAN_VERSION_H
#define CLEAN_VERSION_H

#include <cstdint>
#include <string>

namespace Clean 
{
    /** @brief Represents a Version with major, minor, patch, and build number. */
    struct Version 
    {
        union 
        {
            struct 
            {
                //! @brief Major number. 
                std::uint32_t major = 0;
        
                //! @brief Minor number. 
                std::uint32_t minor = 0;
        
                //! @brief Patch number. 
                std::uint32_t patch = 0;
        
                //! @brief Build number. 
                std::uint32_t build = 0;
            };
            
            //! @brief Packed representation of a Version structure.
            std::uint32_t data[4];
            
            struct 
            {
                //! @brief High bytes (Major << 32 | Minor). Used for compareason of two versions.
                std::uint64_t high;
                //! @brief Low bytes (Patch << 32 | Build). Used for compareason of two versions.
                std::uint64_t low;
            };
        };
        
        /*! @brief Default constructor. */
        Version() = default;
        
        /*! @brief Copy constructor. */
        Version(Version const&) = default;
        
        /*! @brief Move constructor. */
        Version(Version&&) = default;
        
        /*! @brief Constructs a Version from a std::string format. 
         *
         * \note
         * Format is always 'Major.Minor.Patch.Build', where each component is 
         * optional but all dots are required. For example, '1.0..45' means Major 1
         * minor 0 patch 0 build 45.
         *
        **/
        static Version FromString(std::string const& format);
        
        /*! @brief Returns a formatted Version string. 
         *
         * \note
         * Format is always 'Major.Minor.Patch.Build', where each component is 
         * optional but all dots are required. For example, '1.0..45' means Major 1
         * minor 0 patch 0 build 45.
         *
        **/
        std::string toString() const;
    };
    
    /*! @brief Version operators. 
     * @{
    **/
    
    bool operator == (Version const& lhs, Version const& rhs);
    bool operator != (Version const& lhs, Version const& rhs);
    bool operator >  (Version const& lhs, Version const& rhs);
    bool operator >= (Version const& lhs, Version const& rhs);
    bool operator <  (Version const& lhs, Version const& rhs);
    bool operator <= (Version const& lhs, Version const& rhs);
    
    /*! @} */
}

#endif // CLEAN_VERSION_H