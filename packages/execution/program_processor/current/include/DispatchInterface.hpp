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
 
#include "CommandInterface.hpp"
#include "ConditionInterface.hpp"

namespace RTT
{
    /**
     * Dispatch a CommandInterface to a CommandProcessor and track its status.
     */
    class DispatchInterface
        : public CommandInterface
    {
    public:
        /**
         * Returns true if the command is ready for dispatch.
         * If ready() returns true, dispatch() can be safely called,
         * otherwise, dispatch() will fail.
         */
        virtual bool ready() const = 0;

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
         * Returns true if the command is done.
         */
        virtual bool done() const = 0;

        virtual ConditionInterface* createCondition() const = 0;

        virtual DispatchInterface* clone() const = 0;

        virtual DispatchInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return this->clone();
        }
    };
}

#endif
