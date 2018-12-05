/** 
 * \file Core/Camera.h
 * \date 12/02/2018
**/

#ifndef CLEAN_CAMERA_H
#define CLEAN_CAMERA_H

#include "Window.h"
#include "EffectParameterProvider.h"
#include "Property.h"

namespace Clean 
{
    static constexpr const std::uint8_t kCameraActionTranslate = 1;
    static constexpr const std::uint8_t kCameraActionBackTranslate = 2;
    static constexpr const std::uint8_t kCameraActionRotate = 3;
    
    struct CameraAction 
    {
        std::uint8_t action;
        
        union
        {
            glm::vec3 translation;
        
            struct 
            {
                float x;
                float y;
            } rotation;
        };
    };
    
    /** @brief Defines a simple Camera.
     *
     * A Camera is an object that manages a View matrix and a Projection matrix. The View
     * matrix is updated with its Position, Target and Up vectors. The Projection matrix is
     * updated with its Ratio, its FieldOfView, and its Near and Far values.
     *
     * The Camera is a central element that uses only two EffectParameters:
     *  - \ref kEffectProjectionMat4 for the Projection matrix.
     *  - \ref kEffectViewMat4 for the View matrix.
     *
     * As the Projection matrix is always dependant of the Window it is in, the Camera can listen
     * to a Window for its WindowResizeEvent. It will automatically take the ratio from the listened
     * Window to update the Projection matrix.
     *
     * \note Deriving Camera:
     * Camera can be derived from to create custom Cameras, like FPS-like, or following a particular
     * object in a custom way. To derive correctly Camera, you must override:
     *   - \ref onAction if you want to change the default behaviour of your Camera while responding
     *     to some Camera actions.
     *   - \ref onWindowResize if you want to change the default behaviour while the Window is resizing.
     *   - All other methods from WindowListener can be overriden and used to respond to key events.
     *
    **/
    class Camera : public WindowListener, 
                   public std::enable_shared_from_this < Camera >,
                   public EffectParameterProvider
    {
    protected:
        
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 worldUp;
        float yaw, pitch;
        
        float ratio, fov;
        float near, far;
        
        mutable glm::mat4 matView;
        mutable glm::mat4 matProj;
        
        Property < SharedParameter > matViewParam;
        Property < SharedParameter > matProjParam;
        
        std::atomic < bool > constraintPitch;
        std::atomic < float > constraintPitchValue;
        
    public:
        
        Camera(glm::vec3 const& pos,
               glm::vec3 const& look = glm::vec3(0.0f, 0.0f, 1.0f),
               glm::vec3 const& up   = glm::vec3(0.0f, 1.0f, 0.0f));
        
        Camera(Camera const& rhs);
        
        virtual bool onAction(CameraAction const& action);
        
        virtual void onWindowResize(WindowResizeEvent const& event);
        
        virtual void listen(std::shared_ptr < Window > const& window);
        
        /*! @brief Returns all EffectParameters provided by this class. */
        virtual SharedParameters findAllParameters() const;
        
        /*! @brief Returns all TexturedParameters provided by this class. */
        virtual SharedTexParams findAllTexturedParameters() const;
        
        virtual void setProjection(float ratio, float fov, float near, float far);
        
        virtual void setProjection(float fov, float near, float far);
        
        virtual glm::vec3 getPosition() const;
        
        virtual glm::vec3 getForward() const;
        
        virtual glm::vec3 getTarget() const;
        
        virtual glm::mat4 getViewMatrix() const;
        
        virtual glm::vec3 getDirection() const;
        
        virtual glm::vec3 getRight() const;
        
        virtual void reset();
        
    protected:
        
        void setViewParam(glm::mat4 const& mat4);
        
        void setProjParam(glm::mat4 const& mat4);
        
        void makeVectors();
    };
}

#endif // CLEAN_CAMERA_H
