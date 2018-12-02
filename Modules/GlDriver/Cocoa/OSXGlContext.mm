/** \file GlDriver/Cocoa/OSXGlContext.mm
**/

#include "OSXGlContext.h"

#include <Clean/Allocate.h>
using namespace Clean;

void OSXGlFillGlTable(GlPtrTable& gl)
{
    memset(&gl, 0, sizeof(gl));
    
    gl.drawElements = glDrawElements;
    gl.drawArrays = glDrawArrays;
    gl.genTextures = glGenTextures;
    gl.genBuffers = glGenBuffers;
    gl.bindBuffer = glBindBuffer;
    gl.bufferData = glBufferData;
    gl.mapBuffer = glMapBuffer;
    gl.unmapBuffer = glUnmapBuffer;
    gl.deleteBuffers = glDeleteBuffers;
    gl.getIntegerv = glGetIntegerv;
    gl.createProgram = glCreateProgram;
    gl.attachShader = glAttachShader;
    gl.linkProgram = glLinkProgram;
    gl.getProgramiv = glGetProgramiv;
    gl.getProgramInfoLog = glGetProgramInfoLog;
    gl.useProgram = glUseProgram;
    gl.validateProgram = glValidateProgram;
    gl.getUniformLocation = glGetUniformLocation;
    gl.uniform1ui = glUniform1ui;
    gl.uniform1i = glUniform1i;
    gl.uniform1f = glUniform1f;
    gl.uniform2fv = glUniform2fv;
    gl.uniform3fv = glUniform3fv;
    gl.uniform4fv = glUniform4fv;
    gl.uniform2uiv = glUniform2uiv;
    gl.uniform3uiv = glUniform3uiv;
    gl.uniform4uiv = glUniform4uiv;
    gl.uniform2iv = glUniform2iv;
    gl.uniform3iv = glUniform3iv;
    gl.uniform4iv = glUniform4iv;
    gl.uniformMatrix2fv = glUniformMatrix2fv;
    gl.uniformMatrix3fv = glUniformMatrix3fv;
    gl.uniformMatrix4fv = glUniformMatrix4fv;
    gl.uniformMatrix2x3fv = glUniformMatrix2x3fv;
    gl.uniformMatrix3x2fv = glUniformMatrix3x2fv;
    gl.uniformMatrix2x4fv = glUniformMatrix2x4fv;
    gl.uniformMatrix4x2fv = glUniformMatrix4x2fv;
    gl.uniformMatrix3x4fv = glUniformMatrix3x4fv;
    gl.uniformMatrix4x3fv = glUniformMatrix4x3fv;
    gl.getError = glGetError;
    gl.enableVertexAttribArray = glEnableVertexAttribArray;
    gl.vertexAttribPointer = glVertexAttribPointer;
    gl.disableVertexAttribArray = glDisableVertexAttribArray;
    gl.polygonMode = glPolygonMode;
    gl.getAttribLocation = glGetAttribLocation;
    gl.activeTexture = glActiveTexture;
    gl.deleteProgram = glDeleteProgram;
    gl.bindTexture = glBindTexture;
    gl.getTexLevelParameteriv = glGetTexLevelParameteriv;
    gl.pixelStorei = glPixelStorei;
    gl.texParameteri = glTexParameteri;
    gl.texImage2D = glTexImage2D;
    gl.generateMipmap = glGenerateMipmap;
    gl.deleteTextures = glDeleteTextures;
    gl.genVertexArrays = glGenVertexArrays;
    gl.bindVertexArray = glBindVertexArray;
    gl.deleteVertexArrays = glDeleteVertexArrays;
    gl.createShader = glCreateShader;
    gl.shaderSource = glShaderSource;
    gl.compileShader = glCompileShader;
    gl.getShaderiv = glGetShaderiv;
    gl.getShaderInfoLog = glGetShaderInfoLog;
    gl.deleteShader = glDeleteShader;
}

