/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  DispatchInterface.hpp 

                        DispatchInterface.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_EXECUTION_DISPATCHINTERFACE_HPP
#define ORO_EXECUTION_DISPATCHINTERFACE_HPP
 
#include <corelib/CommandInterface.hpp>
namespace ORO_CoreLib { class ConditionInterface; }

namespace ORO_Execution
{
    /**
     * Dispatch a CommandInterface to a CommandProcessor and track its status.
     * The execute() method will return false from the moment
     * on that the dispatched command failed. 
     */
    class DispatchInterface
        : public ORO_CoreLib::CommandInterface
    {
    public:
        /**
         * Dispatch a command. If it is not accepted, fail, if it is accepted,
         * return true
         */
        virtual bool dispatch() = 0;

        /**
         * After reset(), another attempt to dispatch
         * the command will be made upon execute().
         */
        virtual void reset() = 0;

        /**
         * Returns true if the command was sent to the CommandProcessor.
         * You can use this flag to check whether execute() was invoked.
         */
        virtual bool sent() const = 0;

        /**
         * Returns true if the command was accepted when sent to the CommandProcessor.
         * A Command is accepted when the CommandProcessor was running and its queue
         * was not full.
         */
        virtual bool accepted() const = 0;

        /**
         * Returns true if the command was executed by the CommandProcessor.
         * When executed() is true, you can check if it was found valid().
         */
        virtual bool executed() const = 0;

        /**
         * Returns true if the command was valid, i.e. the command itself
         * was executed and returned true. 
         */
        virtual bool valid() const = 0;

        /**
         * Creates a Condition which evaluates (executed() \a and result() ).
         * Thus it will only return true when the CommandProcessor executed
         * the dispatched command \a and it was valid.
         */
        virtual ORO_CoreLib::ConditionInterface* createValidCondition() const = 0;

        virtual DispatchInterface* clone() const = 0;
#if 0
        /*
         * Creates a Command which calls this->execute().
         * Thus it will only return true when the CommandProcessor executed
         * the dispatched command \a and it was valid.
         */
        virtual ORO_CoreLib::CommandInterface* createCommand() const = 0;
#endif
    };
}

#endif
