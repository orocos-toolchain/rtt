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
            std::string mqname;

        public:
            /**
             * Create a channel element for remote data exchange.
             * @param transport The type specific object that will be used to marshal the data.
             */
            MQChannelElement(base::PortInterface* port, types::TypeMarshaller<T> const& transport,
                             const ConnPolicy& policy, bool is_sender) :
                mtransport(transport),
                data_source(new internal::ValueDataSource<T>),
                mis_sender(is_sender), minit_done(false),
                max_size(policy.data_size ? policy.data_size : transport.getSampleSize( data_source->set() ) )
            {
                Logger::In in("MQChannelElement");
                std::stringstream namestr;
                namestr << '/' << port->getInterface()->getParent()->getName()
                        << '.' << port->getName() << '.'<<this << '@' << getpid();

                if ( policy.name_id.empty() )
                    policy.name_id = namestr.str();

                struct mq_attr mattr;
                mattr.mq_maxmsg = policy.size ? policy.size : 10;
                mattr.mq_msgsize = max_size;
                assert( max_size );
                if ( policy.name_id[0] != '/')
                    throw std::runtime_error("Could not open message queue with wrong name. Names must start with '/' and contain no more '/' after the first one.");
                if ( max_size <= 0)
                    throw std::runtime_error("Could not open message queue with zero message size.");
                int oflag = O_CREAT | O_NONBLOCK;
                if (mis_sender)
                    oflag |= O_WRONLY;
                else
                    oflag |= O_RDONLY;
                mqdes = mq_open(policy.name_id.c_str(), oflag, S_IREAD | S_IWRITE, &mattr);

                if (mqdes < 0) {
                    int the_error = errno;
                    log(Error) << "FAILED opening '"<< policy.name_id <<"' with message size "<< mattr.mq_msgsize<< ", buffer size "<<mattr.mq_maxmsg<<" for " << (is_sender ? "writing :" : "reading :") <<endlog();
                    // these are copied from the man page. They are more informative than the plain perrno() text.
                    switch (the_error) {
                    case EACCES:
                        log(Error) <<"The queue exists, but the caller does not have permission to open it in the specified mode."<< endlog();
                        break;
                    case EINVAL:
                        // or the name is wrong...
                        log(Error) << "Wrong mqueue name given OR, In a process  that  is  unprivileged  (does  not  have  the "
                                   << "CAP_SYS_RESOURCE  capability),  attr->mq_maxmsg  must  be  less than or equal to the msg_max limit, and attr->mq_msgsize must be less than or equal to the msgsize_max limit.  In addition, even in a privileged process, "
                                   << "attr->mq_maxmsg cannot exceed the HARD_MAX limit.  (See mq_overview(7) for details of these limits.)"<< endlog();
                        break;
                    case EMFILE:
                        log(Error) <<"The process already has the maximum number of files and message queues open."<< endlog();
                        break;
                    case ENAMETOOLONG:
                        log(Error) <<"Name was too long."<< endlog();
                        break;
                    case ENFILE:
                        log(Error) <<"The system limit on the total number of open files and message queues has been reached."<< endlog();
                        break;
                    case ENOSPC:
                        log(Error) <<"Insufficient space for the creation of a new message queue.  This probably occurred because the queues_max limit was encountered; see mq_overview(7)."<< endlog();
                        break;
                    case ENOMEM:
                        log(Error) <<"Insufficient memory."<< endlog();
                        break;
                    default:
                        log(Error) <<"Submit a bug report. An unexpected mq error occured with errno="<< errno << ": " << strerror(errno) << endlog();
                    }
                    throw std::runtime_error("Could not open message queue: mq_open returned -1.");
                }

                log(Debug) << "Opened '"<< policy.name_id <<"' with mqdes='"<<mqdes<<"' for " << (is_sender ? "writing." : "reading.") <<endlog();

                buf = new char[ max_size];
                memset(buf,0,max_size); // necessary to trick valgrind
                mqname = policy.name_id;
            }

            ~MQChannelElement() {
                if ( !mis_sender) {
                    if (minit_done)
                        Dispatcher::Instance()->removeQueue( mqdes );
                } else {
                    // sender unlinks to avoid future re-use of new readers.
                    mq_unlink( mqname.c_str() );
                }
                // both sender and receiver close their end.
                mq_close( mqdes );

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
                        if ( mtransport.updateFromBlob((void*)buf,max_size, data_source) ) {
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
                    typename base::ChannelElement<T>::value_t sample; // XXX: real-time !
                    // this read should always succeed since signal() means 'data available in a data element'.
                    base::ChannelElement<T>* input = dynamic_cast< base::ChannelElement<T>* >(this->input);
                    if( input->read(sample) == NewData )
                        return this->write(sample);
                } else {
                    // should not get signal when beeing a receiver.
                    assert(this->output);
                    typename base::ChannelElement<T>::value_t sample;
                    typename base::ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< base::ChannelElement<T> >(this->output);
                    if( this->read(sample) == NewData && output )
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
                    //log(Debug) << "Tried read on empty mq!" <<endlog();
                    return NoData;
                }
                //log(Debug) << "Got read on mq. bytes:"<< bytes <<endlog();
                //assert( bytes == mtransport.blobSize() ); //size may differ in complex types.
                if ( mtransport.updateFromBlob((void*)buf, max_size, data_source) ) {
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
                std::pair<void*,int> blob = mtransport.fillBlob(data_source, (void*)buf, max_size);
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

