/**
 * \file GlDriver/Win32/WinGlContext.cpp
 * \date 12/01/2018
**/

#include "WinGlContext.h"

#include <cassert>

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
using namespace Clean;

int WinGlGetPixelFormatAttrib(HWND window, int pixelFormat, int attrib, WGlPtrTable const& wgl)
{
    assert(wgl.ARB_pixel_format && "WGL: Extension ARB_pixel_format is not present.");
    int value = 0;

    if (!wgl.getPixelFormatAttribivARB(GetDC(window), pixelFormat, 0, 1, &attrib, &value))
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Can't retrieve given attribute %i.", attrib);
        NotificationCenter::GetDefault()->send(notif);
        return 0;
    }

    return value;
}

WinGlFBConfig WinGlFillFBConfig(HWND window, const int n, WGlPtrTable const& wgl)
{
    WinGlFBConfig config;
    memset(&config, 0, sizeof(config));

    if (wgl.ARB_pixel_format)
    {
        if (!WinGlGetPixelFormatAttrib(window, n, WGL_SUPPORT_OPENGL_ARB, wgl) ||
            !WinGlGetPixelFormatAttrib(window, n, WGL_DRAW_TO_WINDOW_ARB, wgl))
        return config;

        if (WinGlGetPixelFormatAttrib(window, n, WGL_PIXEL_TYPE_ARB, wgl) != WGL_TYPE_RGBA_ARB)
        return config;
            
        if (WinGlGetPixelFormatAttrib(window, n, WGL_ACCELERATION_ARB, wgl) == WGL_NO_ACCELERATION_ARB)
        return config;
            
        config.redBits = WinGlGetPixelFormatAttrib(window, n, WGL_RED_BITS_ARB, wgl);
        config.greenBits = WinGlGetPixelFormatAttrib(window, n, WGL_GREEN_BITS_ARB, wgl);
        config.blueBits = WinGlGetPixelFormatAttrib(window, n, WGL_BLUE_BITS_ARB, wgl);
        config.alphaBits = WinGlGetPixelFormatAttrib(window, n, WGL_ALPHA_BITS_ARB, wgl);

        config.depthBits = WinGlGetPixelFormatAttrib(window, n, WGL_DEPTH_BITS_ARB, wgl);
        config.stencilBits = WinGlGetPixelFormatAttrib(window, n, WGL_STENCIL_BITS_ARB, wgl);

        config.accRedBits = WinGlGetPixelFormatAttrib(window, n, WGL_ACCUM_RED_BITS_ARB, wgl);
        config.accGreenBits = WinGlGetPixelFormatAttrib(window, n, WGL_ACCUM_GREEN_BITS_ARB, wgl);
        config.accBlueBits = WinGlGetPixelFormatAttrib(window, n, WGL_ACCUM_BLUE_BITS_ARB, wgl);
        config.accAlphaBits = WinGlGetPixelFormatAttrib(window, n, WGL_ACCUM_ALPHA_BITS_ARB, wgl);

        config.auxBuffers = WinGlGetPixelFormatAttrib(window, n, WGL_AUX_BUFFERS_ARB, wgl);
        config.stereo = (bool) WinGlGetPixelFormatAttrib(window, n, WGL_STEREO_ARB, wgl);
        config.doubleBuffer = (bool) WinGlGetPixelFormatAttrib(window, n, WGL_DOUBLE_BUFFER_ARB, wgl);

        if (wgl.ARB_multisample)
        {
            config.sampleBuffers = WinGlGetPixelFormatAttrib(window, n, WGL_SAMPLE_BUFFERS_ARB, wgl);
            config.samples = WinGlGetPixelFormatAttrib(window, n, WGL_SAMPLES_ARB, wgl);
            config.multisample = (config.samples > 1);
        }

        if (wgl.ARB_framebuffer_sRGB || wgl.EXT_framebuffer_sRGB)
            config.sRGB = (bool) WinGlGetPixelFormatAttrib(window, n, WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, wgl);
        else if (wgl.EXT_colorspace)
            config.sRGB = (WinGlGetPixelFormatAttrib(window, n, WGL_COLORSPACE_EXT, wgl) == WGL_COLORSPACE_SRGB_EXT);
    }

    else 
    {
        PIXELFORMATDESCRIPTOR pfd;

        if (!DescribePixelFormat(GetDC(window), n, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
        return config;

        if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW) || !(pfd.dwFlags & PFD_SUPPORT_OPENGL))
        return config;

        if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) && (pfd.dwFlags & PFD_GENERIC_FORMAT))
        return config;

        config.redBits = pfd.cRedBits;
        config.greenBits = pfd.cGreenBits;
        config.blueBits = pfd.cBlueBits;
        config.alphaBits = pfd.cAlphaBits;

        config.depthBits = pfd.cDepthBits;
        config.stencilBits = pfd.cStencilBits;

        config.accRedBits = pfd.cAccumRedBits;
        config.accGreenBits = pfd.cAccumGreenBits;
        config.accBlueBits = pfd.cAccumBlueBits;
        config.accAlphaBits = pfd.cAccumAlphaBits;

        config.stereo = (bool) (pfd.dwFlags & PFD_STEREO);
        config.doubleBuffer = (bool) (pfd.dwFlags & PFD_DOUBLEBUFFER);
    }

    config.handle = n;
    return config;
}

