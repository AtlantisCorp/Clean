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
#include <iostream>

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
        
        std::size_t moduleCount = core.loadAllModules();
        assert(moduleCount && "No module found.");
        
        {
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
            
            // Now we will load a shader and add it to the pipeline state. For now, we will only load the default shader for
            // two stages, Vertex and Fragment stages. Notes that shader objects are stored as shared pointers.
            
            auto vertexShader = gldriver->findDefaultShaderForStage(kShaderTypeVertex);
            firstCommand.pipeline->shader(kShaderTypeVertex, vertexShader);
            auto fragmentShader = gldriver->findDefaultShaderForStage(kShaderTypeFragment);
            firstCommand.pipeline->shader(kShaderTypeFragment, fragmentShader);
            
            // Now we have to load our vertex buffer from our .obj file. This is done by loading a Mesh structure, and requesting
            // its vertex buffer. As we know our mesh only contains one object, we will directly load its first vertex buffer. We
            // must ensure a loader is available to load our Mesh.
            
            auto mesh = MeshManager::Current().load("Clean://Mesh/Example.obj");
            assert(mesh && "Can't load ./example.obj file.");
            
            // Our mesh is loaded, but all its data is on the RAM. We want those buffers to be on GPU! However, Clean engine is
            // optimized in the case multiple drivers are concurrently running. Thus, we must associate a Mesh to a driver. This
            // will make our Mesh generate buffers from the given driver.
            
            mesh->associate(*gldriver);
            mesh->populateRenderCommand(*gldriver, firstCommand);
            
            // We also want to associate a Material to this Mesh. Normally, Example.obj will load one Material specially for this
            // mesh, named Example. We will bind this Material to the RenderCommand to let it add its parameters directly to the
            // RenderCommand. As EffectParameters are used with shared_ptr, changing one will produce effect into the other. 
            
            auto material = MaterialManager::Current().findByName("Example");
            assert(material && "Material 'Example' not found.");
            firstCommand.parameters.addMaterial(*material);
            
            // VertexDescriptor describe how to render one SubMesh. Mesh::findAssociatedDescriptors return a list of VertexDescriptor
            // which can render every mesh's submeshes for the given driver. Those descriptors needs to be mapped to ShaderAttributes
            // by ShaderMapper. Notices that one descriptor is one submesh, but may represent multiple buffers (if submesh's data are
            // not interleaved inside one buffer). You can also use Mesh::populateRenderCommand(driver, firstCommand, vertexShader) as
            // it will generate a RenderCommand::sub for the given shader from our driver, with the correct subcommand type.
            
            // We have our shader, and our buffer. However, how do we make the link between the two ? Clean engine proposes a simple
            // yet complicated system of a Clean::ShaderMapper class. This class obviously maps a list of buffers onto the given shader,
            // by returning a Clean::AttributesMap. A RenderSubCommand holds only one AttributesMap.
            
            // To create your own ShaderMapper, only creates a new class derived from Clean::ShaderMapper. Override method 'map()' and
            // you're done! When loading your shader, be sure to set its mapper with 'Shader::setMapper(myMapper)'. The mapper is owned
            // by the shader so you don't have to keep a copy. Clean::Shader::map uses this mapper to map the given list of buffers to
            // the attributes in the shader. We have to use this system because only the shader creator can know which attributes are
            // in its shader. You also can use Clean::CbkShaderMapper to set 'map()' to your own callback, thus enabling lambdas to define
            // a new ShaderMapper.
            
            // firstCommand.sub(kSubCommandDrawVertex, vertexShader->map(*vertexDescriptor));
            
            // Now we want to be sure our parameters are correctly set for our shader. We basically have two types of parameters: those
            // which are constants for the whole frame, and the others. Notes that constants for the whole frame may also be constants
            // for all other frames. To set manually a parameter to our shader, we can store a new ShaderParameter in our RenderCommand.
            // However, it implies you know the name of the Shader Parameter for the current pipeline.
            
            // Creating global value for our shaders is done via submitting those values through the EffectSession object. Each driver
            // holds an instance of EffectSession that can be used to set global parameters. When Driver::renderCommand executes, it
            // binds this Session with EffectSession::bind.
            
            EffectSession& session = gldriver->getEffectSession();
            session.add(kEffectProjectionMat4, { .mat4 = {glm::perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f)} });
            session.add(kEffectViewMat4, { .mat4 = {glm::mat4(1.0f)} });
            session.add(kEffectModelMat4, { .mat4 = {glm::mat4(1.0f)} });
            
            // We can add our command to our RenderQueue. This must be done only *after* the command is completed, because
            // RenderQueue adds its command by moving objects. Updating a command already added will not take effect in the
            // RenderQueue.
            
            defaultQueue->addCommand(firstCommand);
            
            // Update loop for our driver. Clean::Driver::update commits all rendering jobs to the renderer, and updates
            // every window created by this driver. Calling Clean::WindowManager::updateAll() would only updates each Window
            // with their update implementation, but would not commit rendering jobs and swap buffers of the rendering swap
            // chains. When you work with multiple driver, you should call Clean::Core::updateAllDrivers().
            
            while (!gldriver->allWindowClosed())
            {
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
