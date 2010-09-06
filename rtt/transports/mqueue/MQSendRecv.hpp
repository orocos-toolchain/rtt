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
             * Data source used to store the result of a receive
             * or to read the value to send.
             */
            base::DataSourceBase::shared_ptr mqdata_source;
            /**
             * Transport marshaller used for size calculations
             * and data updates.
             */
            types::TypeMarshaller const& mtransport;
            /**
             * MQueue file descriptor.
             */
            mqd_t mqdes;
            /**
             * Send/Receive buffer, with size equal to the size
             * of mqdata_source, or the value provided by the ConnPolicy
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
            virtual void mqNewSample();

            /**
             * Works only in receive mode, waits for a new sample and
             * adapts the receive buffer to match it's size.
             * @return
             */
            virtual bool mqReady(base::ChannelElementBase* chan);

            /**
             * Read from the message queue.
             * @param sample stores the resulting data sample.
             * @return true if an item could be read.
             */
            bool mqRead();

            /**
             * Write to the message queue
             * @param sample the data sample to write
             * @return true if it could be sent.
             */
            bool mqWrite();
        };
    }
}

#endif /* ORO_MQSENDER_HPP_ */
