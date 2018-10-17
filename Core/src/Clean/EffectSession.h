/** =======================================================
 *  \file Core/EffectSession.h
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_EFFECTSESSION_H
#define CLEAN_EFFECTSESSION_H

#include "Property.h"
#include "EffectParameter.h"
#include "ShaderParameter.h"

#include <vector>
#include <memory>

namespace Clean 
{
    class RenderPipeline;
    class Material;
    
    /** @brief Manages multiple EffectParameter for a 'session'. 
     *
     * EffectSession is used initially by Driver to manage its global parameters for all shaders
     * used by RenderPipeline. However other EffectSession can be used to manage different groups
     * of parameters. 
     *
    **/
    class EffectSession 
    {
        //! @brief List of EffectParameters we manage. 
        Property < std::vector < std::shared_ptr < EffectParameter > > > globals;
        
    public:
        
        /*! @brief Default constructor. */
        EffectSession() = default;
        
        /*! @brief Copies the EffectSession. */
        EffectSession(EffectSession const& rhs);
        
        /*! @brief Default destructor. */
        ~EffectSession() = default;
        
        /*! @brief Adds a Global Parameter.  
         *
         * \param[in] name given to this parameter. Uses the predefined name prefixed by kEffect* to define
         *      a parameter compatible with ShaderMapper mapping. This lets ShaderMapper optimize the parameter
         *      bound to the RenderPipeline, by already knowing what kind of data is given to the shader program.
         * \param[in] value Value for this parameter. Uses the dedicated constructor or fill in the field corresponding
         *      to the data you want to bind to the program. 
         * \param[in] type When not using a predefined name, type is used to know what kind of data must be send 
         *      to the shader program. Let this field with kShaderParamNull if you use predefined parameters. 
         *
         * \return A pointer to the newly created parameter. 
        **/
        std::weak_ptr < EffectParameter > add(std::string const& name, ShaderValue const& value, std::uint8_t const& type = kShaderParamNull);
        
        /*! @brief Adds a parameter possibly shared with other objects. 
         *
         * \param[in] parameter Pointer to an EffectParameter to add. It is checked that the parameter is not already 
         *      in the session before adding it. If this parameter is null, nothing is done. 
         *
        **/
        std::weak_ptr < EffectParameter > add(std::shared_ptr < EffectParameter > const& parameter);
        
        /*! @brief Removes the parameter designated by name. **/
        void remove(std::string const& name);
        
        /*! @brief Clears all globals in this session. **/
        void clear();
        
        /*! @brief Binds all globals parameters to the given RenderPipeline. */
        void bind(RenderPipeline const& pipeline) const;
        
        /*! @brief Adds Material's parameters to this EffectSession. */
        void addMaterial(Material const& material);
        
        /*! @brief Adds multiple Materials making in sort that there will be not more than one hash
         * registered in this session. */
        void batchAddOneHash(std::vector < std::shared_ptr < EffectParameter > > const& parameters);
    };
}

#endif // CLEAN_EFFECTSESSION_H
