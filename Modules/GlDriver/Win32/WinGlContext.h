/**
 * \file GlDriver/Win32/WinGlContext.h
 * \date 12/01/2018
**/

#ifndef GLDRIVER_WIN_WINGLCONTEXT_H
#define GLDRIVER_WIN_WINGLCONTEXT_H

#include "WinGlInclude.h"
#include "../All/GlContext.h"

#include <vector>
#include <Clean/PixelFormat.h>

/** @brief Groups standard WGL configuration. */
struct WinGlFBConfig
{
    int redBits, greenBits, blueBits, alphaBits;
    int depthBits, stencilBits;

    int accRedBits, accGreenBits, accBlueBits, accAlphaBits;
    int auxBuffers;

    bool stereo;
    bool doubleBuffer;

    bool multisample;
    int sampleBuffers;
    int samples;

    bool sRGB;

    int handle;
};

int WinGlGetPixelFormatAttrib(HWND window, int pixelFormat, int attrib, WGlPtrTable const& wgl);
WinGlFBConfig WinGlFillFBConfig(HWND window, const int n, WGlPtrTable const& wgl);
WinGlFBConfig WinGlPixelFormatToFBConfig(Clean::PixelFormat const& pf);
int WinGlChooseFBConfig(WinGlFBConfig& desired, std::vector < WinGlFBConfig > const& usables);
int WinGlChoosePixelFormat(HWND window, Clean::PixelFormat const& pf, WGlPtrTable const& wgl);
HGLRC WinGlMakeContext(HWND window, Clean::PixelFormat const& pf, HGLRC shared, WGlPtrTable const& wgl);

class WinGlContext : public GlContext
{
    HWND window;
    HDC dc;
    HGLRC rc;
    WGlPtrTable wgl;
    Clean::PixelFormat pixelFormat;

public:

    WinGlContext(HWND win, HGLRC ctxt, WGlPtrTable const& tbl, Clean::PixelFormat const& pf);

    ~WinGlContext();

    /*! @brief Returns true if context is valid. */
    bool isValid() const;
    
    /*! @brief 'Locks' the context.
    **/
    void lock() const;
    
    /*! @brief 'Unlocks' the context.
    **/
    void unlock() const;
    
    /*! @brief Returns the current pixel format for this context. */
    Clean::PixelFormat getPixelFormat() const;

    /*! @brief Makes this context current. */
    void makeCurrent();
};

#endif // GLDRIVER_WIN_WINGLCONTEXT_H