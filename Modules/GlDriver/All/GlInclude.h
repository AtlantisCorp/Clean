/** \file GlDriver/GlInclude.h
**/

#ifndef GLDRIVER_GLINCLUDE_H
#define GLDRIVER_GLINCLUDE_H

#if defined(CLEAN_PLATFORM_WIN32)
#   include <GL/glcorearb.h>
#   include <GL/wglext.h>

#elif defined(CLEAN_PLATFORM_MACOS)
#   include <OpenGL/OpenGL.h>
#   include <OpenGL/gl3.h>

#elif defined(CLEAN_PLATFORM_LINUX)
#   include <GL/gl3.h>

#endif

/** @brief Holds all function pointers to OpenGL functions. */
struct GlPtrTable 
{
    PFNGLDRAWELEMENTSPROC drawElements;
    PFNGLDRAWARRAYSPROC drawArrays;
    PFNGLGENTEXTURESPROC genTextures;
    PFNGLGENBUFFERSPROC genBuffers;
    PFNGLBINDBUFFERPROC bindBuffer;
    PFNGLBUFFERDATAPROC bufferData;
    PFNGLMAPBUFFERPROC mapBuffer;
    PFNGLUNMAPBUFFERPROC unmapBuffer;
    PFNGLDELETEBUFFERSPROC deleteBuffers;
    PFNGLGETINTEGERVPROC getIntegerv;
    PFNGLCREATEPROGRAMPROC createProgram;
    PFNGLATTACHSHADERPROC attachShader;
    PFNGLLINKPROGRAMPROC linkProgram;
    PFNGLGETPROGRAMIVPROC getProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC getProgramInfoLog;
    PFNGLUSEPROGRAMPROC useProgram;
    PFNGLVALIDATEPROGRAMPROC validateProgram;
    PFNGLGETUNIFORMLOCATIONPROC getUniformLocation;
    PFNGLUNIFORM1UIPROC uniform1ui;
    PFNGLUNIFORM1IPROC uniform1i;
    PFNGLUNIFORM1FPROC uniform1f;
    PFNGLUNIFORM2FVPROC uniform2fv;
    PFNGLUNIFORM3FVPROC uniform3fv;
    PFNGLUNIFORM4FVPROC uniform4fv;
    PFNGLUNIFORM2UIVPROC uniform2uiv;
    PFNGLUNIFORM3UIVPROC uniform3uiv;
    PFNGLUNIFORM4UIVPROC uniform4uiv;
    PFNGLUNIFORM2IVPROC uniform2iv;
    PFNGLUNIFORM3IVPROC uniform3iv;
    PFNGLUNIFORM4IVPROC uniform4iv;
    PFNGLUNIFORMMATRIX2FVPROC uniformMatrix2fv;
    PFNGLUNIFORMMATRIX3FVPROC uniformMatrix3fv;
    PFNGLUNIFORMMATRIX4FVPROC uniformMatrix4fv;
    PFNGLUNIFORMMATRIX2X3FVPROC uniformMatrix2x3fv;
    PFNGLUNIFORMMATRIX3X2FVPROC uniformMatrix3x2fv;
    PFNGLUNIFORMMATRIX2X4FVPROC uniformMatrix2x4fv;
    PFNGLUNIFORMMATRIX4X2FVPROC uniformMatrix4x2fv;
    PFNGLUNIFORMMATRIX3X4FVPROC uniformMatrix3x4fv;
    PFNGLUNIFORMMATRIX4X3FVPROC uniformMatrix4x3fv;
    PFNGLGETERRORPROC getError;
    PFNGLENABLEVERTEXATTRIBARRAYPROC enableVertexAttribArray;
    PFNGLVERTEXATTRIBPOINTERPROC vertexAttribPointer;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC disableVertexAttribArray;
    PFNGLPOLYGONMODEPROC polygonMode;
    PFNGLGETATTRIBLOCATIONPROC getAttribLocation;
    PFNGLACTIVETEXTUREPROC activeTexture;
    PFNGLDELETEPROGRAMPROC deleteProgram;
    PFNGLBINDTEXTUREPROC bindTexture;
    PFNGLGETTEXLEVELPARAMETERIVPROC getTexLevelParameteriv;
    PFNGLPIXELSTOREIPROC pixelStorei;
    PFNGLTEXPARAMETERIPROC texParameteri;
    PFNGLTEXIMAGE2DPROC texImage2D;
    PFNGLGENERATEMIPMAPPROC generateMipmap;
    PFNGLDELETETEXTURESPROC deleteTextures;
    PFNGLGENVERTEXARRAYSPROC genVertexArrays;
    PFNGLBINDVERTEXARRAYPROC bindVertexArray;
    PFNGLDELETEVERTEXARRAYSPROC deleteVertexArrays;
    PFNGLCREATESHADERPROC createShader;
    PFNGLSHADERSOURCEPROC shaderSource;
    PFNGLCOMPILESHADERPROC compileShader;
    PFNGLGETSHADERIVPROC getShaderiv;
    PFNGLGETSHADERINFOLOGPROC getShaderInfoLog;
    PFNGLDELETESHADERPROC deleteShader;
    PFNGLENABLEPROC enable;
    PFNGLDEPTHFUNCPROC depthFunc;
};

// Helper to check for extension string presence.  Adapted from:
// http://www.opengl.org/resources/features/OGLextensions/
int GlIsExtensionSupported(const char *extList, const char *extension);

#endif // GLDRIVER_GLINCLUDE_H
