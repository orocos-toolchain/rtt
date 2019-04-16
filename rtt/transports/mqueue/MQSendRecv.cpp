/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  MQSendRecv.cpp

                        MQSendRecv.cpp -  description
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


#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <errno.h>
#include <boost/algorithm/string.hpp>

#include "MQSendRecv.hpp"
#include "../../types/TypeTransporter.hpp"
#include "../../types/TypeMarshaller.hpp"
#include "../../Logger.hpp"
#include "Dispatcher.hpp"
#include "../../base/PortInterface.hpp"
#include "../../DataFlowInterface.hpp"
#include "../../TaskContext.hpp"

using namespace RTT;
using namespace RTT::detail;
using namespace RTT::mqueue;


MQSendRecv::MQSendRecv(types::TypeMarshaller const& transport) :
    mtransport(transport), marshaller_cookie(0), buf(0), mis_sender(false), minit_done(false), max_size(0), mdata_size(0)
{
}

void MQSendRecv::setupStream(base::DataSourceBase::shared_ptr ds, base::PortInterface* port, ConnPolicy const& policy,
                             bool is_sender)
{
    Logger::In in("MQSendRecv");

    mdata_size = policy.data_size;
    max_size = policy.data_size ? policy.data_size : mtransport.getSampleSize(ds);
    marshaller_cookie = mtransport.createCookie();
    mis_sender = is_sender;

    if (policy.name_id.empty())
    {
        if (!port->getInterface() || !port->getInterface()->getOwner() || port->getInterface()->getOwner()->getName().empty())
            throw std::runtime_error("MQ name_id not set, and the port is either not attached to a task, or said task has no name. Cannot create a reasonably unique MQ name automatically");

        std::stringstream name_stream;
        name_stream << port->getInterface()->getOwner()->getName() << '.' << port->getName() << '.' << this << '@' << getpid();
        std::string name = name_stream.str();
        boost::algorithm::replace_all(name, "/", "_");
        policy.name_id = "/" + name;
    }

    struct mq_attr mattr;
    mattr.mq_maxmsg = policy.size ? policy.size : 10;
    mattr.mq_msgsize = max_size;
    assert( max_size );
    if (policy.name_id[0] != '/')
        throw std::runtime_error("Could not open message queue with wrong name. Names must start with '/' and contain no more '/' after the first one.");
    if (max_size <= 0)
        throw std::runtime_error("Could not open message queue with zero message size.");
    int oflag = O_CREAT;
    if (mis_sender)
        oflag |= O_WRONLY | O_NONBLOCK;
    else
        oflag |= O_RDONLY; //reading is always blocking (see mqReady() )
    mqdes = mq_open(policy.name_id.c_str(), oflag, S_IREAD | S_IWRITE, &mattr);

    if (mqdes < 0)
    {
        int the_error = errno;
        log(Error) << "FAILED opening '" << policy.name_id << "' with message size " << mattr.mq_msgsize << ", buffer size " << mattr.mq_maxmsg << " for "
                << (is_sender ? "writing :" : "reading :") << endlog();
        // these are copied from the man page. They are more informative than the plain perrno() text.
        switch (the_error)
        {
        case EACCES:
            log(Error) << "The queue exists, but the caller does not have permission to open it in the specified mode." << endlog();
            break;
        case EINVAL:
            // or the name is wrong...
            log(Error) << "Wrong mqueue name given OR, In a process  that  is  unprivileged  (does  not  have  the "
                    << "CAP_SYS_RESOURCE  capability),  attr->mq_maxmsg  must  be  less than or equal to the msg_max limit, and attr->mq_msgsize must be less than or equal to the msgsize_max limit.  In addition, even in a privileged process, "
                    << "attr->mq_maxmsg cannot exceed the HARD_MAX limit.  (See mq_overview(7) for details of these limits.)" << endlog();
            break;
        case EMFILE:
            log(Error) << "The process already has the maximum number of files and message queues open." << endlog();
            break;
        case ENAMETOOLONG:
            log(Error) << "Name was too long." << endlog();
            break;
        case ENFILE:
            log(Error) << "The system limit on the total number of open files and message queues has been reached." << endlog();
            break;
        case ENOSPC:
            log(Error)
                    << "Insufficient space for the creation of a new message queue.  This probably occurred because the queues_max limit was encountered; see mq_overview(7)."
                    << endlog();
            break;
        case ENOMEM:
            log(Error) << "Insufficient memory." << endlog();
            break;
        default:
            log(Error) << "Submit a bug report. An unexpected mq error occured with errno=" << errno << ": " << strerror(errno) << endlog();
        }
        throw std::runtime_error("Could not open message queue: mq_open returned -1.");
    }

    log(Debug) << "Opened '" << policy.name_id << "' with mqdes='" << mqdes << "', msg size='"<<mattr.mq_msgsize<<"' an queue length='"<<mattr.mq_maxmsg<<"' for " << (is_sender ? "writing." : "reading.") << endlog();

    buf = new char[max_size];
    memset(buf, 0, max_size); // necessary to trick valgrind
    mqname = policy.name_id;
}

