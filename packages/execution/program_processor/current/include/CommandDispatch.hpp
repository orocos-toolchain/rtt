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

#ifndef EXECUTION_COMMAND_DISPATCH 
#define EXECUTION_COMMAND_DISPATCH 
 
#include "DataSource.hpp"
#include <corelib/CommandInterface.hpp>


namespace ORO_Execution
{
    class Processor;

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
        public ORO_CoreLib::CommandInterface
    {
        DataSource<bool>::shared_ptr _result;
        bool send;
        Processor* proc;
        ORO_CoreLib::CommandInterface* com;
        CommandDispatch(Processor* p, CommandInterface* c,  DataSource<bool>* result );

        /**
         * Be sure only to delete this command if the target processor is
         * not processing the encapsulated command.
         */
        ~CommandDispatch();

        bool execute();

        void reset();

        ORO_CoreLib::CommandInterface* clone() const;

        ORO_CoreLib::CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };
}

#endif
