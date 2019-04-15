/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  CorbaTypeTransporter.hpp

                        CorbaTypeTransporter.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef CORBA_TYPE_TRANSPORTER_H
#define CORBA_TYPE_TRANSPORTER_H

#include "corba.h"
#include "../../types/TypeTransporter.hpp"
#include "ServiceC.h"

namespace RTT {

    namespace corba {

        class CRemoteChannelElement_i;

    /**
     * Extends the TypeTransporter in order to allow the creation of
     * channel elements or output halves for a given type.
     */
	class RTT_CORBA_API CorbaTypeTransporter : public RTT::types::TypeTransporter
	{
	public:
        /**
         * Evaluate \a source and create an any which contains the value of \a source.
         */
        virtual CORBA::Any_ptr createAny(base::DataSourceBase::shared_ptr source) const = 0;

        /**
         * Evaluate \a source and update an any which contains the value of \a source.
         */
        virtual bool updateAny( base::DataSourceBase::shared_ptr source, CORBA::Any& any) const = 0;

        /**
         * Create a Data Source from an any.
         * @return null if no conversion could be made.
         */
        virtual base::DataSourceBase::shared_ptr createDataSource(const CORBA::Any* any) const = 0;

        /**
         * Update an assignable datasource \a target with the contents of \a blob.
         */
        virtual bool updateFromAny(const CORBA::Any* blob, base::DataSourceBase::shared_ptr target) const = 0;

	    /**
	     * Builds a channel element for remote transport in both directions.
	     * @param sender The data flow interface which will be sending or receiving this channel.
	     * @param poa The POA to manage the server code.
	     * @return the created CChannelElement_i.
	     */
	    virtual CRemoteChannelElement_i* createOutputChannelElement_i(std::string const& dispatcherName, ::PortableServer::POA* poa, bool is_pull) const = 0;

	    /**
	     * Builds a channel element for remote transport in both directions.
	     * @param sender The data flow interface which will be sending or receiving this channel.
	     * @param poa The POA to manage the server code.
	     * @return the created CChannelElement_i.
	     */
	    virtual CRemoteChannelElement_i* createInputChannelElement_i(::PortableServer::POA* poa, bool is_pull) const = 0;

	    /**
	     * The CORBA transport does not support creating 'CORBA' streams.
	     * @return null
	     */
	    virtual base::ChannelElementBase::shared_ptr createStream( base::PortInterface* /*port*/, const ConnPolicy& /*policy*/, bool /*is_sender*/) const;

        /**
         * Create a data source for an attribute or property.
         * Will create an assignable data source when the property or attribute
         * can be changed, a normal data source otherwise.
         */
        virtual base::DataSourceBase::shared_ptr createPropertyDataSource(CService_ptr serv, const std::string& vname) = 0;
        virtual base::DataSourceBase::shared_ptr createAttributeDataSource(CService_ptr serv, const std::string& vname) = 0;
	};
    }
}

#endif

