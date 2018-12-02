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
    
    class Camera : public WindowListener, 
                   public std::enable_shared_from_this < Camera >,
                   public EffectParameterProvider
    {
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 upVector;
        
        float ratio;
        float fov;
        float near;
        float far;
        
        mutable glm::mat4 matView;
        mutable glm::mat4 matProj;
        
        Property < SharedParameter > matViewParam;
        Property < SharedParameter > matProjParam;
        
    public:
        
        Camera(glm::vec3 const& pos, glm::vec3 const& look = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 const& up = glm::vec3(0.0f, 1.0f, 0.0f));
        
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
        
    protected:
        
        void setViewParam(glm::mat4 const& mat4);
        
        void setProjParam(glm::mat4 const& mat4);
    };
}

#endif // CLEAN_CAMERA_H