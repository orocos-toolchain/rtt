/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  CommandDispatch.cxx 

                        CommandDispatch.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 

#include "rtt/CommandDispatch.hpp"
#include "rtt/TryCommand.hpp"
#include "rtt/ConditionComposite.hpp"
#include "rtt/ConditionBoolDataSource.hpp"
#include "rtt/CommandProcessor.hpp"

namespace RTT
{
    

    CommandDispatch::CommandDispatch(CommandProcessor* p, CommandInterface* c, ConditionInterface* cn )
        : send(true), maccepted(false), proc(p), com( c ), mcn(cn), dispatcher(this) {}

    CommandDispatch::~CommandDispatch() {
        delete com;
        delete mcn;
    }

    void CommandDispatch::readArguments()
    {}

    bool CommandDispatch::ready() const
    {
        return proc && send;
    }

    bool CommandDispatch::execute()
    {
        return dispatch();
    }

    bool CommandDispatch::dispatch() {
        //Logger::In in("CommandDispatch");
        //Logger::log() <<Logger::RealTime << "execute()"<<Logger::endl;
        if ( send ) {
            //Logger::log() <<Logger::RealTime << "collecting arguments...";
            com->readArguments();
            //Logger::log() <<Logger::RealTime << "sending...";
            if ( proc->process( &dispatcher ) != 0 ) {
                //Logger::log() <<Logger::RealTime << "accepted"<<Logger::endl;
                // send success !
                maccepted = true;
                //cout << "Message Dispatched !" <<endl;
            }
            else {
                //Logger::log() <<Logger::RealTime << "rejected"<<Logger::endl;
                // send failed ! Target CommandProcessor probably not running, give up.
                // set _result to false, because com will not do this, since not executed.
                dispatcher.mvalid = false;
            }
            send = false;
        }
        // return the (remote)  accept/reject status.
        // initially, it will be true (not yet executed), but may revert to false
        // if the command function returns false.
        return dispatcher.mvalid;
    }

    bool CommandDispatch::sent() const {
        return !send;
    }

    bool CommandDispatch::executed() const {
        return dispatcher.mexecuted;
    }

    bool CommandDispatch::accepted() const {
        return maccepted;
    }

    bool CommandDispatch::valid() const {
        return dispatcher.mexecuted && dispatcher.mvalid;
    }

    bool CommandDispatch::evaluate() const {
        return valid() && mcn->evaluate();
    }

    void CommandDispatch::reset() {
        send = true;
        maccepted = false;
        com->reset();
        dispatcher.reset();
    }

    ConditionInterface* CommandDispatch::createCondition() const
    {
        return mcn->clone();
    }
    

    DispatchInterface* CommandDispatch::clone() const {
        return new CommandDispatch( proc, com->clone(), mcn->clone() );
    }

    DispatchInterface* CommandDispatch::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        return new CommandDispatch( proc, com->copy( alreadyCloned ), mcn->copy(alreadyCloned) );
    }

}
