/**
 * \file GlDriver/Win32/WinGlRenderWindow.cpp
 * \date 11/30/2018
**/

#include "WinGlRenderWindow.h"

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
using namespace Clean;

static LRESULT CALLBACK defaultWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WinGlRenderWindow* window = (WinGlRenderWindow*) GetPropW(hWnd, L"GlDriverRenderWindow");

    if (!window)
    {
        // Property is not present, this message loop is only for the hidden window and for others during initial
        // window creation. SEE GLFW.
        return defaultWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

bool WinGlRegisterWindowClass()
{
    WNDCLASSEXW wc;

    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC) windowProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = GlDriverWindowClass;

    // Load user-provided icon if available
    wc.hIcon = (HICON) LoadImageW(GetModuleHandleW(NULL),
                          L"GlDriverIcon", IMAGE_ICON,
                          0, 0, LR_DEFAULTSIZE | LR_SHARED);
    if (!wc.hIcon)
    {
        // No user-provided icon found, load default icon
        wc.hIcon = (HICON) LoadImageW(NULL,
                              IDI_APPLICATION, IMAGE_ICON,
                              0, 0, LR_DEFAULTSIZE | LR_SHARED);
    }

    if (!RegisterClassExW(&wc))
    {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Can't register window class.", 0));
        return false;
    }

    return true;
}

HWND WinGlCreateHiddenWindow(HDEVNOTIFY* hidNotif)
{
    HWND window = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, GlDriverWindowClass, L"GlDriver message Window", 
                                    WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
                                    0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!window)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "Can't create Hidden Window.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return NULL;
    }

    ShowWindow(window, SW_HIDE);

    // Register for HID device notifications
    if (hidNotif)
    {
        DEV_BROADCAST_DEVICEINTERFACE_W dbi;
        ZeroMemory(&dbi, sizeof(dbi));
        dbi.dbcc_size = sizeof(dbi);
        dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbi.dbcc_classguid = GlDriverGUID;

        *hidNotif = RegisterDeviceNotification(window, (DEV_BROADCAST_HDR*)&dbi, DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    MSG msg;
    while (PeekMessageW(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return window;
}

DWORD WinGlGetWindowStyle(std::uint16_t value)
{
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX;
    if (value & kWindowStyleResizable) style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
    return style;
}

DWORD WinGlGetWindowExStyle(std::uint16_t value)
{
    DWORD style = WS_EX_APPWINDOW;
    return style;
}

void WinGlGetFullWindowSize(DWORD style, DWORD exStyle, std::size_t width, std::size_t height, int* fullWidth, int* fullHeight, UINT dpi)
{
    RECT rect = { 0, 0, width, height };

    if (WinGlIsWindows10BuildOrGreater(14393))
    user32.AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, dpi);

    else 
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    *fullWidth = rect.right - rect.left;
    *fullHeight = rect.bottom - rect.top;
}

HWND WinGlMakeWindow(std::size_t width, std::size_t height, std::string const& title, std::uint16_t userStyle, bool fullscreen)
{
    DWORD style = WinGlGetWindowStyle(userStyle);
    DWORD exStyle = WinGlGetWindowExStyle(userStyle);

    WCHAR* wideTitle = WinGlWStrFromUTF8(title); 
    if (!wideTitle) return NULL;

    int fullWidth, fullHeight;
    WinGlGetFullWindowSize(style, exStyle, width, height, &fullWidth, &fullHeight, USER_DEFAULT_SCREEN_DPI);

    HWND handle = CreateWindowExW(exStyle, GlDriverWindowClass, wideTitle, style, CW_USEDEFAULT, CW_USEDEFAULT, fullWidth, fullHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
    free(wideTitle);

    if (!handle)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create requested window %s.", title.data());
        NotificationCenter::GetDefault()->send(notif);
        return NULL;
    }

    if (WinGlIsWindows7OrGreater())
    {
        user32.ChangeWindowMessageFilterEx(handle, WM_DROPFILES, MSGFLT_ALLOW, NULL);
        user32.ChangeWindowMessageFilterEx(handle, WM_COPYDATA, MSGFLT_ALLOW, NULL);
    }

    return handle;
}

WinGlRenderWindow::WinGlRenderWindow(HWND window, HDC hdc)
{
    handle = window;
    dc = hdc;

    SetPropW(window, L"GlDriverRenderWindow", this);
}

void WinGlRenderWindow::setContext(std::shared_ptr < WinGlContext > const& ctxt)
{
    std::atomic_store(&context, ctxt);
}

bool WinGlRenderWindow::isValid() const 
{
    return handle && dc;
}