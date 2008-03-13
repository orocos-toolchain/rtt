/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  CommandRepository.cpp 

                        CommandRepository.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be
 
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
 
 
#include "CommandRepository.hpp"

namespace RTT
{
    CommandRepository::~CommandRepository()
    {
        clear();
    }

    void CommandRepository::clear() {
        while ( !simplecommands.empty() ) {
            delete simplecommands.begin()->second;
            simplecommands.erase( simplecommands.begin() );
        }
        OperationFactory<DispatchInterface*>::clear();
    }

    /**
     * Returns the names of all commands added to this interface.
     * @see getNames() to get a list of all commands available to scripting.
     */
    std::vector<std::string> CommandRepository::getCommands() const {
        return keys( simplecommands );
    }

    /**
     * Query for the existence of a Command in this interface.
     * @see hasMember() to verify if a command is available to scripting as well.
     */
    bool CommandRepository::hasCommand(const std::string& name) const {
        return simplecommands.count(name) == 1;
    }

    void CommandRepository::removeCommand(const std::string& name)
    {
        simplecommands.erase(name);
        OperationFactory<DispatchInterface*>::remove(name);
    }

}
