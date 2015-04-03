/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  CoreRunnableInterface.cxx

                        CoreRunnableInterface.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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



#include "RunnableInterface.hpp"
#include "../Logger.hpp"
#include <cassert>

namespace RTT {
    using namespace base;

    RunnableInterface::~RunnableInterface() {
        if ( this->owner_act && this->owner_act->isRunning() ) {
            Logger::In in("~RunnableInterface()");
            log(Critical)
            <<"Activity still running, but RunnableInterface destroyed! Stop the task"
            " before deleting this object. Crash may be imminent."<<endlog();
        }
        if ( this->owner_act )
            this->owner_act->disableRun(this);
    }

    RunnableInterface::RunnableInterface() : owner_act(0) {}

    void RunnableInterface::loop() {
        this->step();
    }

    bool RunnableInterface::breakLoop() { return false; }


    bool RunnableInterface::hasWork() {
        return false;
    }

    void RunnableInterface::work(WorkReason reason) {}

    void RunnableInterface::setActivity( ActivityInterface* task ) {
        if (owner_act) {
            // notify old owner he's out.
            owner_act->disableRun(this);
        }

        owner_act = task;
    }

    os::ThreadInterface* RunnableInterface::getThread() const {
        if (owner_act)
            return owner_act->thread();
        return 0;
    }

}
