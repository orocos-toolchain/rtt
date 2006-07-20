/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  CoreRunnableInterface.cxx 

                        CoreRunnableInterface.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include "rtt/RunnableInterface.hpp"
#include "rtt/Logger.hpp"

namespace RTT
{

    RunnableInterface::~RunnableInterface() {
        if ( this->owner_task && this->owner_task->isRunning() ) {
            Logger::In in("~RunnableInterface()");
            Logger::log() << Logger::Critical
                    <<"Activity still running, but RunnableInterface destroyed! Stop the task"
                    " before deleting this object. Crash may be imminent."<<Logger::endl;
        }
        if ( this->owner_task )
            this->owner_task->run(0);
    }
    
  RunnableInterface::RunnableInterface() : owner_task(0) {}

  ActivityInterface* RunnableInterface::getActivity() const { return owner_task; }

  void RunnableInterface::setActivity( ActivityInterface* task ) {
    owner_task = task;
  }
 
}
