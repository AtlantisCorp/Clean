/**
 * \file GlDriver/Win32/WinGlInclude.cpp
 * \date 11/30/2018
**/

#include "WinGlInclude.h"

#include <cassert>

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
using namespace Clean;

WinGlNtDllPtrTable ntdll;

bool WinGlExtensionSupported(WGlPtrTable& table, const char* ext)
{
    const char* exts = NULL;

    if (table.getExtensionsStringARB) 
        exts = table.getExtensionsStringARB(table.getCurrentDC());
    else if (table.getExtensionsStringEXT)
        exts = table.getExtensionsStringEXT();

    if (!exts)
        return false;

    return (bool) GlIsExtensionSupported(exts, ext);
}

bool WinGlMakeWGLTable(HWND window, WGlPtrTable& result)
{
    if (result.instance)
        return true;

    result.instance = (HINSTANCE) LoadLibraryA("opengl32.dll");

    if (!result.instance)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "opengl32.dll not found.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return false;
    }

    result.createContext = (PFNWGLCREATECONTEXTPROC) GetProcAddress(result.instance, "wglCreateContext");
    result.deleteContext = (PFNWGLDELETECONTEXTPROC) GetProcAddress(result.instance, "wglDeleteContext");
    result.getProcAddress = (PFNWGLGETPROCADDRESSPROC) GetProcAddress(result.instance, "wglGetProcAddress");
    result.getCurrentDC = (PFNWGLGETCURRENTDCPROC) GetProcAddress(result.instance, "wglGetCurrentDC");
    result.getCurrentContext = (PFNWGLGETCURRENTCONTEXTPROC) GetProcAddress(result.instance, "wglGetCurrentContext");
    result.makeCurrent = (PFNWGLMAKECURRENTPROC) GetProcAddress(result.instance, "wglMakeCurrent");
    result.shareLists = (PFNWGLSHARELISTSPROC) GetProcAddress(result.instance, "wglShareLists");

    HDC dc = GetDC(window);

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;

    if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd))
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to set pixel format.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return false;
    }

    HGLRC rc = result.createContext(dc);

    if (!rc)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create dummy context.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return false;
    }

    HDC pdc = result.getCurrentDC();
    HGLRC prc = result.getCurrentContext();

    if (!result.makeCurrent(pdc, prc))
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to make dummy context current.", 0);
        NotificationCenter::GetDefault()->send(notif);

        result.makeCurrent(pdc, prc);
        result.deleteContext(rc);
        return false;
    }

    result.swapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) result.getProcAddress("wglSwapIntervalEXT");
    result.getPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC) result.getProcAddress("wglGetPixelFormatAttribivARB");
    result.getExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) result.getProcAddress("wglGetExtensionsStringARB");
    result.getExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) result.getProcAddress("wglGetExtensionsStringEXT");
    result.createContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) result.getProcAddress("wglCreateContextAttribsARB");

    result.EXT_swap_control = WinGlExtensionSupported(result, "WGL_EXT_swap_control");
    result.EXT_colorspace = WinGlExtensionSupported(result, "WGL_EXT_colorspace");
    result.ARB_multisample = WinGlExtensionSupported(result, "WGL_ARB_multisample");
    result.ARB_framebuffer_sRGB = WinGlExtensionSupported(result, "WGL_ARB_framebuffer_sRGB");
    result.EXT_framebuffer_sRGB = WinGlExtensionSupported(result, "WGL_EXT_framebuffer_sRGB");
    result.ARB_pixel_format = WinGlExtensionSupported(result, "WGL_ARB_pixel_format");
    result.ARB_create_context = WinGlExtensionSupported(result, "WGL_ARB_create_context");
    result.ARB_create_context_profile = WinGlExtensionSupported(result, "WGL_ARB_create_context_profile");
    result.EXT_create_context_es2_profile = WinGlExtensionSupported(result, "WGL_EXT_create_context_es2_profile");
    result.ARB_create_context_robustness = WinGlExtensionSupported(result, "WGL_ARB_create_context_robustness");
    result.ARB_create_context_no_error = WinGlExtensionSupported(result, "WGL_ARB_create_context_no_error");
    result.ARB_context_flush_control = WinGlExtensionSupported(result, "WGL_ARB_context_flush_control");

    result.makeCurrent(pdc, prc);
    result.deleteContext(rc);
    return true;
}

