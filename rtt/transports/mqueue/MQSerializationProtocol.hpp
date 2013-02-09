/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  MQSerializationProtocol.hpp

                        MQSerializationProtocol.hpp -  description
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


/*
 * MqueueSerializationProtocol.hpp
 *
 *  Created on: Sep 21, 2009
 *      Author: kaltan
 */

#ifndef MQSERIALIZATIONPROTOCOL_HPP_
#define MQSERIALIZATIONPROTOCOL_HPP_

#include "MQTemplateProtocolBase.hpp"
#include "binary_data_archive.hpp"
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>
namespace RTT
{

    namespace mqueue
    {

        template<class T>
        class MQSerializationProtocol
        : public RTT::mqueue::MQTemplateProtocolBase<T>
        {
        public:
            MQSerializationProtocol() {
            }

            virtual std::pair<void const*,int> fillBlob( base::DataSourceBase::shared_ptr source, void* blob, int size, void* cookie) const
            {
                namespace io = boost::iostreams;
                typename internal::DataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<T> >( source );
                if ( d ) {
                    // we use the boost iostreams library for re-using the blob buffer in the stream object.
                    // and the serialization library to write the data into stream.
                    io::stream<io::array_sink>  outbuf( (char*)blob, size);
                    binary_data_oarchive out( outbuf );
                    out << d->rvalue();
                    return std::make_pair( blob, out.getArchiveSize() );
                }
                return std::make_pair((void*)0,int(0));
            }

            /**
            * Update \a target with the contents of \a blob which is an object of a \a protocol.
            */
            virtual bool updateFromBlob(const void* blob, int size, base::DataSourceBase::shared_ptr target, void* cookie) const {
                namespace io = boost::iostreams;
                typename internal::AssignableDataSource<T>::shared_ptr ad = internal::AssignableDataSource<T>::narrow( target.get() );
                if ( ad ) {
                    io::stream<io::array_source>  inbuf((const char*)blob, size);
                    binary_data_iarchive in( inbuf );
                    in >> ad->set();
                    return true;
                }
                return false;
            }

            virtual unsigned int getSampleSize(base::DataSourceBase::shared_ptr sample, void* cookie) const {
                typename internal::DataSource<T>::shared_ptr tsample = boost::dynamic_pointer_cast< internal::DataSource<T> >( sample );
                if ( ! tsample ) {
                    log(Error) << "getSampleSize: sample has wrong type."<<endlog();
                    return 0;
                }
                namespace io = boost::iostreams;
                char sink[1];
                io::stream<io::array_sink>  outbuf(sink,1);
                binary_data_oarchive out( outbuf, false );
                out << tsample->get();
                //std::cout << "sample size is "<< tsample->rvalue().size() <<" archive is " << out.getArchiveSize() <<std::endl; //disable all types but std::vector<double> for this to compile
                return out.getArchiveSize();
            }
        };

    }

}

#endif /* MQSERIALIZATIONPROTOCOL_HPP_ */
