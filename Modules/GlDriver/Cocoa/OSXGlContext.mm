/** \file GlDriver/Cocoa/OSXGlContext.mm
**/

#include "OSXGlContext.h"

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
std::size_t OSXPixelFormatAttributes(Clean::PixelFormat const& pixelFormat, NSOpenGLPixelFormatAttribute*& attribs, std::size_t attribsCount)
{
    static constexpr const std::size_t maxPFASize = 30;
    if (!attribs) { 
        attribs = Clean::Allocate < NSOpenGLPixelFormatAttribute >(maxPFASize);
        attribsCount = maxPFASize;
    }
    
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
    attribs[currentAttrib + 2] = NSOpenGLPFAMaximumPolicy;
    currentAttrib + 3;
    
    return currentAttrib - 1;
}

OSXGlContext::OSXGlContext(Clean::PixelFormat const& pixelFormat) : context(nil)
{
    NSOpenGLPixelFormatAttribute attributes[30];
    std::size_t result = OSXPixelFormatAttributes(pixelFormat, attributes, 30);
    if (!result) return;
    
    NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if (!pf) return;
    
    NSOpenGLContext* glContext = [[NSOpenGLContext alloc] initWithFormat:pf];
    if (!glContext) return;
    
    context = glContext;
}

OSXGlContext::OSXGlContext(Clean::PixelFormat const& pixelFormat, OSXGlContext const& sharedContext) : context(nil)
{
    NSOpenGLPixelFormatAttribute attributes[30];
    bool result = OSXPixelFormatAttributes(pixelFormat, attributes, 30);
    if (!result) return;
    
    NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if (!pf) return;
    
    NSOpenGLContext* natSharedContext = sharedContext.toNSOpenGLContext();
    NSOpenGLContext* glContext = [[NSOpenGLContext alloc] initWithFormat:pf
                                                            shareContext:natSharedContext];
    if (!glContext) return;
    
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

id OSXGlContext::toNSOpenGLContext() const 
{
    return context;
}