void* WinGlGetProcAddress(WGlPtrTable const& wgl, const char* name)
{
    void *p = (void *)wgl.getProcAddress(name);

    if(p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1) )
    p = (void *)GetProcAddress((HMODULE)wgl.instance, name);

    return p;
}

void WinGlMakeGLTable(WGlPtrTable const& wgl, GlPtrTable& gl)
{
    assert(wgl.instance && wgl.getProcAddress);
    memset(&gl, 0, sizeof(gl));

    gl.drawElements = (PFNGLDRAWELEMENTSPROC) WinGlGetProcAddress(wgl, "glDrawElements");
    gl.drawArrays = (PFNGLDRAWARRAYSPROC) WinGlGetProcAddress(wgl, "glDrawArrays");
    gl.genTextures = (PFNGLGENTEXTURESPROC) WinGlGetProcAddress(wgl, "glGenTextures"); 
    gl.genBuffers = (PFNGLGENBUFFERSPROC) WinGlGetProcAddress(wgl, "glGenBuffers");
    gl.bindBuffer = (PFNGLBINDBUFFERPROC) WinGlGetProcAddress(wgl, "glBindBuffer");
    gl.bufferData = (PFNGLBUFFERDATAPROC) WinGlGetProcAddress(wgl, "glBufferData");
    gl.mapBuffer = (PFNGLMAPBUFFERPROC) WinGlGetProcAddress(wgl, "glMapBuffer");
    gl.unmapBuffer = (PFNGLUNMAPBUFFERPROC) WinGlGetProcAddress(wgl, "glUnmapBuffer");
    gl.deleteBuffers = (PFNGLDELETEBUFFERSPROC) WinGlGetProcAddress(wgl, "glDeleteBuffers");
    gl.getIntegerv = (PFNGLGETINTEGERVPROC) WinGlGetProcAddress(wgl, "glGetIntegerv");
    gl.createProgram = (PFNGLCREATEPROGRAMPROC) WinGlGetProcAddress(wgl, "glCreateProgram");
    gl.attachShader = (PFNGLATTACHSHADERPROC) WinGlGetProcAddress(wgl, "glAttachShader");
    gl.linkProgram = (PFNGLLINKPROGRAMPROC) WinGlGetProcAddress(wgl, "glLinkProgram");
    gl.getProgramiv = (PFNGLGETPROGRAMIVPROC) WinGlGetProcAddress(wgl, "glGetProgramiv");
    gl.getProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) WinGlGetProcAddress(wgl, "glGetProgramInfoLog");
    gl.useProgram = (PFNGLUSEPROGRAMPROC) WinGlGetProcAddress(wgl, "glUseProgram");
    gl.validateProgram = (PFNGLVALIDATEPROGRAMPROC) WinGlGetProcAddress(wgl, "glValidateProgram");
    gl.getUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) WinGlGetProcAddress(wgl, "glGetUniformLocation");
    gl.uniform1ui = (PFNGLUNIFORM1UIPROC) WinGlGetProcAddress(wgl, "glUniform1ui");
    gl.uniform1i = (PFNGLUNIFORM1IPROC) WinGlGetProcAddress(wgl, "glUniform1i");
    gl.uniform1f = (PFNGLUNIFORM1FPROC) WinGlGetProcAddress(wgl, "glUniform1f");
    gl.uniform2fv = (PFNGLUNIFORM2FVPROC) WinGlGetProcAddress(wgl, "glUniform2fv");
    gl.uniform3fv = (PFNGLUNIFORM3FVPROC) WinGlGetProcAddress(wgl, "glUniform3fv");
    gl.uniform4fv = (PFNGLUNIFORM4FVPROC) WinGlGetProcAddress(wgl, "glUniform4fv");
    gl.uniform2uiv = (PFNGLUNIFORM2UIVPROC) WinGlGetProcAddress(wgl, "glUniform2uiv");
    gl.uniform3uiv = (PFNGLUNIFORM3UIVPROC) WinGlGetProcAddress(wgl, "glUniform3uiv");
    gl.uniform4uiv = (PFNGLUNIFORM4UIVPROC) WinGlGetProcAddress(wgl, "glUniform4uiv");
    gl.uniform2iv = (PFNGLUNIFORM2IVPROC) WinGlGetProcAddress(wgl, "glUniform2iv");
    gl.uniform3iv = (PFNGLUNIFORM3IVPROC) WinGlGetProcAddress(wgl, "glUniform3iv");
    gl.uniform4iv = (PFNGLUNIFORM4IVPROC) WinGlGetProcAddress(wgl, "glUniform4iv");
    gl.uniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix2fv");
    gl.uniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix3fv");
    gl.uniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix4fv");
    gl.uniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix2x3fv");
    gl.uniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix3x2fv");
    gl.uniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix2x4fv");
    gl.uniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix4x2fv");
    gl.uniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix3x4fv");
    gl.uniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) WinGlGetProcAddress(wgl, "glUniformMatrix4x3fv");
    gl.getError = (PFNGLGETERRORPROC) WinGlGetProcAddress(wgl, "glGetError");
    gl.enableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) WinGlGetProcAddress(wgl, "glEnableVertexAttribArray");
    gl.vertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) WinGlGetProcAddress(wgl, "glVertexAttribPointer");
    gl.disableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) WinGlGetProcAddress(wgl, "glDisableVertexAttribArray");
    gl.polygonMode = (PFNGLPOLYGONMODEPROC) WinGlGetProcAddress(wgl, "glPolygonMode");
    gl.getAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) WinGlGetProcAddress(wgl, "glGetAttribLocation");
    gl.activeTexture = (PFNGLACTIVETEXTUREPROC) WinGlGetProcAddress(wgl, "glActiveTexture");
    gl.deleteProgram = (PFNGLDELETEPROGRAMPROC) WinGlGetProcAddress(wgl, "glDeleteProgram");
    gl.bindTexture = (PFNGLBINDTEXTUREPROC) WinGlGetProcAddress(wgl, "glBindTexture");
    gl.getTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) WinGlGetProcAddress(wgl, "glGetTexLevelParameteriv");
    gl.pixelStorei = (PFNGLPIXELSTOREIPROC) WinGlGetProcAddress(wgl, "glPixelStorei");
    gl.texParameteri = (PFNGLTEXPARAMETERIPROC) WinGlGetProcAddress(wgl, "glTexParameteri");
    gl.texImage2D = (PFNGLTEXIMAGE2DPROC) WinGlGetProcAddress(wgl, "glTexImage2D");
    gl.generateMipmap = (PFNGLGENERATEMIPMAPPROC) WinGlGetProcAddress(wgl, "glGenerateMipmap");
    gl.deleteTextures = (PFNGLDELETETEXTURESPROC) WinGlGetProcAddress(wgl, "glDeleteTextures");
    gl.genVertexArrays = (PFNGLGENVERTEXARRAYSPROC) WinGlGetProcAddress(wgl, "glGenVertexArrays");
    gl.bindVertexArray = (PFNGLBINDVERTEXARRAYPROC) WinGlGetProcAddress(wgl, "glBindVertexArray");
    gl.deleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) WinGlGetProcAddress(wgl, "glDeleteVertexArrays");
    gl.createShader = (PFNGLCREATESHADERPROC) WinGlGetProcAddress(wgl, "glCreateShader");
    gl.shaderSource = (PFNGLSHADERSOURCEPROC) WinGlGetProcAddress(wgl, "glShaderSource");
    gl.compileShader = (PFNGLCOMPILESHADERPROC) WinGlGetProcAddress(wgl, "glCompileShader");
    gl.getShaderiv = (PFNGLGETSHADERIVPROC) WinGlGetProcAddress(wgl, "glGetShaderiv");
    gl.getShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) WinGlGetProcAddress(wgl, "glGetShaderInfoLog");
    gl.deleteShader = (PFNGLDELETESHADERPROC) WinGlGetProcAddress(wgl, "glDeleteShader");
    gl.enable = (PFNGLENABLEPROC) WinGlGetProcAddress(wgl, "glEnable");
    gl.depthFunc = (PFNGLDEPTHFUNCPROC) WinGlGetProcAddress(wgl, "glDepthFunc");
}

