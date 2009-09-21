#ifndef MQ_CHANNEL_ELEMENT_H
#define MQ_CHANNEL_ELEMENT_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "../../Activity.hpp"
#include "../../base/RunnableInterface.hpp"
#include "../../types/TypeTransporter.hpp"
#include "../../Logger.hpp"
#include "Dispatcher.hpp"
#include "../../base/ChannelElement.hpp"
#include "../../base/PortInterface.hpp"
#include "../../interface/DataFlowInterface.hpp"

namespace RTT
{
    namespace mqueue
    {
        /**
         * Implements the a ChannelElement using message queues.
         * It converts the C++ calls into MQ messages and vice versa.
         */
        template<typename T>
        class MQChannelElement: public base::ChannelElement<T>
        {
            types::TypeMarshaller<T> const& mtransport;
            typename internal::ValueDataSource<T>::shared_ptr data_source;
            mqd_t mqdes;
            char* buf;
            bool mis_sender;
            bool minit_done;
            const int max_size;

        public:
            /**
             * Create a channel element for remote data exchange.
             * @param transport The type specific object that will be used to marshal the data.
             */
            MQChannelElement(base::PortInterface* port, types::TypeMarshaller<T> const& transport,
                             std::string& name_id,
                             int size_hint, bool is_sender) :
                mtransport(transport),
                data_source(new internal::ValueDataSource<T>),
                mis_sender(is_sender), minit_done(false),
                max_size(size_hint ? size_hint : sizeof(T) )
            {
                Logger::In in("MQChannelElement");
                std::stringstream namestr;
                namestr << '/' << port->getInterface()->getParent()->getName()
                        << '.' << port->getName() << '.'<<this << '@' << getpid();

                if ( name_id.empty() )
                    name_id = namestr.str();

                struct mq_attr mattr;
                mattr.mq_maxmsg = 10; // XXX arbitrary hard-coded. This is/should be a policy.
                mattr.mq_msgsize = max_size;

                mqdes = mq_open(name_id.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, S_IREAD | S_IWRITE, &mattr);

                if (mqdes < 0)
                    throw std::runtime_error("Could not open message queue");

                log(Debug) << "Opened '"<< name_id <<"' with mqdes='"<<mqdes<<"' for " << (is_sender ? "writing." : "reading.") <<endlog();

                buf = new char[ max_size];
                memset(buf,0,max_size); // necessary to trick valgrind
            }

            ~MQChannelElement() {
                if ( !mis_sender) {
                    if (minit_done)
                        Dispatcher::Instance()->removeQueue( mqdes );
                    mq_close( mqdes );
                }
                delete[] buf;
            }

            virtual bool inputReady() {
                if (minit_done)
                    return true;

                if ( ! mis_sender ) {
                    // Try to get the initial sample
                    struct timespec abs_timeout;
                    abs_timeout.tv_sec = 0;
                    abs_timeout.tv_nsec = Seconds_to_nsecs(0.5);
                    ssize_t ret = mq_timedreceive(mqdes, buf,
                            max_size, 0,
                            &abs_timeout);
                    if ( ret != -1 ) {
                        if ( mtransport.updateBlob((void*)buf, data_source) ) {
                            T sample = data_source->get();
                            typename base::ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< base::ChannelElement<T> >(this->output);
                            assert(output);
                            output->data_sample(sample);
                            minit_done = true;
                        } else {
                            log(Error) << "Failed to initialize MQ Channel Element with initial data sample."<<endlog();
                            return false;
                        }
                    } else {
                        log(Error) << "Failed to receive initial data sample for MQ Channel Element."<<endlog();
                        return false;
                    }
                    // ok, now we can add the dispatcher.
                    Dispatcher::Instance()->addQueue( mqdes, this);
                } else {
                    assert( !mis_sender ); // we must be receiver. we can only receive inputReady when we're on the input port side of the MQ.
                    return false;
                }
                return true;
            }

            virtual bool data_sample(typename base::ChannelElement<T>::param_t sample)
            {
                // send initial data sample to the other side using a plain write.
                typename base::ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< base::ChannelElement<T> >(this->output);
                if (mis_sender) {
                    return write(sample);
                }
                return false;
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
            FlowStatus read(typename base::ChannelElement<T>::reference_t sample)
            {
                int bytes = 0;
                if ( (bytes = mq_receive(mqdes, buf, max_size, 0)) == -1 ) {
                    return NoData;
                }
                //assert( bytes == mtransport.blobSize() ); //size may differ in complex types.
                if ( mtransport.updateBlob((void*)buf, data_source) ) {
                    sample = data_source->get();
                    return NewData;
                }
                return NoData;
            }

            /**
             * Write to the message queue
             * @param sample the data sample to write
             * @return true if it could be sent.
             */
            bool write(typename base::ChannelElement<T>::param_t sample)
            {
                data_source->set(sample);
                std::pair<void*,int> blob = mtransport.createBlob(data_source);
                char* lbuf = (char*)blob.first;
                if (mq_send(mqdes, lbuf, blob.second, 0) ) {
                    //log(Error) << "Failed to write into MQChannel !" <<endlog();
                    return false;
                }
                return true;
            }

        };
    }
}

#endif

