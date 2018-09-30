/** \file Core/Window.h
**/

#ifndef CLEAN_WINDOW_H
#define CLEAN_WINDOW_H

#include "Handled.h"
#include "Emitter.h"
#include "Key.h"

namespace Clean 
{
    /** @brief Defines a Window's size. **/
    struct WindowSize 
    {
        //! @brief Width of the window's frame, in pixels. 
        std::size_t width;
        
        //! @brief Height of the window's frame, in pixels.
        std::size_t height;
    };
    
    /** @brief Defines a Window's position. **/
    struct WindowPosition 
    {
        std::size_t x;
        std::size_t y;
    };
    
    /** @brief Defines the rectangular zone which surrounds the Window's area. **/
    struct WindowFrame 
    {
        WindowPosition origin;
        WindowSize size;
    };
    
    static constexpr const std::uint16_t kWindowStyleBordered = 1 << 0;
    static constexpr const std::uint16_t kWindowStyleTitled = 1 << 1;
    static constexpr const std::uint16_t kWindowStyleResizable = 1 << 2;
    static constexpr const std::uint16_t kWindowStyleClosable = 1 << 3;
    static constexpr const std::uint16_t kWindowStyleDefault = kWindowStyleBordered|kWindowStyleTitled|kWindowStyleResizable|kWindowStyleClosable;
    
    class Window;
    
    /*! @brief An event related to a Window. */
    struct WindowEvent
    {
        //! @brief Our emitter. 
        Window* emitter = nullptr;
    };
    
    /*! @brief A Key event from a window. */
    struct WindowKeyEvent : public WindowEvent 
    {
        //! @brief Our key pressed. 
        Key key;
        
        //! @brief If it was pressed or released ?
        bool pressed;
    };
    
    /*! @brief An exposure event. */
    struct WindowExposureEvent : public WindowEvent 
    {
        //! @brief Was it hidden or unhidden ?
        bool hidden;
    };
    
    /*! @brief A resize event. */
    struct WindowResizeEvent : public WindowEvent 
    {
        WindowSize newSize;
    };
    
    /** @brief A Window's move event. */
    struct WindowMoveEvent : public WindowEvent 
    {
        WindowPosition newPosition;
    };
    
    /*! @brief Listens to a Window object. */
    class WindowListener 
    {
    public:
        
        /*! @brief Default destructor. */
        virtual ~WindowListener() = default;
        
        /*! @brief Called when a window has received a key event. */
        virtual void onWindowKey(WindowKeyEvent const& event) {}
        
        /*! @brief Called when a window has received an exposure event. */
        virtual void onWindowExposure(WindowExposureEvent const& event) {}
        
        /*! @brief Called when a window has received a resize event. */
        virtual void onWindowResize(WindowResizeEvent const& event) {}
        
        /*! @brief Called when a window moved. */
        virtual void onWindowMove(WindowMoveEvent const& event) {}
    };
    
    /** @brief Base interface to a window object. 
     *
     * A window is a basic Clean object where something can be draw. Basically, it can be
     * a GUI window, a surface where you can draw something inside of another surface, or
     * else. A window has a width and a height, but it may have some other classic attributes,
     * like a title, a style, etc.
     *
     * Window does not implement any function by itself. Usually a Window is loaded by a Driver
     * through a RenderWindow derived, but this level of abstraction lets another Window creator
     * available. Core's WindowManager handles all created windows but does not update them. You
     * have to call manually Window::update() or WindowManager::updateAll().
     *
     * WindowManager and Window handles: As any external or native windowing system, Clean associates
     * all Window with a particular handle. This handle, a std::uint16_t value, can be stored by a user
     * to access the Window from the WindowManager anywhere and at anytime. This value is unique for each
     * window and cannot be reused. 
     *
    **/
    class Window : public Handled < Window, std::uint16_t >, 
                   public Emitter < WindowListener >
    {
    public:
        
        /*! @brief Default constructor. */
        Window() = default;
        
        /*! @brief No copy constructor. */
        Window(Window const&) = delete;
        
        /*! @brief Default destructor. */
        virtual ~Window() = default;
        
        /*! @brief Returns the style of the window, if applicable. */
        virtual std::uint16_t getStyle() const = 0;
        
        /*! @brief Returns the title of the window, if applicable. */
        virtual std::string getTitle() const = 0;
        
        /*! @brief Returns true if the window is 'fullscreen' when focused, if applicable. */
        virtual bool isFullscreen() const = 0;
        
        /*! @brief Updates the window, if it has anything to do.
         *
         * \note For implementors, this can be the function where the window sub object is sent
         * a message to update the implementation event queue (like a PollEvent) to send events
         * to Core::EventQueue. For example, on MAC OS, it can be a good place to call for 
         * NSWindow::update. 
         *
        **/
        virtual void update() = 0;
        
        /*! @brief Draws the window content if needed and applicable. 
         *
         * \note Some window systems may require a specific method to draw the window's content,
         * which we provide here. However, i don't know one which needs that (Windows' doesn't, 
         * MAC OS neither). 
         *
        **/
        virtual void draw() = 0;
        
        /*! @brief Destroys the window. 
         *
         * \note Clears all resources attached to this Window. Generally it is different from close,
         * but has the same visual effect. Implementors can use this function to destroy handles from
         * implementation. To only close the window, see \ref close().
         *
        **/
        virtual void destroy() = 0;
        
        /*! @brief Close the window. 
         *
         * \note Closes the window but doesn't clear its resources. It may be or may not be applicable
         * depending on the implementation. A NSView cannot be closed, for example. 
         *
        **/
        virtual void close() = 0;
        
        /*! @brief Hides the window, if applicable. */
        virtual void hide() = 0;
        
        /*! @brief Unhides the window, if applicable. */
        virtual void unhide() = 0;
        
        /*! @brief Changes the window's title, if applicable. */
        virtual void setTitle(std::string const& title) = 0;
        
        /*! @brief Moves the window, if applicable. */
        virtual void move(std::size_t x, std::size_t y) = 0;
        
        /*! @brief Returns the window's position, in pixels. */
        virtual WindowPosition getPosition() const = 0;
        
        /*! @brief Returns the window's size, in pixels. */
        virtual WindowSize getSize() const = 0;
        
        /*! @brief Changes the window's size, if applicable. */
        virtual void resize(std::size_t width, std::size_t height) = 0;
        
        /*! @brief Returns true if the Window is closed. */
        virtual bool isClosed() const = 0;
    };
}

#endif // CLEAN_WINDOW_H
