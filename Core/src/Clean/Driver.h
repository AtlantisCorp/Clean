/** \file Core/Driver.h
**/

#ifndef CLEAN_DRIVER_H
#define CLEAN_DRIVER_H

#include "RenderWindow.h"
#include "RenderWindowManager.h"
#include "RenderSurface.h"
#include "PixelFormat.h"
#include "RenderQueueManager.h"

namespace Clean
{       
    //! @brief Driver's state group. @{
    
    //! @brief Driver is not yet inited (Driver::initialize() was not called).
    static constexpr const std::uint8_t kDriverStateNotInited = 0;
    //! @brief Driver has been inited. 
    static constexpr const std::uint8_t kDriverStateInited = 1;
    //! @brief Driver has been destroyed. 
    static constexpr const std::uint8_t kDriverStateDestroyed = 2;
    
    //! @}
    
    /** @brief Main implementation-specific interface of the Clean engine. 
     *
     * A Driver is responsible for the creation of most of the objects used later by
     * the engine. RenderWindow, RenderQueue, RenderTarget, Buffer, etc. are all constructed
     * and managed by this class. 
     *
     * When a driver is initialized, it ensure all startup conditions required are met. Then,
     * it allocates minimal resources. This can be the main context for an OpenGL driver, or a
     * D3D device for DirectX. Additional requirements might occurs, like ensuring a NSApp object
     * is available when running on MAC OS. 
     * 
     * When constructing an object, it will always manages it through managers. For example, the
     * RenderWindowManager is a special manager to handle RenderWindows. Driver holds one instance
     * of this class to handle all RenderWindows. In this case, a RenderWindow will be handled by 
     * the RenderWindowManager and by the Core's WindowManager. \see RenderWindow for more explanations
     * on destroying the RenderWindow. 
     *
     * Practically, user can't know how the driver is implemented. To do so, he would have to include
     * the used driver header, and thus link against this driver's library. This would make the program
     * dependent of the used driver, which we do not want to. To prevent this problem, we let the user
     * choose the driver by its name or by its UUID. The driver manages all implemented objects related
     * to himself, but not more. 
     *
     * Example of an OpenGL powered driver: 
     * - initialize creates a single OpenGL context for a main window. The pixel format used is the
     *      one selected with Driver::selectPixelFormat(), or the closest pixel format matching this
     *      one. The main window is not displayed on screen, but is virtually present. 
     * - destroy destroys this OpenGL context and all other resources associated to it. 
     * - createRenderWindow creates a new RenderWindow only if the main RenderWindow has already been
     *      returned. The first RenderWindow returned by createRenderWindow is the one associated with
     *      the OpenGL context, all others are indirectly associated by a different mechanism (like a bigger
     *      framebuffer, or something else).
     *
     * However, some options may be used depending on the driver implementation. A good example could be
     * an option 'AllowMultipleContexts' to allow the driver to create shared OpenGL contexts for each 
     * RenderWindow created. However, resources shared may be implemented differently. 
     *
     * NativeSurface and RenderSurface, an attempt for customization
     *
     * When RenderWindow is not enough because you want some fancy native GUI in your rendering window,
     * you can use instead RenderSurface (if available from the driver) to create a rendering surface inside
     * the parent surface you gives. From the implementor's side, it means the driver should be able to 
     * create a rendering surface on a compatible surface (like a HWND on Windows or NSView for MAC OS). 
     * However, user must provide the native parent handle and its program won't be portable anymore. RenderSurfaces
     * are handled like RenderWindow, and are managed by the RenderWindowManager (because RenderSurface are
     * derived of RenderWindow but with no updates, no title, no style, etc.). A RenderSurface may be the 
     * main RenderWindow returned for an OpenGL driver if it was the first created. 
    **/
    class Driver 
    {
    protected:
        
        //! @brief Current driver's state. It can be kDriverStateNotInited, kDriverStateInited,
        //! kDriverStateDestroyed. 
        std::atomic < std::uint8_t > state;
        
