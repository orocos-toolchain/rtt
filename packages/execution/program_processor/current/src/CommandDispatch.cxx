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
 
 

#include "execution/CommandDispatch.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{
        CommandDispatch::CommandDispatch(Processor* p, CommandInterface* c,  AssignableDataSource<bool>* result )
            : _result(result), send(true), proc(p), com(c) {}

        CommandDispatch::~CommandDispatch() {
            delete com;
        }

        bool CommandDispatch::execute() {
            if ( send ) {
                if ( proc->process( com ) != 0 ) {
                    // send success !
                    send = false;
                    //cout << "Message Dispatched !" <<endl;
                }
                else {
                    // send failed ! Target Processor probably not running, give up.
                    send = false;
                    // set _result to false, because com will not do this itself.
                    _result->set(false);
                    return false;
                }
            }
            // return the (remote)  accept/reject status.
            // initially, it will be true (not yet executed), but may revert to false
            // if the command function returns false.
            return _result->get();
        }

        void CommandDispatch::reset() {
            send = true;
            com->reset();
            // do not reset _result, we do not own it.
        }

        CommandInterface* CommandDispatch::clone() const {
            return new CommandDispatch( proc, com->clone(), _result.get() );
        }

        CommandInterface* CommandDispatch::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDispatch( proc, com->copy( alreadyCloned ), _result->copy( alreadyCloned ) );
        }

}
