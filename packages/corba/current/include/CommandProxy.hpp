/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CommandProxy.hpp 

                        CommandProxy.hpp -  description
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
 
 
#ifndef ORO_CORBA_COMMANDPROXY_HPP
#define ORO_CORBA_COMMANDPROXY_HPP

#include "../DispatchInterface.hpp"
#include "ExecutionC.h"

namespace RTT
{namespace Corba
{

    /**
     * This class manages the access of remote Command Corba Servers.
     */
    class CommandProxy
        : public DispatchInterface
    {
    protected:
        static std::map<Orocos::Command_ptr, CommandProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        CommandProxy( ::Orocos::Command_ptr t );

        Orocos::Command_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Command Object.
         * @param act The Object to connect to.
         * @return A new or previously created CORBA proxy for \a act.
         */
        static CommandProxy* Create(::Orocos::Command_ptr act);

        /**
         * Get the Corba Object reference of the Command.
         * This object universally identifies the remote Command Object
         * and can be used to tell other (remote) objects where to find it.
         */
        Orocos::Command_ptr server() const;

        virtual void readArguments() {}

        virtual bool dispatch() {
            return mdata->execute();
        }

        virtual bool execute() {
            return mdata->execute();
        }

        virtual void reset() {
            mdata->reset();
        }

        virtual bool sent() const {
            return mdata->sent();
        }

        virtual bool accepted() const {
            return mdata->accepted();
        }

        virtual bool executed() const {
            return mdata->executed();
        }

        virtual bool valid() const {
            return mdata->valid();
        }

        virtual bool evaluate() const {
            return mdata->evaluate();
        }

        virtual ConditionInterface* createValidCondition() const;

        virtual DispatchInterface* clone() const {
            return new CommandProxy( mdata.in() );
        }

        virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return this->clone();
        }

    };

}}

#endif
