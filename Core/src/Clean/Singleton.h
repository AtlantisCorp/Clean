/** =======================================================
 *  \file Core/Singleton.h
 *  \date 10/27/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_SINGLETON_H
#define CLEAN_SINGLETON_H

#include <atomic>
#include <cassert>

namespace Clean 
{
    /** @brief Defines an interface for a Singleton-like class.
     *
     * A Singleton is not really a true Singleton. Instead, it is a class wich has to 
     * be initialized by the Core class. The instance is stored in an atomic pointer, and
     * managed by Core. 
     *
    **/
    template < class Derived > 
    class Singleton 
    {
        //! @brief Current pointer to the global instance.
        static std::atomic < Derived* > currentInstance;
        
        //! @brief Makes our Core class a friend.
        friend class Core;
        
    public:
        
        /*! @brief Returns the current instance or throw an exception if not found. */
        static Derived& Current()
        {
            Derived* instance = currentInstance.load();
            assert(instance && "Null Current instance. Perhaps Core class is not created yet.");
            return *instance;
        }
    };
    
    template < class Derived >
    std::atomic < Derived* > Singleton < Derived >::currentInstance = nullptr;
}

#endif // CLEAN_SINGLETON_H
