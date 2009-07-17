/***************************************************************************
  tag: FMTC  do nov 2 13:06:19 CET 2006  CorbaDataObjectProxy.hpp

                        CorbaDataObjectProxy.hpp -  description
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



#ifndef ORO_CORBA_DATAOBJECTPROXY_HPP
#define ORO_CORBA_DATAOBJECTPROXY_HPP

#if 0
//necessary to keep MSVC happy 
//avoids redefinitions in winsock2.h
#pragma push_macro("_WINSOCKAPI_")
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <windows.h>

#pragma pop_macro("_WINSOCKAPI_")
/////////////////////////////////////////
#endif

#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#else
#include <omniORB4/CORBA.h>
#include <omniORB4/poa.h>
#endif

#include "../DataObjectInterfaces.hpp"
#include "../DataSources.hpp"
#include "OperationsC.h"
#include "CorbaLib.hpp"

namespace RTT
{ namespace Corba {


    /**
     * A class which provides access to remote Corba Data.
     * It is constructed proxy-side and Set/Get operations
     * are sent or received from the Event Service. It
     * hence \b mirrors remote data.
     *
     * @ingroup CoreLibBuffers
     */
    template<class T>
    class CorbaDataObjectProxy
        : public DataObjectInterface<T>
    {
        std::string name;
        AssignableExpression_var mec;
    public:
        /**
         * Construct a CorbaDataObject which uses the Corba Event Service
         * as transport medium.
         *
         * @param _name The name of this CorbaDataObject.
         * @param ec The event service which transfers data of type \a T
         */
        CorbaDataObjectProxy(const std::string& _name, AssignableExpression_ptr ec )
            : name(_name), mec(AssignableExpression::_duplicate(ec) )
        {
        }

        /**
         * Destructor
         */
        ~CorbaDataObjectProxy() {
        }

        /**
         * Return the name of this CorbaDataObject.
         *
         * @return The name
         */
        const std::string& getName() const { return name;}

        void setName( const std::string& _name )
        {
            name = _name;
        }

        /**
         * The type of the data.
         */
        typedef T DataType;

        /**
         * Get a copy of the Data of the module.
         *
         * @param pull A copy of the data.
         */
        void Get( DataType& pull ) const {
            Logger::In in("CorbaDataObjectProxy::Get");
            CORBA::Any_var v;
            v = mec->get();
            ReferenceDataSource<T> rds(pull);
            rds.ref();
            if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &v.in() ) == false) {
                log(Error) << "Could not read remote value: wrong data type."<<endlog();
                return;
            }
        }

        /**
         * Get a copy of the data of the module.
         *
         * @return The result of the module.
         */
        DataType Get() const { DataType p; this->Get(p); return p; }

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        void Set( const DataType& push ) {
            // Trick: we must create a datasource to convert 'push' to
            // an Any. We do not convert it ourselves, since this would
            // invoke Get() first.
            ValueDataSource<T> vds(push);
            vds.ref();
            CORBA::Any_var toset = (CORBA::Any_ptr)vds.createBlob(ORO_CORBA_PROTOCOL_ID);
            mec->set( toset.in() );
        }

        CorbaDataObjectProxy<DataType>* clone() const {
            return new CorbaDataObjectProxy<DataType>(name, mec.in());
        }

        CorbaDataObjectProxy<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) const {
            return const_cast<CorbaDataObjectProxy<DataType>*>(this);
        }

    };
}}

#endif

