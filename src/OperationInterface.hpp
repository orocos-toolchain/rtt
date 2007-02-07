/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  OperationInterface.hpp 

                        OperationInterface.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
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
 
 
#ifndef ORO_OPERATION_INTERFACE_HPP
#define ORO_OPERATION_INTERFACE_HPP

#include <string>
#include "CommandRepository.hpp"
#include "MethodRepository.hpp"

namespace RTT
{
    /**
     * The interface for accessing and executing 'operations', being commands
     * and methods.
     */
    class OperationInterface
    {
    public:
        virtual ~OperationInterface() {}
        virtual const std::string& getName() const = 0;
        virtual const std::string& getDescription() const = 0;
        virtual CommandRepository* commands() = 0;

        virtual MethodRepository* methods() = 0;
    };


    /**
     * A task object groups a set of commands and methods (operations)
     * which may be invoked.
     */
    class TaskObject
        : public OperationInterface
    {
        std::string mname;
        std::string mdescription;

        CommandRepository mcommands;

        MethodRepository mmethods;
    public:
        TaskObject(std::string name, std::string description ="A Task Object.")
            : mname(name), mdescription(description)
        {}

        const std::string& getName() const { return mname; }

        const std::string& getDescription() const { return mdescription; }

        void setDescription(const std::string& d) { mdescription = d;}

        virtual CommandRepository* commands() { return &mcommands; }

        virtual MethodRepository* methods() { return &mmethods;}
    };
}


#endif
