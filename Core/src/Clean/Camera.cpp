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
    Camera::Camera(glm::vec3 const& pos, glm::vec3 const& look, glm::vec3 const& up)
    {
        position = pos;
        target = look;
        upVector = up;
        
        matView = glm::lookAt(position, target, upVector);
        matViewParam = AllocateShared < EffectParameter >(kEffectViewMat4, ShaderValue{ .mat4 = matView }, kShaderParamMat4);
        matProjParam = AllocateShared < EffectParameter >(kEffectProjectionMat4, ShaderValue{ .mat4 = matProj }, kShaderParamMat4);
    }
    
    Camera::Camera(Camera const& rhs) 
    {
        position = rhs.position;
        target = rhs.target;
        upVector = rhs.upVector;
        ratio = rhs.ratio;
        fov = rhs.fov;
        near = rhs.near;
        far = rhs.far;
        
        matView = glm::lookAt(position, target, upVector);
        matProj = glm::perspective(glm::radians(fov), ratio, near, far);
        matViewParam = AllocateShared < EffectParameter >(kEffectViewMat4, ShaderValue{ .mat4 = matView }, kShaderParamMat4);
        matProjParam = AllocateShared < EffectParameter >(kEffectProjectionMat4, ShaderValue{ .mat4 = matProj }, kShaderParamMat4);
    }
    
    bool Camera::onAction(CameraAction const& action)
    {
        if (action.action == kCameraActionTranslate) 
        {
            position = position + action.translation;
            target = target + action.translation;
            matView = glm::translate(matView, action.translation);
            setViewParam(matView);
            return true;
        }
        
        else if (action.action == kCameraActionBackTranslate)
        {
            position = position - action.translation;
            target = target - action.translation;
            matView = glm::translate(matView, -action.translation);
            setViewParam(matView);
            return true;
        }
        
        else if (action.action == kCameraActionRotate)
        {
            matView = glm::rotate(matView, action.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matView = glm::rotate(matView, action.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            setViewParam(matView);
            
            glm::vec3 forward(matView[3][0], matView[3][1], matView[3][2]);
            target = position + forward;
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
        return target - position;
    }
    
    glm::vec3 Camera::getTarget() const 
    {
        return target;
    }
    
    glm::mat4 Camera::getViewMatrix() const 
    {
        return matView;
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
}