BOOL WinGlIsWindows10BuildOrGreater(WORD build)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 10, 0, build };
    DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER;
    ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
    cond = VerSetConditionMask(cond, VER_BUILDNUMBER, VER_GREATER_EQUAL);
    // HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
    //       latter lies unless the user knew to embedd a non-default manifest
    //       announcing support for Windows 10 via supportedOS GUID
    return ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}

BOOL WinGlIsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0, {0}, sp };
    DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
    ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
    cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
    // HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
    //       latter lies unless the user knew to embedd a non-default manifest
    //       announcing support for Windows 10 via supportedOS GUID
    return ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}

void WinGlLoadLibraries()
{
    if (!ntdll.instance)
    {
        ntdll.instance = LoadLibraryA("ntdll.dll");

        if (!ntdll.instance)
        {
            Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to load ntdll.dll.", 0);
            NotificationCenter::GetDefault()->send(notif);
            return;
        }

        ntdll.RtlVerifyVersionInfo = (PFNRTLVERIFYVERSIONINFO) GetProcAddress(ntdll.instance, "RtlVerifyVersionInfo");
    }

    if (!user32.instance)
    {
        user32.instance = LoadLibraryA("user32.dll");

        if (!user32.instance)
        {
            Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to load user32.dll.", 0);
            NotificationCenter::GetDefault()->send(notif);
            return;
        }

        user32.AdjustWindowRectExForDpi = (PFNADJUSTWINDOWRECTEXFORDPI) GetProcAddress(user32.instance, "AdjustWindowRectExForDpi");
        user32.ChangeWindowMessageFilterEx = (PFNCHANGEWINDOWMESSAGEFILTEREX) GetProcAddress(user32.instance, "ChangeWindowMessageFilterEx");
        user32.EnableNonClientDpiScaling = (PFNENABLENONCLIENTDPISCALING) GetProcAddress(user32.instance, "EnableNonClientDpiScaling");
        user32.GetDpiForWindow = (PFNGETDPIFORWINDOW) GetProcAddress(user32.instance, "GetDpiForWindow");
        user32.SetProcessDPIAware = (PFNSETPROCESSDPIAWARE) GetProcAddress(user32.instance, "SetProcessDPIAware");
        user32.SetProcessDpiAwarenessContext = (PFNSETPROCESSDPIAWARENESSCONTEXT) GetProcAddress(user32.instance, "SetProcessDpiAwarenessContext");
    }
}

WCHAR* WinGlWStrFromUTF8(std::string const& str)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, NULL, 0);

    if (!count)
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create Wide String from UTF8.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return NULL;
    }

    WCHAR* target = (WCHAR*) calloc(count, sizeof(WCHAR));

    if (!MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, target, count))
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create Wide String from UTF8.", 0);
        NotificationCenter::GetDefault()->send(notif);
        free(target);
        return NULL;
    }

    return target;
}