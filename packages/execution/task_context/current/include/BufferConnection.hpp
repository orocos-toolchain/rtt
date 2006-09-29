/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  BufferConnection.hpp 

                        BufferConnection.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_EXECUTION_BUFFER_CONNECTION_HPP
#define ORO_EXECUTION_BUFFER_CONNECTION_HPP

#include <vector>
#include "BufferConnectionInterface.hpp"
#include "BufferDataSource.hpp"
#ifdef OROPKG_CORBA
#include "corba/Services.hpp"
#include "corba/CorbaBuffer.hpp"
#include "corba/DataFlowI.h"
#endif

namespace RTT
{
    template<class T>
    class ReadBufferPort;

    template<class T>
    class WriteBufferPort;

    /**
     * A local connection with a Buffer, which is used to connect multiple
     * Ports to that Buffer.
     * @see ConnectionInterface why connection management is not thread-safe
     * in this class.
     */
    template<class T>
    class BufferConnection
        : public BufferConnectionInterface<T>
    {
        typedef T DataType;
        typedef ReadBufferPort<DataType> Reader;
        typedef WriteBufferPort<DataType> Writer;
        typename BufferInterface<T>::shared_ptr buf;
        typedef std::vector<Reader*> RList;
        RList readers;
        typedef std::vector<Writer*> WList;
        WList writers;
        bool mconnected;
    public:
        /**
         * Create an BufferConnection with initially no readers and no writers.
         */
        BufferConnection( BufferInterface<T>* bufi )
            : buf( bufi ), mconnected(false)
        {
        }

        /**
         * Create an BufferConnection with initially one reader and one writer.
         */
        BufferConnection( WriteBufferPort<DataType>* w, ReadBufferPort<DataType>* r, BufferInterface<T>* bufi )
            : buf( bufi ), mconnected(false)
        {
            readers.push_back(r);
            writers.push_back(w);
        }

        /**
         * The cleanup of BufferConnection does nothing. The only cause
         * of deletion is that no Port holds a pointer to this
         * connection (refcount becomes zero), hence, all readers and
         * writers are already disconnected.
         */
        ~BufferConnection() { }

        virtual DataSourceBase::shared_ptr getDataSource() const {
            return DataSourceBase::shared_ptr( new BufferDataSource<T>( buf ) );
        }

        void setImplementation( BufferInterface<T>* bufi )
        {
            if (bufi) {
                buf.reset( bufi );
            }
        }

        bool connect();

        virtual bool connected() const { return mconnected; }

        virtual bool disconnect();
        
        virtual BufferInterface<T>* buffer() { return buf.get(); }
        virtual ReadInterface<T>* read() { return buf.get(); }
        virtual WriteInterface<T>* write() { return buf.get(); }

        virtual bool addReader(PortInterface* r);

        virtual bool removeReader(PortInterface* r);

        virtual bool addWriter(PortInterface* w);

        virtual bool removeWriter(PortInterface* w);

#ifdef OROPKG_CORBA
        virtual CORBA::Object_ptr toChannel();
#endif
    };
}

#endif

#ifndef ORO_BUFFER_CONNECTION_INLINE
#define ORO_BUFFER_CONNECTION_INLINE
#include "BufferPort.hpp"

namespace RTT
{
    template<class T>
    bool BufferConnection<T>::connect()
    { 
        if (!mconnected) { 
            for ( typename WList::iterator it = ( writers.begin() ); it != writers.end(); ++it)
                (*it)->connect(this);
            for ( typename RList::iterator it = ( readers.begin() ); it != readers.end(); ++it)
                (*it)->connect(this);
            mconnected = true;
            return true;
        }
        return false;
    }
    template<class T>
    bool BufferConnection<T>::disconnect()
    {
        if (mconnected) { 
            for ( typename WList::iterator it( writers.begin() ); it != writers.end(); ++it)
                (*it)->disconnect();
            for ( typename RList::iterator it( readers.begin() ); it != readers.end(); ++it)
                (*it)->disconnect();
            mconnected = false;
            return true;
        }
        return false;
    }

    template<class T>
    bool BufferConnection<T>::addReader(PortInterface* r)
    {
        Reader* newr = dynamic_cast<Reader*>(r);
        if ( newr == 0 ) return false;
        readers.push_back( newr );
        if ( this->connected() )
            newr->connect(this);
        return true;
    }

    template<class T>
    bool BufferConnection<T>::removeReader(PortInterface* r) {
        typename RList::iterator it( find(readers.begin(), readers.end(), r ) );
        if ( it != readers.end() ) {
            readers.erase(it);
            return true;
        }
        return false;
    }

    template<class T>
    bool BufferConnection<T>::addWriter(PortInterface* w)
    {
        Writer* neww = dynamic_cast<Writer*>(w);
        if ( neww == 0 ) return false;
        writers.push_back( neww );
        if ( this->connected() )
            neww->connect(this);
        return true;
    }

    template<class T>
    bool BufferConnection<T>::removeWriter(PortInterface* w) {
        typename WList::iterator it( find(writers.begin(), writers.end(), w ) );
        if ( it != writers.end() ) {
            writers.erase(it);
            return true;
        }
        return false;
    }

#ifdef OROPKG_CORBA
    template<class T>
    CORBA::Object_ptr BufferConnection<T>::toChannel() {
        log(Debug) << "Creating Corba BufferChannel." << endlog();
        RTT_Corba_BufferChannel_i<T>* cbuf = new RTT_Corba_BufferChannel_i<T>( buf );
        // activate servant:
        CORBA::Object_var ret = cbuf->_this();
        return ret._retn();
    }
#endif

}
#endif
