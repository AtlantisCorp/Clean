/** 
 * Clean C++ Engine
 *
 * Clean Engine is a graphics engine powered by external modules. It is organised with several
 * principles that must be followed everywhere:
 *
 * - Core Clean engine is a set of managers and interfaces used to load specific objects, mainly 
 *   implemented in an external module. 
 * - All modules are using CLEAN macros defined in CMake/CleanMacros.cmake. Those macros can help
 *   finding the current platform (CLEAN_PLATFORM_*), the current language (CLEAN_LANG_*) and the 
 *   current window system (CLEAN_WINDOW_*). Notes that more than one window system may be available. 
 *
 * A Clean project is divised in several parts:
 * - The main executable: Under directory 'Main/', it contains all the sources for the main executable. 
 *   The program should initialize the Clean engine here. 
 * - Modules loaded by the main executable under 'Modules/'. It contains every module projects the main
 *   executable needs. For example, the OpenGL driver might be under 'Modules/GlDriver/'. 
 * - The Core of Clean engine, under 'Core/'. This is where the Clean engine sources must be.
 *
 * A Clean project always follow the CMake/ExecutableTemplate.cmake template. A Clean module must follow
 * the CMake/ModuleTemplate.cmake template. 
 *
 * Modules can be added to the module directory independantly. An executable may link against some modules
 * if they need to have some special methods from those modules. For example, setting on MAC OS an NSView 
 * as the NativeWindow for a Clean::RenderWindow. 
 *
 * Modules are compiled for a triplet Platform, Language and WindowSystem. If more than one Window system
 * is available, by default the first one is chosen. However, CMake can let you choose which window system
 * might be used. 
 *
 * The Clean::Driver object is responsible for creating RenderWindow and acts as a factory for all objects
 * that are specific to the render system chosen. By so, it also manages and owns all those objects. 
 *
 * Clean engine uses the C++ exception system to throw errors. However, some functions might not throw errors
 * when they are expected (and thus not 'fatal'). Clean::NotificationCenter is used to send informations or 
 * error messages, but fatal error are still thrown by exception. 
 *
 * This file contains a very simple sample that only creates a Clean::RenderWindow and updates it untill the
 * user closes it. It will be improved with the engine. 
 * 
**/

#include <Clean/NotificationListener.h>
#include <Clean/Core.h>
#include <Clean/Allocate.h>
#include <Clean/Mesh.h>
#include <Clean/Camera.h>
#include <iostream>
#include <chrono>

#include <glm/gtc/matrix_transform.hpp>

/** @brief Classic listener to display notifications to std::cout. */
class NotificationListener : public Clean::NotificationListener 
{
public:
    
    /*! @brief Default destructor. */
    ~NotificationListener() noexcept = default;
    
    /*! @brief Displays notification to std::cout. */
    void process(Clean::Notification const& notification)
    {
        if (notification.level == Clean::kNotificationLevelInfo)
            return;
        
        static constexpr const char* strLevel[] = { "info", "warn", "erro", "fatal" };
        std::string function = notification.function.substr(0, 10);
        
        if (notification.function.size() > 10)
            function += "...";
        
        std::cout << "{" << strLevel[notification.level] << "}" 
            << "[" << function << "] " 
            << notification.message 
            << std::endl;
    }
};

/** @brief A simple class for all objects that may be time-based updated. */
class TimeUpdatable
{
public:
    
    virtual ~TimeUpdatable() = default;
    
    virtual void update(std::chrono::milliseconds const& deltaTime) = 0;
};

static constexpr const float kFPSCameraDefaultSpeed = 0.007f;
static constexpr const float kFPSCameraDefaultSensitivity = 0.04f;

/** @brief An example of a very simple FPS-like Camera. */
class FPSCamera : public Clean::Camera, public TimeUpdatable
{
    std::map < std::uint16_t, bool > keymap;
    std::atomic < float > speed, sensitivity;
    
public:
    
    FPSCamera(glm::vec3 const& pos, glm::vec3 const& look = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 const& up = glm::vec3(0.0f, 1.0f, 0.0f))
    : Clean::Camera(pos, look, up)
    {
        keymap[Clean::kKeyW] = false;
        keymap[Clean::kKeyS] = false;
        keymap[Clean::kKeyA] = false;
        keymap[Clean::kKeyD] = false;
        speed = kFPSCameraDefaultSpeed;
        sensitivity = kFPSCameraDefaultSensitivity;
    }
    
