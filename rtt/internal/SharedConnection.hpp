/***************************************************************************
  tag: Intermodalics   Thu Jul 30 18:28:12 CEST 2015  SharedConnection.hpp

                   SharedConnection.hpp -  description
                           -------------------
    begin                : Thu July 30 2015
    copyright            : (C) 2015 Intermodalics
    email                : johannes@intermodalics.eu

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

#ifndef ORO_SHARED_CONNECTION_HPP
#define ORO_SHARED_CONNECTION_HPP

#include "ConnID.hpp"
#include "../base/ChannelElement.hpp"
#include "../ConnPolicy.hpp"

#include <map>


std::ostream &operator<<(std::ostream &, const RTT::internal::SharedConnID &);

namespace RTT {
namespace internal {

    /**
     * Represents a shared connection created by the ConnFactory.
     */
    struct RTT_API SharedConnID : public ConnID
    {
        boost::intrusive_ptr< SharedConnectionBase > connection;
        SharedConnID(boost::intrusive_ptr< SharedConnectionBase > connection)
            : connection(connection) {}
        virtual ConnID* clone() const;
        virtual bool isSameID(ConnID const& id) const;
        template <typename T> boost::intrusive_ptr< SharedConnection<T> > getConnection() const
        {
            return boost::static_pointer_cast< SharedConnection<T> >(connection);
        }
    };

    /**
     * Base class for shared connection elements.
     */
    class RTT_API SharedConnectionBase : virtual public base::ChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr< SharedConnectionBase > shared_ptr;

    private:
        ConnPolicy policy;

    public:
        SharedConnectionBase(const ConnPolicy &policy);
        virtual ~SharedConnectionBase();

        /**
         * Return a pointer to the typed instance of a SharedConnectionBase
         */
        template <typename T> static SharedConnection<T> *narrow(ChannelElementBase *e)
        {
            return dynamic_cast<SharedConnection<T> *>(e);
        }

        /**
         * Return a pointer to the typed variant of this SharedConnectionBase
         */
        template <typename T> SharedConnection<T> *narrow()
        {
            return dynamic_cast<SharedConnection<T> *>(this);
        }

        virtual SharedConnID *getConnID();
        virtual const std::string &getName() const;

        virtual const ConnPolicy *getConnPolicy() const;
    };

    /**
     * A repository which stores pointers to all shared connections within the process.
     */
    class RTT_API SharedConnectionRepository
    {
    public:
        typedef boost::shared_ptr<SharedConnectionRepository> shared_ptr;

        typedef std::string key_t;
        typedef std::map<key_t, SharedConnectionBase*> Map;

    private:
        mutable RTT::os::SharedMutex mutex;
        Map map;

    public:
        static shared_ptr Instance();

        bool add(const key_t &key, SharedConnectionBase* connection);
        void remove(SharedConnectionBase* connection);

        bool has(const key_t &key) const;
        SharedConnectionBase::shared_ptr get(const key_t &key) const;

    private:
        SharedConnectionRepository() {}
    };

    template <typename T>
    class SharedConnection : public SharedConnectionBase, public base::MultipleInputsMultipleOutputsChannelElement<T> {
    public:
        typedef boost::intrusive_ptr< SharedConnection<T> > shared_ptr;
        typedef typename base::ChannelElement<T>::value_t value_t;
        typedef typename base::ChannelElement<T>::param_t param_t;
        typedef typename base::ChannelElement<T>::reference_t reference_t;

    private:
        typename base::ChannelElement<T>::shared_ptr mstorage;
        bool mstorage_initialized;

    public:
        SharedConnection(typename base::ChannelElement<T> *storage, const ConnPolicy &policy)
            : SharedConnectionBase(policy)
            , mstorage(storage)
            , mstorage_initialized(false)
        {
            assert(policy.buffer_policy == Shared);
            // forward buffer policy to the underlying MultipleInputsMultipleOutputsChannelElement<T> instance
            (void) this->setBufferPolicy(policy.buffer_policy);
        }
        virtual ~SharedConnection() {}

        using base::MultipleInputsMultipleOutputsChannelElement<T>::signal;

        /**
         * Writes a new sample on this connection. \a sample is the sample to
         * write.
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual WriteStatus write(param_t sample)
        {
            WriteStatus result = mstorage->write(sample);
            if (result == WriteSuccess) {
                if (!this->signal()) {
                    return WriteFailure;
                }
            }
            return result;
        }

        /**
         * Reads the last sample given to write()
         *
         * @return false if no sample has ever been written, true otherwise
         */
        virtual FlowStatus read(reference_t sample, bool copy_old_data = true)
        {
            return mstorage->read(sample, copy_old_data);
        }

        /**
         * Resets the stored sample. After clear() has been called, read()
         * returns false
         */
        virtual void clear()
        {
            mstorage->clear();
            SharedConnectionBase::clear();
        }

        /**
         * Provides a data sample to initialize this connection.
         * This is used before the first write() in order to inform this
         * connection of the size of the data. As such enough storage
         * space can be allocated before the actual writing begins.
         *
         * @returns false if an error occured that requires the channel to be invalidated.
         */
        virtual WriteStatus data_sample(param_t sample, bool reset = true)
        {
            // ignore reset, only the first caller can initialize the data/buffer object
            if (!mstorage_initialized) {
                mstorage->data_sample(sample, reset);
                mstorage_initialized = true;
            }
            return base::MultipleInputsMultipleOutputsChannelElement<T>::data_sample(sample, reset);
        }

        virtual value_t data_sample()
        {
            return mstorage->data_sample();
        }
    };

    template <typename T>
    class SharedRemoteConnection : public SharedConnectionBase, public base::ChannelElement<T> {
    public:
        typedef boost::intrusive_ptr< SharedRemoteConnection<T> > shared_ptr;

    public:
        SharedRemoteConnection(const ConnPolicy &policy)
            : SharedConnectionBase(policy)
        {}
        virtual ~SharedRemoteConnection() {}
    };
}}

#endif
