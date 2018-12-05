/** \file GlDriver/GlDriver.h
**/

#ifndef GLDRIVER_GLDRIVER_H
#define GLDRIVER_GLDRIVER_H

#include <Clean/Driver.h>

#include "GlContext.h"
#include "GlRenderWindow.h"
#include "GlBufferManager.h"
#include "GlShaderManager.h"

class GlDriver : public Clean::Driver 
{
    //! @brief Our main OpenGL Context. This is where all begins. It may be associated to a 
    //! main window if necessary (like on Windows), but not always (like on MAC OS). 
    std::shared_ptr < GlContext > defaultContext;
    
    //! @brief Our main OpenGL Window, if created. OpenGL is always associated to one window
    //! which represents the default framebuffer. When 'makeRenderWindow' is called, the first
    //! render window will be this one and other will have another context shared with defaultContext.
    std::shared_ptr < GlRenderWindow > defaultWindow;
    
    //! @brief Protects defaultContext and defaultWindow.
    mutable std::mutex defaultsMutex;
    
    //! @brief BufferManager for all GlBuffer created by this driver. At destruction of the driver, all
    //! buffers are released automatically. When someone wants a buffer from this driver to be released,
    //! he has to call Driver::releaseBuffer() or Buffer::release(). For example, when Mesh finished to 
    //! use its buffer (as in destructor, or when removing it), it uses Buffer::release() to release the 
    //! buffer if not used anymore.
    GlBufferManager bufferManager;
    
    //! @brief ShaderManager for all GlShader created by this Driver. The only purpose of this manager is
    //! to provide precompiled shaders for all classes. Once a shader is compiled, it is stored in this manager
    //! and can be retrieve by using several find* functions. A shader is also a DriverResource. Once all 
    //! programs have used it (no program uses the compiled shader), it can be still stored if marked persistent,
    //! or be released. A new shader object must be compiled at this time. 
    GlShaderManager shaderManager;
    
    //! @brief Holds default shaders for each stage. 
    std::map < std::uint8_t, std::shared_ptr < GlShader > > defaultShadersMap;
    
    //! @brief Protects defaultShadersMap. 
    mutable std::mutex defaultShadersMapMutex;

#   ifdef CLEAN_PLATFORM_WIN32
    //! Holds a hidden Window helper for some actions.
    HWND hiddenWindow;

    //! Holds the broadcast notification for input controllers.
    HDEVNOTIFY deviceNotifHandle;

    //! Holds function pointers to all WGL functions.
    WGlPtrTable wglTable;

    //! Holds the hiden context tied to the hidden window.
    HGLRC hiddenContext;

#   endif

public:

    //! @brief Holds current OPENGL function pointers. In a single system, loading an OpenGL always load the same 
    //! library. However, as this is not guaranteed, each GlDriver holds its version of the function pointers table.
    //! To get quick access over those pointers, this table is a static table organized as a C structure. As it should be 
    //! writable only by the GlDriver, no concurrency is expected when accessing this data. Just ensure \ref initialize
    //! has been called before.
    GlPtrTable glTable;
    
public:
    
    /*! @brief Initializes an OpenGL Driver. 
     *
     * Depending on the current platform and window system chosen to compile this module, a GlContext
     * and its default GlRenderWindow are created during the process. The current pixel format is used
     * to initialize those objects. 
     * 
     * \notes[OSX] OSXGlContext and OSXGlRenderWindow are used to initialize our objects. Considering events
     * loop, it may be useful to have a valid NSApp while running our engine. However, this application may not 
     * be updated by using NSApplication::update, but rather by updating our RenderWindow directly. 
     *
    **/
    bool initialize();
    
    /*! @brief Destroys every objects created by this driver. */
    void destroy();
    
    /*! @brief Select a pixel format. */
    Clean::PixelFormat selectPixelFormat(Clean::PixelFormat const& pixFormat, Clean::PixelFormatPolicy policy = Clean::kPixelFormatClosest);
    
    /*! @brief Final call to glDraw*. */
    void drawShaderAttributes(Clean::ShaderAttributesMap const& attributes);
    
    /*! @brief Returns a RenderCommand filled with a new GlRenderPipeline. */
    Clean::RenderCommand makeRenderCommand();
    
    /*! @brief Always return 'Clean.GlDriver' string. */
    std::string const getName() const;
    
    /*! @brief Creates a new buffer allocated in VRAM from given buffer. */
    std::shared_ptr < Clean::Buffer > makeBuffer(std::uint8_t type, std::shared_ptr < Clean::Buffer > const& buffer);
    
    /*! @brief Returns the default shader for specified stage. 
     * 
     * Default shaders are loaded at Driver creation, and stored in a defaultShadersMap. This map holds the
     * defaultShaders for each stage. Anyone can modify this map to change the default shader used for each
     * shader stage. Default shaders are considered persistent as the Driver always holds one retain count. 
     *
    **/
    std::shared_ptr < Clean::Shader > findDefaultShaderForStage(std::uint8_t stage) const;
    
    /*! @brief Creates a new Shader from given source text and stage. */
    std::shared_ptr < Clean::Shader > makeShader(const char* src, std::uint8_t stage);
    
    /*! @brief Finds a shader by its original file path. */
    std::shared_ptr < Clean::Shader > findShaderPath(std::string const& origin) const;
    
    /*! @brief Makes a texture from an Image. */
    std::shared_ptr < Clean::Texture > makeTexture(std::shared_ptr < Clean::Image > const& image);
    
protected:
    
    /*! @brief Loads default shaders for this driver. */
    void loadDefaultShaders();
    
    /*! @brief Creates a RenderWindow. */
    std::shared_ptr < Clean::RenderWindow > _createRenderWindow(std::size_t width, std::size_t height, 
        std::string const& title, std::uint16_t style, bool fullscreen) const;
    
    /*! @brief Creates a RenderQueue. */
    std::shared_ptr < Clean::RenderQueue > _createRenderQueue(std::uint8_t type) const;
    
    /*! @brief Loads default GL states like depth-test. */
    void loadDefaultGlStates();
};

#endif // GLDRIVER_GLDRIVER_H
