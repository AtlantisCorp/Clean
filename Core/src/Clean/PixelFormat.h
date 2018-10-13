/** \file Core/PixelFormat.h
**/

#ifndef CLEAN_PIXELFORMAT_H
#define CLEAN_PIXELFORMAT_H

#include <cstddef>
#include <utility>

namespace Clean 
{
    /** @brief Represents a PixelFormat used by the Driver. 
     *
     * Actually it is how the data will be represented in the main Framebuffer, i.e. the main 
     * window displayed by the driver. For example, in OpenGL this represents the pixel format
     * of the default Framebuffer object. 
     *
     * Defining this PixelFormat is done with Driver::selectPixelFormat() which select the closest
     * PixelFormat available from given one. All fields might not be used by the Driver to compute
     * the final pixel format.
     *
    **/
    struct PixelFormat 
    {
        //! @brief Color bits.
        struct {
            std::uint8_t red;
            std::uint8_t green;
            std::uint8_t blue;
            std::uint8_t alpha;
        } bits;
        
        //! @brief Alternate way to describe 'bits': bits per pixel.
        std::uint8_t bitsPerPixel = 0;
        
        //! @brief True if should be optimized for fullscreen.
        bool fullscreen = false;
        
        //! @brief Number of wanted buffers. 
        std::uint8_t buffers = 2;
        
        //! @brief True if wanting a multisampled pixel format.
        bool multisampled = false;
        
        //! @brief Number of samples by buffer.
        std::uint8_t samples;
        
        //! @brief Number of samples buffer.
        std::uint8_t sampleBuffers;
        
        //! @brief True if we want a static back buffer size (not available everywhere).
        bool isFixedBufferSize = false;
        
        //! @brief Size for our fixed buffer, width then height.
        std::pair < std::uint16_t, std::uint16_t > fixedBufferSize;
    };
    
    /** @defgroup PixelFormatPoliciesGroup PixelFormat policies group
     *  @brief Regroups every policies to select a PixelFormat. 
     *  @{
    **/
    
    typedef std::uint8_t PixelFormatPolicy;
    static constexpr const std::uint8_t kPixelFormatClosest = 1;
    static constexpr const std::uint8_t kPixelFormatHighest = 2;
    static constexpr const std::uint8_t kPixelFormatLowest = 3;
    
    /** @} */
}

#endif // CLEAN_PIXELFORMAT_H
