 
#ifndef STARTSTOPMANAGER_HPP
#define STARTSTOPMANAGER_HPP

#include "os/fosi.h"
#include <corelib/Time.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <algorithm>

namespace ORO_OS
{
    /**
     * This manager starts and stops all globally registered start/stop
     * functions. This is guaranteed to work, because it uses the Singleton
     * pattern with a global pointer. This pointer is initialised to zero
     * at program startup, before the global constructors are called. The 
     * first time a global constructor calls this manager, it will be properly
     * constructed.
     */
    class StartStopManager
    {
        static StartStopManager* mt;

    public:
        static StartStopManager* Instance();

        static void Release();

        typedef boost::function<int (void)> start_fun;
        typedef boost::function<void (void)> stop_fun;

        /**
         * Register a start function.
         */
        void startFunction( start_fun t )
        {
            startv.push_back(t);
        }

        /**
         * Register a stop function
         */
        void stopFunction( stop_fun t )
        {
            stopv.push_back(t);
        }

        ~StartStopManager()
        {
        }

         void res_collector( start_fun f )
        {
            if ( f() != 0 )
                res = -1;
        }

        static void caller( stop_fun f)
        {
            f();
        }

        int start()
        {
            std::for_each(startv.begin(), startv.end(), boost::function<void (start_fun)>( std::bind1st(std::mem_fun( &StartStopManager::res_collector ), this) ) );
            return res;
        }

        void stop()
        {
            std::for_each(stopv.begin(), stopv.end(), boost::function<void (stop_fun)>( &StartStopManager::caller ) );
        }

    private:

        StartStopManager() : res(0) {}

        int res;
            
        std::vector<start_fun> startv;
        std::vector<stop_fun> stopv;
    };


    /**
     * Use this to register a global init function to the StartStopManager.
     * The function will be called after global variable construction.
     */
    struct InitFunction
    {
        InitFunction( int (*f)(void) ) 
        {
            StartStopManager::Instance()->startFunction(f);
        }
    };

    /**
     * Use this to register a global cleanup function to the StartStopManager.
     * The function will be called right before global variable destruction.
     */
    struct CleanupFunction
    {
        CleanupFunction( void (*f)(void) )
        {
            StartStopManager::Instance()->stopFunction(f);
        }
    };




}

#endif
