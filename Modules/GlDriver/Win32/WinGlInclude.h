/**
 * \file GlDriver/Win32/WinGlInclude.h
 * \date 11/30/2018
**/

#ifndef GLDRIVER_WIN_WINGLINCLUDE_H
#define GLDRIVER_WIN_WINGLINCLUDE_H

#include "../All/GlInclude.h"

#define WIN32_LEAN_AND_MEAN
#include <wctype.h>
#include <windows.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>

//! @brief Default name for the Window class used by GlDriver.
static const LPWSTR GlDriverWindowClass = L"GlDriverWindowClass";
//! @brief Default GUID to broadcast notifications.
static const GUID GlDriverGUID = { 0x946DADC8, 0xBA47, 0x4744, { 0xA4, 0x6B, 0xEA, 0xE4, 0x2B, 0x49, 0xE2, 0x02 }};

typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTPROC)(HDC);
typedef BOOL (WINAPI * PFNWGLDELETECONTEXTPROC)(HGLRC);
typedef PROC (WINAPI * PFNWGLGETPROCADDRESSPROC)(LPCSTR);
typedef HDC (WINAPI * PFNWGLGETCURRENTDCPROC)(void);
typedef HGLRC (WINAPI * PFNWGLGETCURRENTCONTEXTPROC)(void);
typedef BOOL (WINAPI * PFNWGLMAKECURRENTPROC)(HDC,HGLRC);
typedef BOOL (WINAPI * PFNWGLSHARELISTSPROC)(HGLRC,HGLRC);

/** 
 * @brief Holds function pointers for WGL and its extensions.
 * 
**/
struct WGlPtrTable 
{
    HINSTANCE instance = NULL;

    PFNWGLCREATECONTEXTPROC createContext = NULL;
    PFNWGLDELETECONTEXTPROC deleteContext = NULL;
    PFNWGLGETPROCADDRESSPROC getProcAddress = NULL;
    PFNWGLGETCURRENTDCPROC getCurrentDC = NULL;
    PFNWGLGETCURRENTCONTEXTPROC getCurrentContext = NULL;
    PFNWGLMAKECURRENTPROC makeCurrent = NULL;
    PFNWGLSHARELISTSPROC shareLists = NULL;

    PFNWGLSWAPINTERVALEXTPROC swapIntervalEXT = NULL;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC getPixelFormatAttribivARB = NULL;
    PFNWGLGETEXTENSIONSSTRINGARBPROC getExtensionsStringARB = NULL;
    PFNWGLGETEXTENSIONSSTRINGEXTPROC getExtensionsStringEXT = NULL;
    PFNWGLCREATECONTEXTATTRIBSARBPROC createContextAttribsARB = NULL;

    bool EXT_swap_control;
    bool EXT_colorspace;
    bool ARB_multisample;
    bool ARB_framebuffer_sRGB;
    bool EXT_framebuffer_sRGB;
    bool ARB_pixel_format;
    bool ARB_create_context;
    bool ARB_create_context_profile;
    bool EXT_create_context_es2_profile;
    bool ARB_create_context_robustness;
    bool ARB_create_context_no_error;
    bool ARB_context_flush_control;
};

/** @brief Returns true if extension is supported. */
bool WinGlExtensionSupported(WGlPtrTable& table, const char* ext);
/** @brief Make the WGL function table and returns true on success. */
bool WinGlMakeWGLTable(HWND window, WGlPtrTable& result);
/** @brief Returns address of a WGL or GL function. */
void* WinGlGetProcAddress(WGlPtrTable const& wgl, const char* name);
/** @brief Fills the GlPtrTable with a valid WGlPtrTable. */
void WinGlMakeGLTable(WGlPtrTable const& wgl, GlPtrTable& gl);

// ntdll
typedef LONG (WINAPI * PFNRTLVERIFYVERSIONINFO)(OSVERSIONINFOEXW*,ULONG,ULONGLONG);

// user32
typedef BOOL (WINAPI * PFNSETPROCESSDPIAWARE)(void);
typedef BOOL (WINAPI * PFNCHANGEWINDOWMESSAGEFILTEREX)(HWND,UINT,DWORD,CHANGEFILTERSTRUCT*);
typedef BOOL (WINAPI * PFNENABLENONCLIENTDPISCALING)(HWND);
typedef BOOL (WINAPI * PFNSETPROCESSDPIAWARENESSCONTEXT)(HANDLE);
typedef UINT (WINAPI * PFNGETDPIFORWINDOW)(HWND);
typedef BOOL (WINAPI * PFNADJUSTWINDOWRECTEXFORDPI)(LPRECT,DWORD,BOOL,DWORD,UINT);

struct WinGlNtDllPtrTable 
{
    HINSTANCE instance;
    PFNRTLVERIFYVERSIONINFO RtlVerifyVersionInfo;
};

extern WinGlNtDllPtrTable ntdll;

struct WinGlUser32PtrTable
{
    HINSTANCE instance;
    PFNSETPROCESSDPIAWARE SetProcessDPIAware;
    PFNCHANGEWINDOWMESSAGEFILTEREX ChangeWindowMessageFilterEx;
    PFNENABLENONCLIENTDPISCALING EnableNonClientDpiScaling;
    PFNSETPROCESSDPIAWARENESSCONTEXT SetProcessDpiAwarenessContext;
    PFNGETDPIFORWINDOW GetDpiForWindow;
    PFNADJUSTWINDOWRECTEXFORDPI AdjustWindowRectExForDpi;
};

extern WinGlUser32PtrTable user32;

BOOL WinGlIsWindows10BuildOrGreater(WORD build);
BOOL WinGlIsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp);
void WinGlLoadLibraries();
WCHAR* WinGlWStrFromUTF8(std::string const& str);

#define WinGlIsWindows7OrGreater()                                    \
    WinGlIsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7),         \
                                   LOBYTE(_WIN32_WINNT_WIN7), 0)

#endif // GLDRIVER_WIN_WINGLINCLUDE_H