        //! @brief A Manager for RenderWindows. It typically is the same manager as WindowManager,
        //! but can accept RenderWindow instead. When the driver is destroyed, this manager clears
        //! all its shared_ptr and calls RenderWindow::destroy() for each of them before. 
        RenderWindowManager renderWindows;
        
        //! @brief PixelFormat selected. May be modified by derived classes. Uses pixelFormatMutex 
        //! to modify the pixel format. 
        PixelFormat pixelFormat;
        
        //! @brief Protects pixelFormat.
        mutable std::mutex pixelFormatMutex;
        
        //! @brief Handles RenderQueue registered in this driver. 
        RenderQueueManager renderQueues;
        
    public:
        
        /*! @brief Default constructor. */
        Driver();
        
        /*! @brief Default destructor. */
        virtual ~Driver() = default;
        
        /*! @brief Returns state. */
        std::uint8_t getState() const;
        
        /*! @brief Initializes the driver. */
        virtual bool initialize() = 0;
        
        /*! @brief Destroys the driver and all its resources. */
        virtual void destroy() = 0;
        
        /*! @brief Creates a RenderWindow ready to be drawn on. 
         * 
         * The RenderWindow returned is not displayed. You must call RenderWindow::show() 
         * to display it on screen. However, rendering can still occur while offscreen. The
         * window is also added to Core's WindowManager. 
         * 
         * \param width Width for this window, in pixels.
         * \param height Height for this window, in pixels.
         * \param title Title for the window, if applicable. 
         * \param style Style for the window. 
         * \param fullscreen Boolean indicating weither the current RenderWindow should be
         *      in fullscreen or not. If fullscreen, the RenderWindow will change the display 
         *      settings when focused. 
         * 
         * \return A valid pointer to a RenderWindow, or null if it failed. It also may send
         *      notifications about the error. 
        **/
        virtual std::shared_ptr < RenderWindow > createRenderWindow(std::size_t width, std::size_t height, 
            std::string const& title, std::uint16_t style = kWindowStyleDefault, bool fullscreen = false);
        
        /*! @brief Selects the given PixelFormat as the main pixel format for this Driver. 
         *
         * \note If a pixel format has already been selected and the driver is already inited, it throws an 
         * exception of type DriverAlreadyInitedException.
         *
         * \param pixFormat PixelFormat to select. If it cannot be found, the closest policy 
         *      should be applied, but it is left to implementors to do the best. 
         *
         * \return PixelFormat selected, or PixelFormat::Invalid if none was selected. 
        **/
        virtual PixelFormat selectPixelFormat(PixelFormat const& pixFormat, PixelFormatPolicy policy = kPixelFormatClosest) = 0;
        
        /*! @brief Returns currently selected PixelFormat. */
        virtual PixelFormat getPixelFormat() const;
        
        /*! @brief Returns true if all windows in renderWindows are closed. */
        virtual bool allWindowClosed() const;
        
        /*! @brief Performs an update operation on all this driver's resources. 
         *
         * This includes: - commits of all RenderQueue registered to the driver. 
         *                - swaps buffers for all RenderWindow registered. 
         *                - updates all windows registered. 
         *
         * All work stored in RenderQueue is updated. Rendered objects must registers
         * new RenderCommands to the render queues as buffers have changed. Notes that 
         * some render queues might be static and as this, their RenderCommands never
         * get cleared. 
         *
        **/
        virtual void update();
        
        /*! @brief Commits all RenderQueue objects to the driver. */
        virtual void commitAllQueues();
        
