/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  BufferConnection.hpp 

                        BufferConnection.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_EXECUTION_BUFFER_CONNECTION_HPP
#define ORO_EXECUTION_BUFFER_CONNECTION_HPP

#include <vector>
#include "BufferConnectionInterface.hpp"
#include "BufferDataSource.hpp"

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

        virtual BufferBase::shared_ptr getBuffer() const {
            return buf;
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

}
#endif