MQSendRecv::~MQSendRecv()
{
    if ( mqdes > 0)
        mq_close(mqdes);
}

void MQSendRecv::cleanupStream()
{
    if (!mis_sender)
    {
        if (minit_done)
        {
            Dispatcher::Instance()->removeQueue(mqdes);
            minit_done = false;
        }
    }
    else
    {
        // sender unlinks to avoid future re-use of new readers.
        mq_unlink(mqname.c_str());
    }
    // both sender and receiver close their end.
    mq_close( mqdes);

    if (marshaller_cookie)
        mtransport.deleteCookie(marshaller_cookie);

    if (buf)
    {
        delete[] buf;
        buf = 0;
    }
}


void MQSendRecv::mqNewSample(RTT::base::DataSourceBase::shared_ptr ds)
{
    // only deduce if user did not specify it explicitly:
    if (mdata_size == 0)
        max_size = mtransport.getSampleSize(ds);
    delete[] buf;
    buf = new char[max_size];
    memset(buf, 0, max_size); // necessary to trick valgrind
}

bool MQSendRecv::mqReady(base::DataSourceBase::shared_ptr ds, base::ChannelElementBase* chan)
{
    if (minit_done)
        return true;

    if (!mis_sender)
    {
        // Try to get the initial sample
        //
        // The output port implementation guarantees that there will be one
        // after the connection is ready
        struct timespec abs_timeout;
        clock_gettime(CLOCK_REALTIME, &abs_timeout);
        abs_timeout.tv_nsec += Seconds_to_nsecs(0.5);
        abs_timeout.tv_sec += abs_timeout.tv_nsec / (1000*1000*1000);
        abs_timeout.tv_nsec = abs_timeout.tv_nsec % (1000*1000*1000);
        //abs_timeout.tv_sec +=1;
        ssize_t ret = mq_timedreceive(mqdes, buf, max_size, 0, &abs_timeout);
        if (ret != -1)
        {
            if (mtransport.updateFromBlob((void*) buf, ret, ds, marshaller_cookie))
            {
                minit_done = true;
                // ok, now we can add the dispatcher.
                Dispatcher::Instance()->addQueue(mqdes, chan);
                return true;
            }
            else
            {
                log(Error) << "Failed to initialize MQ Channel Element with initial data sample." << endlog();
                return false;
            }
        }
        else
        {
            log(Error) << "Failed to receive initial data sample for MQ Channel Element: " << strerror(errno) << endlog();
            return false;
        }
    }
    else
    {
        assert( !mis_sender ); // we must be receiver. we can only receive inputReady when we're on the input port side of the MQ.
        return false;
    }
    return true;
}


bool MQSendRecv::mqRead(RTT::base::DataSourceBase::shared_ptr ds)
{
    int bytes = 0;
    struct timespec abs_timeout;
    clock_gettime(CLOCK_REALTIME, &abs_timeout);
    abs_timeout.tv_nsec += Seconds_to_nsecs(0.5);
    abs_timeout.tv_sec += abs_timeout.tv_nsec / (1000*1000*1000);
    abs_timeout.tv_nsec = abs_timeout.tv_nsec % (1000*1000*1000);
    //abs_timeout.tv_sec +=1;
    if ((bytes = mq_timedreceive(mqdes, buf, max_size, 0, &abs_timeout)) == -1)
    {
        //log(Debug) << "Tried read on empty mq!" <<endlog();
        return false;
    }
    if (bytes == 0) {
        log(Error) << "Failed to read from MQ Channel Element: no data received within 500ms!" <<endlog();
        return false;
    }
    if (mtransport.updateFromBlob((void*) buf, bytes, ds, marshaller_cookie))
    {
        return true;
    }
    return false;
}

bool MQSendRecv::mqWrite(RTT::base::DataSourceBase::shared_ptr ds)
{
    std::pair<void const*, int> blob = mtransport.fillBlob(ds, buf, max_size, marshaller_cookie);
    if (blob.first == 0)
    {
        log(Error) << "MQChannel: failed to marshal sample" << endlog();
        return false;
    }

    char* lbuf = (char*) blob.first;
    if (mq_send(mqdes, lbuf, blob.second, 0) == -1)
    {
        if (errno == EAGAIN)
            return true;

        log(Error) << "MQChannel "<< mqdes << " became invalid (mq length="<<max_size<<", msg length="<<blob.second<<"): " << strerror(errno) << endlog();
        return false;
    }
    return true;
}

