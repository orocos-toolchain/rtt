/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  CommandDispatch.hpp 

                        CommandDispatch.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#include "Processor.hpp"
#include "DataSource.hpp"
#include <corelib/CommandInterface.hpp>


namespace ORO_Execution
{
    /**
     * Dispatch a CommandInterface to another processor.
     * Combine with TryCommand, which provides the DataSource for
     * the constructor.
     * The execute() method will return false from the moment
     * on that the dispatched command failed. Hence, this execute()
     * must not wrapped in an AsyncCommandDecorator, but directly
     * executed by the calling processor.
     */
    struct CommandDispatch :
        public CommandInterface
    {
        DataSource<bool>::shared_ptr _dsb;
        bool send;
        Processor* proc;
        CommandInterface* com;
        CommandDispatch(Processor* p, CommandInterface* c,  DataSource<bool>* dsb )
            : _dsb(dsb), send(true), proc(p), com(c) {}
        bool execute() {
            if ( send ) {
                if (proc->process( com ) == true) // try to send each time.
                    {
                        send = false;
                        //cout << "Message Dispatched !" <<endl;
                    }
            }
            // return the accept/reject status.
            return _dsb->get();
        }
        void reset() {
            send = true;
            com->reset();
            _dsb->reset();
        }
        CommandInterface* clone() const {
            return new CommandDispatch( proc, com, _dsb.get() );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDispatch( proc, com, _dsb->copy( alreadyCloned ) );
        }
    };
}