/*! @brief Fills attribs with the corresponding NSOpenGLPixelFormatAttribute values
 *  for the given pixelFormat. 
 *
 * \param[in] pixelFormat PixelFormat from which we are based to fill attribs. 
 * \param[in] attribs A non-null array of NSOpenGLPixelFormatAttribute to receive those
 *      attributes. If attribs is null, a new array is allocated and deletion is up to the
 *      caller. 
 * \param[in] attribsCount If attribs is not null, this is the size of the array.
 *
 * \return Number of attributes filled in the array, not counting the last zero.
 *
**/
std::size_t OSXPixelFormatAttributes(PixelFormat const& pixelFormat, NSOpenGLPixelFormatAttribute* attribs, std::size_t attribsCount)
{
    assert(attribs && attribsCount && "Illegal value attribs or attribsCount.");
    memset(attribs, 0, attribsCount * sizeof(NSOpenGLPixelFormatAttribute));
    std::size_t currentAttrib = 0;
    
    if (pixelFormat.buffers == 2) {
        attribs[currentAttrib] = NSOpenGLPFADoubleBuffer;
        currentAttrib++;
    } else if (pixelFormat.buffers >= 3) {
        attribs[currentAttrib] = NSOpenGLPFATripleBuffer;
        currentAttrib++;
    }
    
    if (pixelFormat.multisampled) {
        attribs[currentAttrib + 0] = NSOpenGLPFAMultisample;
        attribs[currentAttrib + 1] = NSOpenGLPFASamples;
        attribs[currentAttrib + 2] = static_cast < NSOpenGLPixelFormatAttribute >(pixelFormat.samples);
        attribs[currentAttrib + 3] = NSOpenGLPFASampleBuffers;
        attribs[currentAttrib + 4] = static_cast < NSOpenGLPixelFormatAttribute >(pixelFormat.sampleBuffers);
        currentAttrib += 5;
    }
    
    if (pixelFormat.bitsPerPixel) {
        attribs[currentAttrib + 0] = NSOpenGLPFAColorSize;
        attribs[currentAttrib + 1] = static_cast < NSOpenGLPixelFormatAttribute >(pixelFormat.bitsPerPixel);
        currentAttrib += 2;
    }
    
    attribs[currentAttrib + 0] = NSOpenGLPFAOpenGLProfile;
    attribs[currentAttrib + 1] = (NSOpenGLPixelFormatAttribute) NSOpenGLProfileVersion3_2Core;
    attribs[currentAttrib + 2] = NSOpenGLPFAClosestPolicy;
    attribs[currentAttrib + 3] = NSOpenGLPFAAccelerated;
    attribs[currentAttrib + 4] = NSOpenGLPFADepthSize;
    attribs[currentAttrib + 5] = (NSOpenGLPixelFormatAttribute)24;
    currentAttrib += 6;
    
    return currentAttrib - 1;
}

OSXGlContext::OSXGlContext(Clean::PixelFormat const& pixelFormat_) : context(nil)
{
    NSOpenGLPixelFormatAttribute attributes[30];
    std::size_t result = OSXPixelFormatAttributes(pixelFormat_, attributes, 30);
    if (!result) return;
    
    NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if (!pf) return;
    
    NSOpenGLContext* glContext = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    if (!glContext) return;
    
    pixelFormat = pixelFormat_;
    context = glContext;
}

OSXGlContext::OSXGlContext(Clean::PixelFormat const& pixelFormat_, OSXGlContext const& sharedContext) : context(nil)
{
    NSOpenGLPixelFormatAttribute attributes[30];
    bool result = OSXPixelFormatAttributes(pixelFormat_, attributes, 30);
    if (!result) return;
    
    NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if (!pf) return;
    
    NSOpenGLContext* natSharedContext = sharedContext.toNSOpenGLContext();
    NSOpenGLContext* glContext = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:natSharedContext];
    if (!glContext) return;
    
    pixelFormat = pixelFormat_;
    context = glContext;
}

OSXGlContext::OSXGlContext(NSOpenGLContext* natContext)
{
    assert(natContext && "Invalid NSOpenGLContext passed.");
    context = natContext;
}

OSXGlContext::~OSXGlContext() 
{
    context = nil;
}

bool OSXGlContext::isValid() const 
{
    return context != nil;
}

void OSXGlContext::swapBuffers() 
{
    if (context) {
        [context flushBuffer];
    }
}

void OSXGlContext::lock() const
{
    if (context) {
        // [context lock];
        [context makeCurrentContext];
    }
}

void OSXGlContext::unlock() const
{
    if (context) {
        // [context unlock];
    }
}

void OSXGlContext::makeCurrent()
{
    if (context) {
        NSOpenGLContext* currentCtxt = [NSOpenGLContext currentContext];
        if (currentCtxt != context) {
            [context makeCurrentContext];
        }
    }
}

PixelFormat OSXGlContext::getPixelFormat() const
{
    return pixelFormat;
}

id OSXGlContext::toNSOpenGLContext() const 
{
    return context;
}

id OSXGlContext::getNSOpenGLPixelFormat() const 
{
    if (context) {
        return [context pixelFormat];
    }
    
    return nil;
}