    void onWindowKey(Clean::WindowKeyEvent const& event)
    {
        if (event.pressed)
        {
            if (event.key == Clean::kKeyW)
            keymap[Clean::kKeyW] = true;
            
            else if (event.key == Clean::kKeyS)
            keymap[Clean::kKeyS] = true;
            
            else if (event.key == Clean::kKeyA)
            keymap[Clean::kKeyA] = true;
            
            else if (event.key == Clean::kKeyD)
            keymap[Clean::kKeyD] = true;
            
            else if (event.key == Clean::kKeyShift)
            speed = speed * 2;
        }
        
        else if (!event.pressed)
        {
            if (event.key == Clean::kKeyW)
            keymap[Clean::kKeyW] = false;
            
            else if (event.key == Clean::kKeyS)
            keymap[Clean::kKeyS] = false;
            
            else if (event.key == Clean::kKeyA)
            keymap[Clean::kKeyA] = false;
            
            else if (event.key == Clean::kKeyD)
            keymap[Clean::kKeyD] = false;
            
            else if (event.key == Clean::kKeyU)
            invert();
            
            else if (event.key == Clean::kKeyShift)
            speed = speed / 2;
        }
    }
    
    void invert()
    {
        worldUp = -worldUp;
        makeVectors();
    }
    
    void update(std::chrono::milliseconds const& deltaTime)
    {
        if (keymap[Clean::kKeyW])
        {
            Clean::CameraAction forward;
            forward.action = Clean::kCameraActionTranslate;
            forward.translation = speed * deltaTime.count() * getDirection();
            onAction(forward);
        }
        
        if (keymap[Clean::kKeyS])
        {
            Clean::CameraAction forward;
            forward.action = Clean::kCameraActionBackTranslate;
            forward.translation = speed * deltaTime.count() * getDirection();
            onAction(forward);
        }
        
        if (keymap[Clean::kKeyA])
        {
            Clean::CameraAction left;
            left.action = Clean::kCameraActionBackTranslate;
            left.translation = speed * deltaTime.count() * getRight();
            onAction(left);
        }
        
        if (keymap[Clean::kKeyD])
        {
            Clean::CameraAction right;
            right.action = Clean::kCameraActionTranslate;
            right.translation = speed * deltaTime.count() * getRight();
            onAction(right);
        }
    }
    
    void onWindowMouseMoved(Clean::WindowMouseMovedEvent const& event)
    {
        Clean::CameraAction mouseMoved;
        mouseMoved.action = Clean::kCameraActionRotate;
        mouseMoved.rotation.x = sensitivity * event.deltaX;
        mouseMoved.rotation.y = -sensitivity * event.deltaY;
        onAction(mouseMoved);
    }
    
    void setSpeed(float value)
    {
        speed = value;
    }
    
    void setSensitivity(float value)
    {
        sensitivity = value;
    }
    
    void reset()
    {
        speed = kFPSCameraDefaultSpeed;
        sensitivity = kFPSCameraDefaultSensitivity;
        Camera::reset();
    }
};

