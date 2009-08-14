/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ActionProxy.hpp

                        ActionProxy.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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


#ifndef ORO_CORBA_ACTIONPROXY_HPP
#define ORO_CORBA_ACTIONPROXY_HPP

#include "../../base/ActionInterface.hpp"
#include "OperationsC.h"

namespace RTT
{namespace Corba
{

    /**
     * This class manages the access of remote Action Corba Servers.
     */
    class RTT_CORBA_API ActionProxy
        : public ActionInterface
    {
    protected:
        static std::map<Corba::Action_ptr, ActionProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ActionProxy( ::RTT::Corba::Action_ptr t );

        Corba::Action_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Action Object.
         * @param act The Object to connect to.
         * @return A new or previously created CORBA proxy for \a act.
         */
        static ActionProxy* Create(::RTT::Corba::Action_ptr act);

        /**
         * Get the Corba Object reference of the Action.
         * This object universally identifies the remote Action Object
         * and can be used to tell other (remote) objects where to find it.
         */
        Corba::Action_ptr server() const;

        virtual void readArguments() {}

        virtual bool execute() {
            return mdata->execute();
        }

        virtual void reset() {
            mdata->reset();
        }

        virtual ActionInterface* clone() const {
            return new ActionProxy( mdata.in() );
        }

        virtual ActionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return this->clone();
        }

    };

}}

#endif
