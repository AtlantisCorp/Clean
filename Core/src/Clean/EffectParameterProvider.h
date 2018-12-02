/** \file Core/EffectParameterProvider.h
 *  \date 11/25/2018
**/

#ifndef CLEAN_EFFECTPARAMETERPROVIDER_H
#define CLEAN_EFFECTPARAMETERPROVIDER_H

#include "EffectParameter.h"

#include <vector>
#include <memory>

namespace Clean 
{
    /** @brief Base class for all object that may provide some EffectParameters to an 
     *  EffectSession. 
     *
     * As some classes may provide EffectParameters, this class provide a base for an EffectSession
     * to retrieve their parameters while adding them. This follows the visitor pattern where the
     * session visits the provider to find its parameters. 
     *
     * Actually, Material and Camera derives from this class. Other classes may derive later, like 
     * Light or custom classes. 
     *
    **/
    class EffectParameterProvider
    {
    public:
        
        typedef std::vector < std::shared_ptr < EffectParameter > > SharedParameters;
        typedef std::vector < std::shared_ptr < TexturedParameter > > SharedTexParams;
        typedef std::shared_ptr < EffectParameter > SharedParameter;
        typedef std::shared_ptr < TexturedParameter > SharedTexParam;
        
        /*! @brief Destruct the provider. */
        virtual ~EffectParameterProvider() = default;
        
        /*! @brief Returns all EffectParameters provided by this class. */
        virtual SharedParameters findAllParameters() const = 0;
        
        /*! @brief Returns all TexturedParameters provided by this class. */
        virtual SharedTexParams findAllTexturedParameters() const = 0;
    };
}

#endif // CLEAN_EFFECTPARAMETERPROVIDER_H