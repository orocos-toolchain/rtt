/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  StartStopManager.hpp 

                        StartStopManager.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
 
#ifndef STARTSTOPMANAGER_HPP
#define STARTSTOPMANAGER_HPP

#include <corelib/Time.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <algorithm>

namespace ORO_OS
{
    /**
     * @brief This manager starts and stops all globally registered start/stop
     * functions, without a particular order.
     *
     * This is guaranteed to work, because it uses the Singleton
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
         * @brief Register a start function.
         */
        void startFunction( start_fun t )
        {
            startv.push_back(t);
        }

        /**
         * @brief Register a stop function
         */
        void stopFunction( stop_fun t )
        {
            stopv.push_back(t);
        }

        /**
         * @brief Call all registered start functions.
         *
         * @return -1 if one or more failed.
         */
        int start()
        {
            // save some memory trick
            startv.resize( startv.size() );
            stopv.resize( stopv.size() );
            std::for_each(startv.begin(), startv.end(), boost::function<void (start_fun)>( std::bind1st(std::mem_fun( &StartStopManager::res_collector ), this) ) );
            return res;
        }

        /**
         * @brief Call all registered stop functions.
         */
        void stop()
        {
            std::for_each(stopv.begin(), stopv.end(), boost::function<void (stop_fun)>( &StartStopManager::caller ) );
        }

    private:

        StartStopManager() : res(0) {}

        ~StartStopManager()
        {
            startv.clear();
            stopv.clear();
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