WinGlFBConfig WinGlPixelFormatToFBConfig(PixelFormat const& pf)
{
    WinGlFBConfig result;
    memset(&result, 0, sizeof(result));
    
    if (pf.bitsPerPixel)
    {
        if (pf.bitsPerPixel == 24) 
        {
            result.redBits = 8;
            result.greenBits = 8;
            result.blueBits = 8;
        }

        else if (pf.bitsPerPixel == 32)
        {
            result.redBits = 8;
            result.greenBits = 8;
            result.blueBits = 8;
            result.alphaBits = 8;
        }

        else 
        {
            Notification notif = BuildNotification(kNotificationLevelError, "WGL: Specified BitsPerPixel value %i is not supported.", pf.bitsPerPixel);
            NotificationCenter::GetDefault()->send(notif);
            return result;
        }
    }

    else
    {
        result.redBits = pf.bits.red;
        result.greenBits = pf.bits.green;
        result.blueBits = pf.bits.blue;
        result.alphaBits = pf.bits.alpha;
    }

    result.depthBits = 24;
    result.stencilBits = 8;

    // result.accRedBits = pf.accumBits.red;
    // result.accGreenBits = pf.accumBits.green;
    // result.accBlueBits = pf.accumBits.blue;
    // result.accAlphaBits = pf.accumBits.alpha;

    // result.stereo = pf.stereo;
    result.doubleBuffer = (pf.buffers > 1);
    // result.sRGB = pf.sRGB;

    result.multisample = pf.multisampled;
    result.sampleBuffers = pf.sampleBuffers;
    result.samples = pf.samples;

    return result;
}

int WinGlChooseFBConfig(WinGlFBConfig& desired, std::vector < WinGlFBConfig > const& usables)
{
    unsigned int missing, leastMissing = UINT_MAX;
    unsigned int colorDiff, leastColorDiff = UINT_MAX;
    unsigned int extraDiff, leastExtraDiff = UINT_MAX;
    const WinGlFBConfig* closest = NULL;

    for (auto current : usables)
    {
        if (desired.stereo && !current.stereo)
        continue;

        if (desired.doubleBuffer && !current.doubleBuffer)
        continue;

        // Count number of missing buffers
        {
            missing = 0;

            if (desired.alphaBits > 0 && current.alphaBits == 0)
            missing++;

            if (desired.depthBits > 0 && current.depthBits == 0)
            missing++;

            if (desired.stencilBits > 0 && current.stencilBits == 0)
            missing++;

            if (desired.auxBuffers > 0 && current.auxBuffers < desired.auxBuffers)
            missing += desired.auxBuffers - current.auxBuffers;

            if (desired.samples > 0 && current.samples < desired.samples)
            missing += desired.samples - current.samples;
        }

        // Count value for color bits missing.
        {
            colorDiff = 0;
            colorDiff += (desired.redBits - current.redBits) * (desired.redBits - current.redBits);
            colorDiff += (desired.greenBits - current.greenBits) * (desired.greenBits - current.greenBits);
            colorDiff += (desired.blueBits - current.blueBits) * (desired.blueBits - current.blueBits);
            colorDiff += (desired.alphaBits - current.alphaBits) * (desired.alphaBits - current.alphaBits);
        }

        // Count value for non-color channel size.
        {
            extraDiff = 0;
            extraDiff += (desired.depthBits - current.depthBits) * (desired.depthBits - current.depthBits);
            extraDiff += (desired.stencilBits - current.stencilBits) * (desired.stencilBits - current.stencilBits);
            extraDiff += (desired.accRedBits - current.accRedBits) * (desired.accRedBits - current.accRedBits);
            extraDiff += (desired.accGreenBits - current.accGreenBits) * (desired.accGreenBits - current.accGreenBits);
            extraDiff += (desired.accBlueBits - current.accBlueBits) * (desired.accBlueBits - current.accBlueBits);
            extraDiff += (desired.accAlphaBits - current.accAlphaBits) * (desired.accAlphaBits - current.accAlphaBits);
            if (desired.sRGB && !current.sRGB) extraDiff++;
        }

        if (missing < leastMissing)
        closest = &current;

        else if (missing == leastMissing)
        {
            if ((colorDiff < leastColorDiff) || 
                (colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
            closest = &current;
        }

        if (closest == &current)
        {
            leastMissing = missing;
            leastColorDiff = colorDiff;
            leastExtraDiff = extraDiff;
        }
    }

    if (closest) return closest->handle;
    else return 0;
}

int WinGlChoosePixelFormat(HWND window, PixelFormat const& pf, WGlPtrTable const& wgl)
{
    int nativeCount = 0;

    if (wgl.ARB_pixel_format) 
        nativeCount = WinGlGetPixelFormatAttrib(window, 1, WGL_NUMBER_PIXEL_FORMATS_ARB, wgl);
    else 
        nativeCount = DescribePixelFormat(GetDC(window), 1, sizeof(tagPIXELFORMATDESCRIPTOR), NULL);

    std::vector < WinGlFBConfig > usableConfigs;
    usableConfigs.reserve(nativeCount);

    for (int i = 0; i < nativeCount; ++i)
    {
        const int n = i + 1;
        auto config = WinGlFillFBConfig(window, n, wgl);

        if (!config.redBits || !config.blueBits || !config.greenBits || !config.alphaBits)
        continue;
        
        usableConfigs.push_back(config);
    }

    if (!usableConfigs.size())
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: No pixel format found.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return 0;
    }

    WinGlFBConfig desiredConfig = WinGlPixelFormatToFBConfig(pf);
    int closest = WinGlChooseFBConfig(desiredConfig, usableConfigs);

    if (!closest)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to find a suitable pixel format.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return 0;
    }

    return closest;
}

