/***************************************************************************
  tag: FMTC  do nov 2 13:06:20 CET 2006  CorbaBufferProxy.hpp 

                        CorbaBufferProxy.hpp -  description
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
 
 

#ifndef ORO_CORBA_BUFFER_PROXY_HPP
#define ORO_CORBA_BUFFER_PROXY_HPP

#include "../BufferInterface.hpp"
#include "DataFlowC.h"
#include "DataFlowS.h"
#include "orbsvcs/CosEventChannelAdminC.h"
#include "orbsvcs/CosEventCommC.h"
#include "../DataSources.hpp"
#include "CorbaLib.hpp"


namespace RTT
{ namespace Corba {

    /**
     * A Buffer Proxy to a remote Orocos Buffer connection.
     */
    template<class T>
    class CorbaBufferProxy
        :public BufferInterface<T>
    {
        //! the Buffer
        BufferChannel_var buf;
    public:

        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

        /**
         * Create a buffer which uses the Corba Event Service
         * as transport medium.
         * @param ec The buffer channel which transfers data of type \a T
         */
        CorbaBufferProxy( BufferChannel_ptr ec )
            : buf( BufferChannel::_duplicate(ec) )
        {
        }

        /**
         * Destructor
         */
        ~CorbaBufferProxy() {
        }
        
        bool Push( param_t item )
        {
            ValueDataSource<T> vds(item);
            vds.ref();
            CORBA::Any_var toset = (CORBA::Any_ptr)vds.createBlob(ORO_CORBA_PROTOCOL_ID);
            try {
                buf->push( toset.in() );
            } catch (...) {
                return false;
            }

            return true;
        }

        size_type Push(const std::vector<T>& items)
        {
            typename std::vector<T>::const_iterator itl( items.begin() );
            while ( itl != items.end() ) {
                if ( this->Push( *itl ) == false )
                    break;
                ++itl;
            }
            return (itl - items.begin());
                
        }
        bool Pop( reference_t item )
        {
            CORBA::Any_var res;
            try {
                if ( buf->pull( res.out() ) ) {
                    ReferenceDataSource<T> rds(item);
                    rds.ref();
                    if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &res.in() ) == false) {
                        Logger::log() <<Logger::Error << "Could not Convert remote value: wrong data type."<<Logger::endl;
                        return false;
                    }
                    return true;
                }
            } catch(...) {
                return false;
            }
            return false;
        }

        size_type Pop(std::vector<T>& items )
        {
            value_t item;
            if ( Pop(item) ) {
                items.push_back(item);
                return 1;
            }
            return 0;
        }

        value_t front() const
        {
            value_t item = value_t();
            // Corba's pull() is equal to Orocos' front().
            CORBA::Any_var res;
            try {
                res = buf->front();
            } catch (...) {
                return item;
            }
            ReferenceDataSource<T> rds( item );
            rds.ref();
            if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &res.in() ) == false) {
                Logger::log() <<Logger::Error << "Could not inspect remote value: wrong data type."<<Logger::endl;
            }
            return item;
        }

        size_type capacity() const {
            try {
                return buf->capacity();
            } catch (...) {
                return 0;
            }
        }

        size_type size() const {
            try {
                return buf->size();
            } catch (...) {
                return 0;
            }
        }

        void clear() {
            try {
                buf->clear();
            } catch (...) {
            }
        }

        bool empty() const {
            try {
                return buf->empty();
            } catch (...) {
                return true;
            }
        }

        bool full() const {
            try {
                return buf->full();
            } catch (...) {
                return false;
            }
        }
    };
}}

#endif // BUFFERSIMPLE_HPP
