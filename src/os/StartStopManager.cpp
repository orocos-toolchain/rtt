/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  StartStopManager.cxx 

                        StartStopManager.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "os/StartStopManager.hpp"

namespace RTT
{ namespace OS {

    StartStopManager* StartStopManager::mt;

    StartStopManager* StartStopManager::Instance()
    {
        if ( mt == 0 )
            mt = new StartStopManager();
        return mt;
    }

    void StartStopManager::Release()
    {
        delete mt;
        mt = 0;
    }

    void StartStopManager::startFunction( start_fun t )
    {
        startv.push_back(t);
    }

    void StartStopManager::stopFunction( stop_fun t )
    {
        stopv.push_back(t);
    }

    bool StartStopManager::start()
    {
        // save some memory trick
        //startv.resize( startv.size() );
        //stopv.resize( stopv.size() );
        res = true;
        std::for_each(startv.begin(), startv.end(), boost::function<void (start_fun)>( std::bind1st(std::mem_fun( &StartStopManager::res_collector ), this) ) );
        return res;
    }

    void StartStopManager::stop()
    {
        std::for_each(stopv.rbegin(), stopv.rend(), boost::function<void (stop_fun)>( &StartStopManager::caller ) );
    }

    StartStopManager::StartStopManager() : res(true) {}

    StartStopManager::~StartStopManager()
    {
        startv.clear();
        stopv.clear();
    }


}}
