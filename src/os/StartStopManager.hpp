/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  StartStopManager.hpp 

                        StartStopManager.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
 
#ifndef STARTSTOPMANAGER_HPP
#define STARTSTOPMANAGER_HPP

#include "../Time.hpp"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <algorithm>
#include "../rtt-config.h"

namespace RTT
{ namespace OS {
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
    class RTT_API StartStopManager
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
        void startFunction( start_fun t );

        /**
         * @brief Register a stop function
         */
        void stopFunction( stop_fun t );

        /**
         * @brief Call all registered start functions.
         *
         * @return false if one or more failed.
         */
        bool start();

        /**
         * @brief Call all registered stop functions.
         */
        void stop();

    private:

        StartStopManager();

        ~StartStopManager();

        void res_collector( start_fun f )
        {
            if ( f() != 0 )
                res = false;
        }

        static void caller( stop_fun f)
        {
            f();
        }

        bool res;
            
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




}}

#endif
