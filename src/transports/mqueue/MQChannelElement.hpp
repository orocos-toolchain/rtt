#ifndef MQ_CHANNEL_ELEMENT_H
#define MQ_CHANNEL_ELEMENT_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <cassert>
#include <exception>

#include "../../Activity.hpp"
#include "../../base/RunnableInterface.hpp"
#include "../../types/TypeTransporter.hpp"
#include "../../Logger.hpp"
#include "Dispatcher.hpp"
#include "../../base/ChannelElement.hpp"
#include "MQTypeTransporter.hpp"

namespace RTT
{
    namespace mqueue
    {

        /**
         * Processes incomming messages and forwards them
         * to the next element in chain.
         */
        class MQChannelProcessing
        {
        public:
            virtual void processIncomming() = 0;
        };
        /**
         * Implements the a ChannelElement using message queues.
         * It converts the C++ calls into MQ messages and vice versa.
         */
        template<typename T>
        class MQChannelElement: public base::ChannelElement<T>
        {
            MQTypeTransporter const& mtransport;
            typename internal::ValueDataSource<T>::shared_ptr data_source;
            mqd_t mqdes;
            char* buf;
            bool mis_sender;

        public:
            /**
             * Create a channel element for remote data exchange.
             * @param transport The type specific object that will be used to marshal the data.
             */
            MQChannelElement(MQTypeTransporter const& transport, std::string name_id, bool is_sender) :
                mtransport(transport),
                data_source(new internal::ValueDataSource<T>),
                mis_sender(is_sender)
            {
                Logger::In in("MQChannelElement");
                const MQTypeTransporter* mqtransport = dynamic_cast<const MQTypeTransporter*>( &transport );
                assert(mqtransport);
                std::stringstream namestr;
                namestr << '/' <<name_id << '.'<<getpid();

                struct mq_attr mattr;
                mattr.mq_maxmsg = 10;
                mattr.mq_msgsize = mqtransport->blobSize();

                mqdes = mq_open(namestr.str().c_str(), O_CREAT | O_RDWR | O_NONBLOCK, S_IREAD | S_IWRITE, &mattr);
                log(Debug) << "Opening '"<< namestr.str() <<"' with mqdes='"<<mqdes<<"' for " << (is_sender ? "writing." : "reading.") <<endlog();
                if (mqdes < 0)
                    throw std::runtime_error("Could not open message queue");

                buf = new char[mtransport.blobSize()];
                memset(buf,0,mtransport.blobSize()); // necessary to trick valgrind
                if ( !mis_sender) {
                    Dispatcher::Instance()->addQueue( mqdes, this);
                }
            }

            ~MQChannelElement() {
                if ( !mis_sender) {
                    Dispatcher::Instance()->removeQueue( mqdes );
                    mq_close( mqdes );
                }
                delete[] buf;
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
                    assert(this->input);
                    typename base::ChannelElement<T>::value_t sample;
                    // this read should always succeed since signal() means 'data available in a data element'.
                    base::ChannelElement<T>* input = dynamic_cast< base::ChannelElement<T>* >(this->input);
                    if( input->read(sample) )
                        return this->write(sample);
                } else {
                    // should not get signal when beeing a receiver.
                    assert(this->output);
                    typename base::ChannelElement<T>::value_t sample;
                    typename base::ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< base::ChannelElement<T> >(this->output);
                    if( this->read(sample) )
                        return output->write(sample);
                }
                return false;
            }

            /**
             * Read from the message queue.
             * @param sample stores the resulting data sample.
             * @return true if an item could be read.
             */
            bool read(typename base::ChannelElement<T>::reference_t sample)
            {
                int bytes = 0;
                if ( (bytes = mq_receive(mqdes, buf, mtransport.blobSize(), 0)) == -1 ) {
                    return false;
                }
                assert( bytes == mtransport.blobSize() );
                if ( mtransport.updateBlob((void*)buf, data_source) ) {
                    sample = data_source->get();
                    return true;
                }
                return false;
            }

            /**
             * Write to the message queue
             * @param sample the data sample to write
             * @return true if it could be sent.
             */
            bool write(typename base::ChannelElement<T>::param_t sample)
            {
                data_source->set(sample);
                char* lbuf = (char*)mtransport.createBlob(data_source);
                if (mq_send(mqdes, lbuf, mtransport.blobSize(), 0) ) {
                    //log(Error) << "Failed to write into MQChannel !" <<endlog();
                    return false;
                }
                return true;
            }

        };
    }
}

#endif

