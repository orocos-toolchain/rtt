/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  MQChannelElement.hpp

                        MQChannelElement.hpp -  description
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


#ifndef MQ_CHANNEL_ELEMENT_H
#define MQ_CHANNEL_ELEMENT_H

#include "MQSendRecv.hpp"
#include "../../Logger.hpp"
#include "../../base/ChannelElement.hpp"
#include "../../internal/DataSource.hpp"
#include "../../internal/DataSources.hpp"
#include <stdexcept>

namespace RTT
{
    namespace mqueue
    {
        /**
         * Implements the a ChannelElement using message queues.
         * It converts the C++ calls into MQ messages and vice versa.
         * @todo This class can be refactored into a base class with
         * generic mqueue code and a subclass with type specific info.
         * @todo This is an inspiration for a generic, transport independent
         * channel element.
         */
        template<typename T>
        class MQChannelElement: public base::ChannelElement<T>, public MQSendRecv
        {
            /** Used as a temporary on the reading side */
            typename internal::ValueDataSource<T>::shared_ptr read_sample;
            /** Used in write() to refer to the sample that needs to be written */
            typename internal::LateConstReferenceDataSource<T>::shared_ptr write_sample;

        public:
            /**
             * Create a channel element for remote data exchange.
             * @param transport The type specific object that will be used to marshal the data.
             */
            MQChannelElement(base::PortInterface* port, types::TypeMarshaller const& transport,
                             const ConnPolicy& policy, bool is_sender)
                : MQSendRecv(transport)
                , read_sample(new internal::ValueDataSource<T>)
                , write_sample(new internal::LateConstReferenceDataSource<T>)

            {
                Logger::In in("MQChannelElement");
                setupStream(read_sample, port, policy, is_sender);
            }

            ~MQChannelElement() {
                cleanupStream();
            }

            virtual bool inputReady(base::ChannelElementBase::shared_ptr const& caller) {
                if ( mqReady(read_sample, this) ) {
                    typename base::ChannelElement<T>::shared_ptr output = caller->narrow<T>();
                    assert(output);
                    output->data_sample(read_sample->rvalue());
                    return true;
                }
                return false;
            }

            virtual WriteStatus data_sample(typename base::ChannelElement<T>::param_t sample, bool reset = true)
            {
                // send initial data sample to the other side using a plain write.
                if (mis_sender && (!write_sample->getRawDataConst() || reset)) {
                    write_sample->setPointer(&sample);
                    // update MQSendRecv buffer:
                    mqNewSample(write_sample);
                    return mqWrite(write_sample) ? WriteSuccess : WriteFailure;
                }
                return NotConnected;
            }

            /**
             * Signal will cause a read-write cycle to transfer the
             * data from the data/buffer element to the message queue
             * and vice versa.
             *
             * Note: this virtual function is a bit abused. For a sending
             * MQ, signal triggers a direct read on the data element.
             * For a receiving MQ, signal is used by the dispatcher thread
             * to provoque a read from the MQ and forward it to the next
             * channel element.
             *
             * In the sending case, signal could trigger a dispatcher thread
             * that does the read/write cycle, but that seems only causing overhead.
             * The receiving case must use a thread which blocks on all mq
             * file descriptors.
             * @return true in case the forwarding could be done, false otherwise.
             */
            bool signal()
            {
                // copy messages into channel
                if (mis_sender) {
                    // this read should always succeed since signal() means
                    // 'data available in a data element'.
                    typename base::ChannelElement<T>::shared_ptr input =
                        this->getInput();
                    if( input && input->read(read_sample->set(), false) == NewData )
                        return ( this->write(read_sample->rvalue()) == WriteSuccess );
                } else {
                    typename base::ChannelElement<T>::shared_ptr output =
                        this->getOutput();
                    if (output && mqRead(read_sample))
                        return ( output->write(read_sample->rvalue()) == WriteSuccess );
                }
                return false;
            }

            /**
             * Read from the message queue.
             * @param sample stores the resulting data sample.
             * @return true if an item could be read.
             */
            FlowStatus read(typename base::ChannelElement<T>::reference_t sample, bool copy_old_data)
            {
                throw std::runtime_error("not implemented");
            }

            /**
             * Write to the message queue
             * @param sample the data sample to write
             * @return true if it could be sent.
             */
            WriteStatus write(typename base::ChannelElement<T>::param_t sample)
            {
                write_sample->setPointer(&sample);
                if (!mqWrite(write_sample)) {
                    return WriteFailure;
                }
                return WriteSuccess;
            }

            virtual bool isRemoteElement() const
            {
                return true;
            }
            
            virtual std::string getRemoteURI() const
            {
                //check for output element case
                RTT::base::ChannelElementBase *base = const_cast<MQChannelElement<T> *>(this);
                if(base->getOutput())
                    return RTT::base::ChannelElementBase::getRemoteURI();
                
                return mqname;
            }

            virtual std::string getLocalURI() const
            {
                //check for input element case
                RTT::base::ChannelElementBase *base = const_cast<MQChannelElement<T> *>(this);
                if(base->getInput())
                    return RTT::base::ChannelElementBase::getLocalURI();

                return mqname;
            }

            virtual std::string getElementName() const
            {
                return "MQChannelElement";
            }
        };
    }
}

#endif

