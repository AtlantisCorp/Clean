/** =======================================================
 *  \file Core/Hash.h
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_HASH_H
#define CLEAN_HASH_H

namespace Clean 
{
    namespace HashDetail
    {
        constexpr const std::uint32_t Val32Const = 0x811c9dc5;
        constexpr const std::uint32_t Prime32Const = 0x1000193;
        constexpr const std::uint64_t Val64Const = 0xcbf29ce484222325;
        constexpr const std::uint64_t Prime64Const = 0x100000001b3;
    }
    
    /*! @brief Calculates a Hash32 from generic data. */
    inline const std::uint32_t Hash32(const void* key, const std::uint32_t len) {
        
        const char* data = (const char*)key;
        std::uint32_t hash = HashDetail::Val32Const;
        std::uint32_t prime = HashDetail::Prime32Const;
        
        for(int i = 0; i < len; ++i) {
            std::uint8_t value = data[i];
            hash = hash ^ value;
            hash *= prime;
        }
        
        return hash;
    }
    
    /*! @brief Calculates a Hash32 from a string. */
    inline const std::uint32_t Hash32(const char* str) { 
        return Hash32((const void*)str, strlen(str));
    }
    
    /*! @brief Calculates a constant expression hash32 from a string. */
    inline constexpr std::uint32_t Hash32Const(const char* const str, const std::uint32_t value = HashDetail::Val32Const) noexcept {
        return (str[0] == '\0') ? value : Hash32Const(&str[1], (value ^ std::uint32_t(str[0])) * HashDetail::Prime32Const);
    }
    
    /*! @brief Calculates a Hash64 from generic data. */
    inline const std::uint64_t Hash64(const void* key, const std::uint64_t len) {
        
        const char* data = (const char*)key;
        std::uint64_t hash = HashDetail::Val64Const;
        std::uint64_t prime = HashDetail::Prime64Const;
        
        for(int i = 0; i < len; ++i) {
            std::uint8_t value = data[i];
            hash = hash ^ value;
            hash *= prime;
        }
        
        return hash;
    }
    
    /*! @brief Calculates a Hash64 from a string. */
    inline const std::uint64_t Hash64(const char* str) { 
        return Hash64((const void*)str, strlen(str));
    }
    
    /*! @brief Calculates a constant expression hash64 from a string. */
    inline constexpr std::uint64_t Hash64Const(const char* const str, const std::uint64_t value = HashDetail::Val64Const) noexcept {
        return (str[0] == '\0') ? value : Hash64Const(&str[1], (value ^ std::uint64_t(str[0])) * HashDetail::Prime64Const);
    }
}

#endif // CLEAN_HASH_H