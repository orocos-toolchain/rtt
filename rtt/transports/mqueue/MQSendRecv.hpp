/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  MQSendRecv.hpp

                        MQSendRecv.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_MQSENDER_HPP_
#define ORO_MQSENDER_HPP_

#include <mqueue.h>
#include "../../rtt-fwd.hpp"
#include "../../base/DataSourceBase.hpp"

namespace RTT
{
    namespace mqueue
    {
        /**
         * Implements the sending/receiving of mqueue messages.
         * It can only be OR sender OR receiver (logical XOR).
         */
        class MQSendRecv
        {
        protected:
            /**
             * Transport marshaller used for size calculations
             * and data updates.
             */
            types::TypeMarshaller const& mtransport;
            /**
             * A private blob that is returned by mtransport.getCookie(). It is
             * used by the marshallers if they need private internal data to do
             * the marshalling
             */
            void* marshaller_cookie;
            /**
             * MQueue file descriptor.
             */
            mqd_t mqdes;
            /**
             * Send/Receive buffer. It is initialized to the size of the value
             * provided by the ConnPolicy or, if the policy has a zero data
             * size, the sample given to setupStream
             *
             * Its size is saved in max_size
             */
            char* buf;
            /**
             * True if this object is a sender.
             */
            bool mis_sender;
            /**
             * True if setupStream() was called, false after cleanupStream().
             */
            bool minit_done;
            /**
             * The size of buf.
             */
            int max_size;
            /**
             * The name of the queue, as specified in the ConnPolicy when
             * creating the stream, or self-calculated when that name was empty.
             */
            std::string mqname;
            /**
             * The size of the data, as specified in the ConnPolicy when
             * creating the stream, or calculated using the transport when
             * that size was zero.
             */
            int mdata_size;

        public:
            /**
             * Create a channel element for remote data exchange.
             * @param transport The type specific object that will be used to marshal the data.
             */
            MQSendRecv(types::TypeMarshaller const& transport);

            void setupStream(base::DataSourceBase::shared_ptr ds, base::PortInterface* port, ConnPolicy const& policy, bool is_sender);

            ~MQSendRecv();

            void cleanupStream();

            /**
             * Adapts the mq send/receive buffer size according to the
             * data in mqdata_source, or the value set in mdata_size;
             * @param sample
             */
            virtual void mqNewSample(base::DataSourceBase::shared_ptr ds);

            /**
             * Works only in receive mode, waits for a new sample and
             * adapts the receive buffer to match it's size.
             * @return
             */
            virtual bool mqReady(base::DataSourceBase::shared_ptr ds, base::ChannelElementBase* chan);

            /**
             * Read from the message queue.
             * @param sample stores the resulting data sample.
             * @return true if an item could be read.
             */
            bool mqRead(base::DataSourceBase::shared_ptr ds);

            /**
             * Write to the message queue
             * @param ds the data sample to write
             * @param is_data_sample true if the sample is used for initialization, false if it is a proper write
             * @return true if it could be sent.
             */
            bool mqWrite(base::DataSourceBase::shared_ptr ds);
        };
    }
}

#endif /* ORO_MQSENDER_HPP_ */
