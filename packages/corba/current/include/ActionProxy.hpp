/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ActionProxy.hpp 

                        ActionProxy.hpp -  description
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
 
 
#ifndef ORO_CORBA_ACTIONPROXY_HPP
#define ORO_CORBA_ACTIONPROXY_HPP

#include "../CommandInterface.hpp"
#include "ExecutionC.h"

namespace RTT
{namespace Corba
{

    /**
     * This class manages the access of remote Action Corba Servers.
     */
    class ActionProxy
        : public CommandInterface
    {
    protected:
        static std::map<Orocos::Action_ptr, ActionProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ActionProxy( ::Orocos::Action_ptr t );

        Orocos::Action_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Action Object.
         * @param act The Object to connect to.
         * @return A new or previously created CORBA proxy for \a act.
         */
        static ActionProxy* Create(::Orocos::Action_ptr act);

        /**
         * Get the Corba Object reference of the Action.
         * This object universally identifies the remote Action Object
         * and can be used to tell other (remote) objects where to find it.
         */
        Orocos::Action_ptr server() const;

        virtual void readArguments() {}

        virtual bool execute() {
            return mdata->execute();
        }

        virtual void reset() {
            mdata->reset();
        }

        virtual CommandInterface* clone() const {
            return new ActionProxy( mdata.in() );
        }

        virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return this->clone();
        }

    };

}}

#endif
