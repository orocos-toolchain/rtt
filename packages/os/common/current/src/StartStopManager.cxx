/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  StartStopManager.cxx 

                        StartStopManager.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "os/StartStopManager.hpp"

namespace ORO_OS
{

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

    int StartStopManager::start()
    {
        // save some memory trick
        //startv.resize( startv.size() );
        //stopv.resize( stopv.size() );
        std::for_each(startv.begin(), startv.end(), boost::function<void (start_fun)>( std::bind1st(std::mem_fun( &StartStopManager::res_collector ), this) ) );
        return res;
    }

    void StartStopManager::stop()
    {
        std::for_each(stopv.rbegin(), stopv.rend(), boost::function<void (stop_fun)>( &StartStopManager::caller ) );
    }

    StartStopManager::StartStopManager() : res(0) {}

    StartStopManager::~StartStopManager()
    {
        startv.clear();
        stopv.clear();
    }


}
