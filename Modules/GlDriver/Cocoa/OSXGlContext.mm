/** \file GlDriver/Cocoa/OSXGlContext.mm
**/

#include "OSXGlContext.h"

#include <Clean/Allocate.h>
using namespace Clean;

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
