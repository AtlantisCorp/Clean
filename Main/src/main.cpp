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
#include <iostream>

/** @brief Classic listener to display notifications to std::cout. */
class NotificationListener : public Clean::NotificationListener 
{
public:
    
    /*! @brief Default destructor. */
    ~NotificationListener() = default;
    
    /*! @brief Displays notification to std::cout. */
    void process(Clean::Notification const& notification)
    {
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

int main()
{
    try 
    {
        // Creates our NotificationListener before creating the Clean::Core instance. 
        // This way all notifications will be redirected to this listener. Other listeners 
        // can be added after the core creation with Clean::NotificationCenter::addListener. 
        
        auto listener = Clean::AllocateShared < NotificationListener >();
        assert(listener && "Can't allocate NotificationListener.");
        
        // Creates our core object. It automatically loads every modules located under 'Modules/'
        // directory. However, directories can be added later with Clean::ModuleManager::addDirectory. 
        // Clean::ModuleManager::loadAllModules must be called after adding a new directory. 
        
        Clean::Core& core = Clean::Core::Create(listener);
        std::size_t moduleCount = core.getModuleCount();
        assert(moduleCount && "No module found.");
        
        // Try to find our driver 'Clean.GlDriver'. This is the default Clean::Driver for OpenGL. Driver
        // is loaded by 'Clean.GlDriverModule' module at startup. A driver is added to the Clean::DriverManager
        // manager and must be initialized. Multiple drivers can be initialized, thus all object created by 
        // a driver derives from Clean::DriverManaged class. 
    
        auto gldriver = core.findDriver("Clean.GlDriver");
        assert(gldriver->initialize() && "Can't initialize Clean.GlDriver.");
        
        // Creates our RenderWindow. A RenderWindow is accessible via the Clean::WindowManager, as every other
        // derived of Clean::Window, but it is created by the Clean::Driver. Thus, a RenderWindow is also accessible
        // from Clean::Driver. When updating the driver, it will automatically updates its RenderWindows. 
    
        auto window = gldriver->createRenderWindow(1024, 768, "RenderWindow Title");
        assert(window && "Can't create RenderWindow.");
        
        // Update loop for our driver. Clean::Driver::update commits all rendering jobs to the renderer, and updates
        // every window created by this driver. Calling Clean::WindowManager::updateAll() would only updates each Window
        // with their update implementation, but would not commit rendering jobs and swap buffers of the rendering swap 
        // chains. When you work with multiple driver, you should call Clean::Core::updateAllDrivers(). 
    
        while (!gldriver->allWindowClosed())
        {
            gldriver->update();
        }
        
        return 0;
    }
    
    catch(std::exception const& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
}