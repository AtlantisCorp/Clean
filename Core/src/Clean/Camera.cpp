/** 
 * \file Core/Camera.cpp
 * \date 12/02/2018
**/

#include "Camera.h"
#include "Allocate.h"
#include "ShaderParameter.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Clean 
{
    Camera::Camera(glm::vec3 const& pos, glm::vec3 const& look, glm::vec3 const& up_)
    {
        position = pos;
        front = glm::normalize(pos - look);
        worldUp = up_;
        
        yaw = -90.0f;
        pitch = 0.0f;
        
        matViewParam = AllocateShared < EffectParameter >(kEffectViewMat4, ShaderValue{ .mat4 = matView }, kShaderParamMat4);
        matProjParam = AllocateShared < EffectParameter >(kEffectProjectionMat4, ShaderValue{ .mat4 = matProj }, kShaderParamMat4);
        
        makeVectors();
    }
    
    Camera::Camera(Camera const& rhs) 
    {
        position = rhs.position;
        front = rhs.front;
        worldUp = rhs.worldUp;
        
        yaw = rhs.yaw;
        pitch = rhs.pitch;
        
        ratio = rhs.ratio;
        fov = rhs.fov;
        near = rhs.near;
        far = rhs.far;
        
        matProj = glm::perspective(glm::radians(fov), ratio, near, far);
        matViewParam = AllocateShared < EffectParameter >(kEffectViewMat4, ShaderValue{ .mat4 = matView }, kShaderParamMat4);
        matProjParam = AllocateShared < EffectParameter >(kEffectProjectionMat4, ShaderValue{ .mat4 = matProj }, kShaderParamMat4);
        
        makeVectors();
    }
    
    bool Camera::onAction(CameraAction const& action)
    {
        if (action.action == kCameraActionTranslate) 
        {
            position = position + action.translation;
            matView = glm::translate(matView, -action.translation);
            setViewParam(matView);
            return true;
        }
        
        else if (action.action == kCameraActionBackTranslate)
        {
            position = position - action.translation;
            matView = glm::translate(matView, action.translation);
            setViewParam(matView);
            return true;
        }
        
        else if (action.action == kCameraActionRotate)
        {
            yaw   += action.rotation.x;
            pitch += action.rotation.y;
            
            if (constraintPitch)
            {
                if (pitch > constraintPitchValue)
                    pitch = constraintPitchValue;
                
                else if (pitch < -constraintPitchValue)
                    pitch = -constraintPitchValue;
            }
            
            makeVectors();
            return true;
        }
        
        return false;
    }
    
    void Camera::onWindowResize(WindowResizeEvent const& event) 
    {
        ratio = (float) event.newSize.width / (float) event.newSize.height;
        matProj = glm::perspective(glm::radians(fov), ratio, near, far);
        setProjParam(matProj);
    }
    
    void Camera::listen(std::shared_ptr < Window > const& window) 
    {
        auto cameraAsListener = std::static_pointer_cast < WindowListener >(shared_from_this());
        window->addListener(cameraAsListener);
        
        WindowSize size = window->getSize();
        ratio = (float) size.width / (float) size.height;
        matProj = glm::perspective(glm::radians(fov), ratio, near, far);
        setProjParam(matProj);
    }
    
    Camera::SharedParameters Camera::findAllParameters() const 
    {
        SharedParameters params;
        params.push_back(matViewParam.load());
        params.push_back(matProjParam.load());
        return params;
    }
    
    Camera::SharedTexParams Camera::findAllTexturedParameters() const 
    {
        return {};
    }
    
    void Camera::setProjection(float r, float f, float nr, float fr)
    {
        ratio = r; fov = f; near = nr; far = fr;
        matProj = glm::perspective(glm::radians(fov), ratio, near, far);
        setProjParam(matProj);
    }
    
    void Camera::setProjection(float f, float nr, float fr)
    {
        fov = f; near = nr; far = fr;
        matProj = glm::perspective(glm::radians(fov), ratio, near, far);
        setProjParam(matProj);
    }
    
    glm::vec3 Camera::getPosition() const 
    {
        return position;
    }
    
    glm::vec3 Camera::getForward() const 
    {
        return -front;
    }
    
    glm::vec3 Camera::getTarget() const 
    {
        return position + front;
    }
    
    glm::mat4 Camera::getViewMatrix() const 
    {
        return matView;
    }
    
    glm::vec3 Camera::getDirection() const
    {
        return front;
    }
    
    glm::vec3 Camera::getRight() const
    {
        return right;
    }
    
    void Camera::reset()
    {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        makeVectors();
    }
        
    void Camera::setViewParam(glm::mat4 const& mat4) 
    {
        auto param = matViewParam.lock();
        param->mutex.lock();
        param->value.mat4 = mat4;
        param->mutex.unlock();
        matViewParam.unlock();
    }
    
    void Camera::setProjParam(glm::mat4 const& mat4)
    {
        auto param = matProjParam.lock();
        param->mutex.lock();
        param->value.mat4 = mat4;
        param->mutex.unlock();
        matProjParam.unlock();
    }
    
    void Camera::makeVectors()
    {
        float radYaw = glm::radians(yaw), radPitch = glm::radians(pitch);
        glm::vec3 newFront;
        newFront.x = cos(radYaw) * cos(radPitch);
        newFront.y = sin(radPitch);
        newFront.z = sin(radYaw) * cos(radPitch);
        
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
        matView = glm::lookAt(position, position + front, up);
        setViewParam(matView);
    }
}
