/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  AsynchCommandDecorator.cxx

                        AsynchCommandDecorator.cxx -  description
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
#include "AsynchCommandDecorator.hpp"

namespace RTT
{

	AsynchCommandDecorator::AsynchCommandDecorator(ActionInterface* cmd)
		: command(cmd),executed(false)
	{
	}

	AsynchCommandDecorator::AsynchCommandDecorator(const AsynchCommandDecorator& orig)
		: command( orig.getCommand()->clone() ),
          executed(false)
	{
	}

	AsynchCommandDecorator::~AsynchCommandDecorator()
	{
		delete command;
	}

     ActionInterface* AsynchCommandDecorator::clone() const
     {
         return new AsynchCommandDecorator( command->clone() );
     }

    const ActionInterface* AsynchCommandDecorator::getCommand() const
    {
        return command;
    }

    void AsynchCommandDecorator::readArguments()
    {}

	bool AsynchCommandDecorator::execute()
    {
     	if (!executed) {
            //this clause should be executed once
            command->readArguments();
     		status = command->execute();
            executed = true;
     	}
        return status;
    }

    void AsynchCommandDecorator::reset()
    {
        executed = false;
    }

    ActionInterface* AsynchCommandDecorator::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
    {
      return new AsynchCommandDecorator( command->copy( alreadyCloned ) );
    }
}
