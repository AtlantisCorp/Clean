/** \file GlDriver/Cocoa/OSXGlRenderWindow.h
**/

#ifndef GLDRIVER_COCOA_OSXGLRENDERWINDOW_H
#define GLDRIVER_COCOA_OSXGLRENDERWINDOW_H

#include "OSXPrerequisites.h"
#include "OSXGlContext.h"
#include "../All/GlRenderWindow.h"

class OSXGlRenderWindow : public GlRenderWindow 
{
    //! @brief An id representing a NSWindow.
    id nativeWindow;
    
    //! @brief The nativeWindow delegate. 
    id nativeWindowDelegate;
    
    //! @brief The NSOpenGLContext associated to this render window.
    std::shared_ptr < OSXGlContext > nativeContext;
    
    //! @brief True if in Fullscreen mode, false otherwise.
    std::atomic < bool > fullscreenMode;
    
    //! @brief Our specialized view.
    id nativeView;
    
public:
    
    /*! @brief Constructs an OSXGlRenderWindow from the given native context. */
    OSXGlRenderWindow(std::shared_ptr < OSXGlContext > const& context);
    
    /*! @brief Constructs an OSXGlRenderWindow from the given native context and its parameters. */
    OSXGlRenderWindow(std::shared_ptr < OSXGlContext > const& context, std::size_t width, std::size_t height, std::uint16_t style, std::string const& title);
    
    /*! @brief Constructs an OSXGlRenderWindow from the given native RenderWindow. */
    OSXGlRenderWindow(OSXGlRenderWindow const& rhs);
    
    /*! @brief Destructs the window. */
    ~OSXGlRenderWindow();
    
    /*! @brief Returns true if all components could be created (native* variables). */
    bool isValid() const;
    
    /*! @brief Returns the title of the window. */
    std::string getTitle() const;
    
    /*! @brief Returns always false. 
     *
     * As of modern macOS versions, there is no more real 'fullscreen' modes. All we can do is rely on 
     * the 'go-to-fullscreen' new resize green button on the window title bar. This handles for us the
     * fullscreen mode without having to deal with pixel modes.
     *
    **/
    bool isFullscreen() const;
    
    /*! @brief Updates the message loop for this Window. 
     * 
     * Basically it calls NSWindow::update to send NSWindowUpdateNotification to the default notification
     * center. OSXWindowDelegate normally handles this update notification by also updating the NSOpenGLContext.
     *
    **/
    void update();
    
    /*! @brief On macOS, currently does nothing as everything is done in NSOpenGLContext. */
    void draw();
    
    /*! @brief Closes the window and nullize its id. */
    void destroy();
    
    /*! @brief Closes the window but do not nullize its id. */
    void close();
    
    /*! @brief Minimizes the window. */
    void hide();
    
    /*! @brief Restores the window if minimized. */
    void unhide();
    
    /*! @brief Changes the window's title. */
    void setTitle(std::string const& title);
    
    /*! @brief Changes the Window's coordinates. */
    void move(std::size_t x, std::size_t y);
    
    /*! @brief Returns current position. */
    Clean::WindowPosition getPosition() const;
    
    /*! @brief Returns current size. */
    Clean::WindowSize getSize() const;
    
    /*! @brief Resizes the window. */
    void resize(std::size_t width, std::size_t height);
    
    /*! @brief Returns true only if nativeWindow is nil or released. */
    bool isClosed() const;
    
    /*! @brief Returns 1, 2 or 3 buffers. */
    std::size_t getBuffersCount() const;
    
    /*! @brief Swaps buffers for this window. */
    void swapBuffers();
    
    /*! @brief Locks the NSOpenGLContext for this window for caller thread. */
    void lock();
    
    /*! @brief Unlocks NSOpenGLContext from called thread. */
    void unlock();
    
    /*! @brief Actually makes current the NSOpenGLContext of this window. */
    void bind(Clean::Driver& driver) const;
    
    /*! @brief Returns the current Window's style. */
    std::uint16_t getStyle() const;
    
    /*! @brief Returns our OSXGlContext. */
    std::shared_ptr < OSXGlContext > getOSXGlContext() const;
    
    /*! @brief Should make the Window ready to draw and receive events. */
    void show() const;
    
    /*! @brief Sets the Window into fullscreen mode or windowed mode. */
    void setFullscreen(bool value);
    
    /*! @brief Prepares the RenderTarget for drawing.
     *
     * Clears OpenGL buffers, and set the viewport to the correct contentView size. OpenGL
     * can be used to draw on the window after this function.
     *
    **/
    void prepare(Clean::Driver& driver) const;
    
#   ifdef CLEAN_LANG_OBJC
    /*! @brief Notifies this render window its NSWindow will close. */
    void notifyClose(id delegate);
    
    /*! @brief Changes the fullscreenMode value. */
    void notifyFullscreen(id delegate, bool value);
    
#   endif
};

#endif // GLDRIVER_COCOA_OSXGLRENDERWINDOW_H
