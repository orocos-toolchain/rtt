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
#include "BufferPort.hpp"
#include "DataPort.hpp"
#include "BufferConnectionInterface.hpp"
#include <corelib/BufferLockFree.hpp>
#include "BufferDataSource.hpp"

namespace ORO_Execution
{
    /**
     * A local connection with a Buffer, which is used to connect multiple
     * Ports to that Buffer.
     */
    template<class T, class BufferType = ORO_CoreLib::BufferLockFree<T> >
    class BufferConnection
        : public BufferConnectionInterface<T>
    {
        typedef T DataType;
        typedef ReadBufferPort<DataType> Reader;
        typedef WriteBufferPort<DataType> Writer;
        typename ORO_CoreLib::BufferInterface<T>::shared_ptr buf;
        typedef std::vector<Reader*> RList;
        RList readers;
        typedef std::vector<Writer*> WList;
        WList writers;
        bool mconnected;
    public:
        BufferConnection( WriteBufferPort<DataType>* w, ReadBufferPort<DataType>* r, int size)
            : buf( new BufferType(size) ), mconnected(false)
        {
            readers.push_back(r);
            writers.push_back(w);
        }

        ~BufferConnection() { }

        virtual DataSourceBase::shared_ptr getDataSource() const {
            return DataSourceBase::shared_ptr( new BufferDataSource<T>( buf ) );
        }

        bool connect()
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

        virtual bool connected() const { return mconnected; }

        virtual bool disconnect()
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
        
        virtual ORO_CoreLib::BufferInterface<T>* buffer() { return buf.get(); }
        virtual ORO_CoreLib::ReadInterface<T>* read() { return buf.get(); }
        virtual ORO_CoreLib::WriteInterface<T>* write() { return buf.get(); }

        virtual bool addReader(PortInterface* r)
        {
            Reader* newr = dynamic_cast<Reader*>(r);
            if ( newr == 0 ) return false;
            readers.push_back( newr );
            if ( this->connected() )
                newr->connect(this);
            return true;
        }

        virtual bool removeReader(PortInterface* r) {
            typename RList::iterator it( find(readers.begin(), readers.end(), r ) );
            if ( it != readers.end() ) {
                readers.erase(it);
                return true;
            }
            return false;
        }


        virtual bool addWriter(PortInterface* w)
        {
            Writer* neww = dynamic_cast<Writer*>(w);
            if ( neww == 0 ) return false;
            writers.push_back( neww );
            if ( this->connected() )
                neww->connect(this);
            return true;
        }

        virtual bool removeWriter(PortInterface* r) {
            typename WList::iterator it( find(writers.begin(), writers.end(), r ) );
            if ( it != writers.end() ) {
                writers.erase(it);
                return true;
            }
            return false;
        }
    };
}

#endif