        /*! @brief Creates a RenderQueue. 
         *
         * \param[in] priority RenderQueueManager stores RenderQueue inside a map where each key is the queue
         *      priority. Queue with the highest priority will be draw before queue with the lowest priority. When
         *      a Driver does not have time to render lower priority queues, it may ignore them to gain time. However,
         *      this may lead to commands not executed at all. By default, a driver will try to execute all commands
         *      before presenting the job to the screen. 
         * \param[in] type RenderQueue can be of two types: static or dynamic. When static, a queue is never cleared
         *      and its commands are re-executed at next frame. When dynamic, its commands are executed only once. 
         *
         * \note A RenderQueue is valid only for the driver that created this queue. When the driver is destroyed, it 
         *      calls RenderQueue::release() to release specific resources used by the RenderQueue. 
         *
         * \return A valid pointer to a new RenderQueue, or nullptr.
        **/
        virtual std::shared_ptr < RenderQueue > makeRenderQueue(std::uint8_t priority, std::uint8_t type);
        
        /*! @brief Commits given RenderQueue to this driver, if the queue is registered by this driver. */
        virtual void commit(std::shared_ptr < RenderQueue > const& queue);
        
        /*! @brief Renders a RenderCommand. */
        virtual void renderCommand(RenderCommand const& command);
        
        /*! @brief Draws a ShaderAttributesMap element. 
         *
         * Called in \ref renderCommand(), this function assumes everything is set up to draw some vertexes or some
         * indexes. It represents the final draw call when rendering a RenderSubCommand, and can only be implemented
         * by a derived Driver. 
         *
         * Only two elements in ShaderAttributesMap should be used by this function: indexInfos if indexInfos.elements
         * is superior to zero (which means indexed drawing), or elements if indexInfos.elements is zero (which means
         * normal vertex draw).
         *
        **/
        virtual void drawShaderAttributes(ShaderAttributesMap const& attributes) = 0;
        
        /*! @brief Creates a new RenderCommand. */
        virtual RenderCommand makeRenderCommand() = 0;
        
        /*! @brief Returns the Driver's name. */
        virtual std::string const getName() const = 0;
        
        /*! @brief Creates a new buffer or return null on failure.
         *
         * \param[in] type Type of the Buffer we want to create.
         * \param[in] buffer Buffer we want to copy.
         *
        **/
        virtual std::shared_ptr < Buffer > makeBuffer(std::uint8_t type, std::shared_ptr < Buffer > const& buffer) = 0;
        
        /*! @brief Should return the default shader for the given stage. */
        virtual std::shared_ptr < Shader > findDefaultShaderForStage(std::uint8_t stage) const = 0;
        
        /*! @brief Delegate method for a DriverResource created by this Driver. 
         *
         * Called when a DriverResource's use count is zero, returns true if the resource is allowed to release
         * its data, or false if it musn't. It can be used for a Driver to allow, for example, persistent data 
         * without needing at least one external count. For example, a persistent Buffer can be used to permanently 
         * store static data, even when no mesh uses it. 
         *
        **/
        virtual bool shouldReleaseResource(DriverResource const& resource) const;
        
        /*! @brief Creates a new Shader from given source text and stage. */
        virtual std::shared_ptr < Shader > makeShader(const char* src, std::uint8_t stage) = 0;
        
    protected:
        
        /*! @brief Creates a RenderWindow from implementation. */
        virtual std::shared_ptr < RenderWindow > _createRenderWindow(std::size_t width, std::size_t height, 
            std::string const& title, std::uint16_t style, bool fullscreen) const = 0;
        
        /*! @brief Implementation of createRenderQueue. 
         *
         * This function is called when \ref createRenderQueue() creates the RenderQueue. It is overridden by the derived
         * driver to provide its custom RenderQueue object. Notes that returning a null RenderQueue is an error and will 
         * lead to an exception throw. 
         * 
         * \param[in] type RenderQueue can be of two types: static or dynamic. When static, a queue is never cleared
         *      and its commands are re-executed at next frame. When dynamic, its commands are executed only once.
         *
         * \return A valid pointer to a new RenderQueue, or nullptr.
        **/
        virtual std::shared_ptr < RenderQueue > _createRenderQueue(std::uint8_t type) const = 0;
    };
}

#endif // CLEAN_DRIVER_H
