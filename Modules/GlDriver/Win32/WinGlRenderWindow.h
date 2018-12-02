/**
 * \file GlDriver/Win32/WinGlRenderWindow.h
 * \date 11/30/2018
**/

#ifndef GLDRIVER_WIN_WINGLRENDERWINDOW_H
#define GLDRIVER_WIN_WINGLRENDERWINDOW_H

#include "WinGlInclude.h"
#include "WinGlContext.h"

#include <Clean/RenderWindow.h>

/** @brief Registers the GlDriver's Window Class. */
bool WinGlRegisterWindowClass();

/** @brief Creates a hidden Window for fake context and other things. */
HWND WinGlCreateHiddenWindow(HDEVNOTIFY* hidNotif);

DWORD WinGlGetWindowStyle(std::uint16_t value);
DWORD WinGlGetWindowExStyle(std::uint16_t value);
void WinGlGetFullWindowSize(DWORD style, DWORD exStyle, std::size_t width, std::size_t height, int* fullWidth, int* fullHeight, UINT dpi);
HWND WinGlMakeWindow(std::size_t width, std::size_t height, std::string const& title, std::uint16_t userStyle, bool fullscreen);

/**
 * @brief Specialization of RenderWindow for the Windows API.
 * 
 * Holds an OPENGL Rendering Context if initialized. A valid RenderWindow has a dedicated OPENGL
 * context which is always shared with the default context of the current driver.
 * 
**/
class WinGlRenderWindow : public Clean::RenderWindow
{
    HWND handle;
    HDC dc;
    std::shared_ptr < WinGlContext > context;

public:

    WinGlRenderWindow(HWND window, HDC hdc);

    void setContext(std::shared_ptr < WinGlContext > const& ctxt);

    bool isValid() const;
};

#endif // GLDRIVER_WIN_WINGLRENDERWINDOW_H