HGLRC WinGlMakeContext(HWND window, PixelFormat const& pf, HGLRC shared, WGlPtrTable const& wgl)
{
    int pixelFormat = WinGlChoosePixelFormat(window, pf, wgl);
    int attribs[40];

    #define setAttrib(a, v) \
    { \
        assert((size_t) (index + 1) < sizeof(attribs) / sizeof(attribs[0])); \
        attribs[index++] = a; \
        attribs[index++] = v; \
    }

    if (!pixelFormat)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: No suitable pixel format.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return NULL;
    }

    HDC dc = GetDC(window);
    PIXELFORMATDESCRIPTOR pfd;

    if (!DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd))
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to retrieve pixel format.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return NULL;
    }

    if (!SetPixelFormat(dc, pixelFormat, &pfd))
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to set selected pixel format.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return NULL;
    }

    if (wgl.ARB_create_context)
    {
        int index = 0, mask = 0, flags = 0;

        // We assume our user wants a Core profile OpenGL 3+ context.
        mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        // if (isDebugMode()) mask |= WGL_CONTEXT_DEBUG_BIT_ARB;

        setAttrib(WGL_CONTEXT_MAJOR_VERSION_ARB, 3);
        setAttrib(WGL_CONTEXT_MINOR_VERSION_ARB, 3);

        if (flags)
        setAttrib(WGL_CONTEXT_FLAGS_ARB, flags);

        if (mask)
        setAttrib(WGL_CONTEXT_PROFILE_MASK_ARB, mask);

        setAttrib(0, 0);

        HGLRC handle = wgl.createContextAttribsARB(dc, shared, attribs);

        if (!handle)
        {
            DWORD const error = GetLastError();

            if (error == (0xc0070000 | ERROR_INVALID_VERSION_ARB))
            {
                Notification notif = BuildNotification(kNotificationLevelError, "WGL: OpenGL version %i.%i not supported.", 3, 3);
                NotificationCenter::GetDefault()->send(notif);
                return NULL;
            }

            else if (error == (0xc0070000 | ERROR_INVALID_PROFILE_ARB))
            {
                Notification notif = BuildNotification(kNotificationLevelError, "WGL: OpenGL profile not supported.", 0);
                NotificationCenter::GetDefault()->send(notif);
                return NULL;
            }

            else if (error == (0xc0070000 | ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB))
            {
                Notification notif = BuildNotification(kNotificationLevelError, "WGL: The share context is not compatible with the requested context.", 0);
                NotificationCenter::GetDefault()->send(notif);
                return NULL;
            }

            else
            {
                Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create requested context.", 0);
                NotificationCenter::GetDefault()->send(notif);
                return NULL;
            }
        }

        return handle;
    }

    else
    {
        HGLRC handle = wgl.createContext(dc);

        if (!handle)
        {
            Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create requested context.", 0);
            NotificationCenter::GetDefault()->send(notif);
            return NULL;
        }

        if (shared)
        {
            if (!wgl.shareLists(handle, shared))
            {
                Notification notif = BuildNotification(kNotificationLevelError, "WGL: The share context is not compatible with the requested context.", 0);
                NotificationCenter::GetDefault()->send(notif);
                return NULL;
            }
        }

        return handle;
    }
}

WinGlContext::WinGlContext(HWND win, HGLRC ctxt, WGlPtrTable const& tbl, PixelFormat const& pf) 
{
    window = win;
    dc = GetDC(window);
    rc = ctxt;
    wgl = tbl;
    pixelFormat = pf;
}

WinGlContext::~WinGlContext()
{
    wgl.makeCurrent(dc, NULL);
    wgl.deleteContext(rc);
}

bool WinGlContext::isValid() const 
{
    return window && dc && rc;
}

void WinGlContext::lock() const 
{
    
}

void WinGlContext::unlock() const
{

}

PixelFormat WinGlContext::getPixelFormat() const 
{
    return pixelFormat;
}

void WinGlContext::makeCurrent()
{
    wgl.makeCurrent(dc, rc);
}