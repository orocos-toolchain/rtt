/*
 * MqueueSerializationProtocol.hpp
 *
 *  Created on: Sep 21, 2009
 *      Author: kaltan
 */

#ifndef MQSERIALIZATIONPROTOCOL_HPP_
#define MQSERIALIZATIONPROTOCOL_HPP_

#include "MQTemplateProtocol.hpp"
#include "../../marsh/binary_data_archive.hpp"
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>
namespace RTT
{

    namespace mqueue
    {

        template<class T>
        class MQSerializationProtocol
        : public RTT::mqueue::MQTemplateProtocol<T>
        {
        public:
            MQSerializationProtocol() {
            }

            virtual std::pair<void*,int> fillBlob( base::DataSourceBase::shared_ptr source, void* blob, int size) const
            {
                namespace io = boost::iostreams;
                internal::AssignableDataSource<T>* d = internal::AdaptAssignableDataSource<T>()( source );
                if ( d ) {
                    // we use the boost iostreams library for re-using the blob buffer in the stream object.
                    // and the serialization library to write the data into stream.
                    io::stream<io::array_sink>  outbuf( (char*)blob, size);
                    marsh::binary_data_oarchive out( outbuf );
                    out << d->set();
                    return std::make_pair( blob, out.getArchiveSize() );
                }
                return std::make_pair((void*)0,int(0));
            }

            /**
            * Update \a target with the contents of \a blob which is an object of a \a protocol.
            */
            virtual bool updateFromBlob(const void* blob, int size, base::DataSourceBase::shared_ptr target) const {
                namespace io = boost::iostreams;
                typename internal::AssignableDataSource<T>::shared_ptr ad = internal::AssignableDataSource<T>::narrow( target.get() );
                if ( ad ) {
                    io::stream<io::array_source>  inbuf((const char*)blob, size);
                    marsh::binary_data_iarchive in( inbuf );
                    in >> ad->set();
                    return true;
                }
                return false;
            }

            virtual unsigned int getSampleSize(const T& sample) const {
                namespace io = boost::iostreams;
                char sink[1];
                io::stream<io::array_sink>  outbuf(sink,1);
                marsh::binary_data_oarchive out( outbuf, false );
                out << sample;
                //std::cout << "sample is "<< sample.size() <<" arch is " << out.getArchiveSize() <<std::endl;
                return out.getArchiveSize();
            }
        };

    }

}

#endif /* MQSERIALIZATIONPROTOCOL_HPP_ */