int main()
{
    try 
    {
        using namespace Clean;
        // Creates our NotificationListener before creating the Clean::Core instance. 
        // This way all notifications will be redirected to this listener. Other listeners 
        // can be added after the core creation with Clean::NotificationCenter::addListener. 
        
        auto listener = Clean::AllocateShared < ::NotificationListener >();
        assert(listener && "Can't allocate NotificationListener.");
        
        // Creates our core object. It automatically loads every modules located under 'Modules/'
        // directory. However, directories can be added later with Clean::ModuleManager::addDirectory. 
        // Clean::ModuleManager::loadAllModules must be called after adding a new directory. 
        
        Clean::Core& core = Clean::Core::Create(listener);
        
        Clean::FileSystem& fs = core.getCurrentFileSystem();
        fs.addRealPath("Mesh", "../Meshes");
        fs.addRealPath("Material", "../Materials");
        fs.addRealPath("Shader", "../Shaders");
        fs.addRealPath("Texture", "../Textures");
        
        std::size_t moduleCount = core.loadAllModules();
        assert(moduleCount && "No module found.");
        
        {
            // Try to find our driver 'Clean.GlDriver'. This is the default Clean::Driver for OpenGL. Driver
            // is loaded by 'Clean.GlDriverModule' module at startup. A driver is added to the Clean::DriverManager
            // manager and must be initialized. Multiple drivers can be initialized, thus all object created by
            // a driver derives from Clean::DriverManaged class.
            
            Clean::PixelFormat format;
            format.multisampled = true;
            format.sampleBuffers = 1;
            format.samples = 8;
            format.buffers = 2;
            format.bitsPerPixel = 32;
            
            auto gldriver = core.findDriver("Clean.GlDriver");
            gldriver->selectPixelFormat(format);
            assert(gldriver->initialize() && "Can't initialize Clean.GlDriver.");
            
            // Creates our RenderWindow. A RenderWindow is accessible via the Clean::WindowManager, as every other
            // derived of Clean::Window, but it is created by the Clean::Driver. Thus, a RenderWindow is also accessible
            // from Clean::Driver. When updating the driver, it will automatically updates its RenderWindows.
            
            auto window = gldriver->createRenderWindow(1024, 768, "RenderWindow Title");
            assert(window && "Can't create RenderWindow.");
            window->show();
            
            // Now that we have a window, and a driver. We must create our default RenderQueue and store its handle, in order
            // to communicate RenderCommands to the driver. The driver will dispatch each command to the given target, which
            // might be a RenderWindow, or a generic RenderTarget.
            
            auto defaultQueue = gldriver->makeRenderQueue(kRenderQueuePriorityHighest, kRenderQueueStatic);
            
            // A RenderCommand is defined by a target to commit to, a pipeline state, and multiple RenderSubCommands. Each of
            // those sub commands defines one draw. A draw is a work submitted to a driver with a valid target, pipeline state,
            // vertex buffer. Some other buffers may be given like index buffer and texture buffers.
            
            auto firstCommand = gldriver->makeRenderCommand();
            firstCommand.target = window;
            
            // We load a ShaderMapper with Predefined Shaders to complete our RenderPipeline.
            // See the ShaderMapper file to see what is happening. We also ensure this loading is correct by
            // checking the return value.
            
            bool result = firstCommand.pipeline->buildMapper("Clean://Shader/GLSL/LearnOpenGL/GettingStarted-Textures.json");
            assert(result && "pipeline->buildMapper() failed.");
            
            // Now we have to load our vertex buffer from our .obj file. This is done by loading a Mesh structure, and requesting
            // its vertex buffer. As we know our mesh only contains one object, we will directly load its first vertex buffer. We
            // must ensure a loader is available to load our Mesh.
            
            std::string meshfile = "Clean://Mesh/Cube.obj";
            auto mesh = MeshManager::Current().load(meshfile);
            assert(mesh && "Can't load mesh file.");
            
            // Our mesh is loaded, but all its data is on the RAM. We want those buffers to be on GPU! However, Clean engine is
            // optimized in the case multiple drivers are concurrently running. Thus, we must associate a Mesh to a driver. This
            // will make our Mesh generate buffers from the given driver.
            
            mesh->associate(*gldriver);
            mesh->populateRenderCommand(*gldriver, firstCommand);
            
            // We also want to associate a Material to this Mesh. Normally, Example.obj will load one Material specially for this
            // mesh, named Example. We will bind this Material to the RenderCommand to let it add its parameters directly to the
            // RenderCommand. As EffectParameters are used with shared_ptr, changing one will produce effect into the other. 
            
            auto material = MaterialManager::Current().findByName("cube");
            assert(material && "Material 'Example' not found.");
            firstCommand.parameters.add(*material);
            
            // Tries to load our Texture. 
            
            auto texture = gldriver->makeTexture("Clean://Texture/Cube.png");
            material->setAmbientTexture(texture);
            
            // We can add our command to our RenderQueue. This must be done only *after* the command is completed, because
            // RenderQueue adds its command by moving objects. Updating a command already added will not take effect in the
            // RenderQueue.
            
            defaultQueue->addCommand(firstCommand);
            
            // Input subsystem is controlled by each Window. Therefore, a Camera can be connected to a Window input. Each Camera
            // action can be connected to an input key. However, as some keyboards have different keys, an action layout can be
            // loaded by Camera for each languages.
            
            auto camera = AllocateShared < FPSCamera >(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            camera->setProjection(60.0f, 0.1f, 1000.0f);
            camera->listen(window);
            
            auto& effSession = gldriver->getEffectSession();
            effSession.add(*camera);
            
            /* 
            
            auto locale = Locale::Current();
            std::string layout = BuildString("Clean://Layout/%s.layout", locale.string());
            camera->loadLayout(layout);
            camera->listen(window);
            
            */
            
            // Update loop for our driver. Clean::Driver::update commits all rendering jobs to the renderer, and updates
            // every window created by this driver. Calling Clean::WindowManager::updateAll() would only updates each Window
            // with their update implementation, but would not commit rendering jobs and swap buffers of the rendering swap
            // chains. When you work with multiple driver, you should call Clean::Core::updateAllDrivers().
            
            auto lastTime = std::chrono::high_resolution_clock::now();
            
            while (!gldriver->allWindowClosed())
            {
                auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastTime);
                lastTime = std::chrono::high_resolution_clock::now();
                
                camera->update(deltaTime);
                gldriver->update();
            }
        }
        
        core.destroy();
        return 0;
    }
    
    catch(std::exception const& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
}
