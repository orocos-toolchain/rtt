/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  DataConnection.hpp 

                        DataConnection.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_DATA_CONNECTION_HPP
#define ORO_EXECUTION_DATA_CONNECTION_HPP

#include "DataConnectionInterface.hpp"
#include "DataObjectInterfaces.hpp"

namespace RTT
{
    template<class T>
    class ReadDataPort;
    template<class T>
    class WriteDataPort;

    /**
     * A local connection with a DataObject, which is used to connect multiple
     * Ports to that DataObject.
     *
     * @see ConnectionInterface why connection management is not thread-safe
     * in this class.
     */
    template<class T>
    class DataConnection
        :public DataConnectionInterface<T>
    {
        typedef T DataType;
        typename DataObjectInterface<T>::shared_ptr buf;
        typedef std::vector<ReadDataPort<DataType>*> RList;
        RList readers;
        typedef std::vector<WriteDataPort<DataType>*> WList;
        WList writers;
        bool mconnected;
    public:
        /**
         * Create a Data Connection with initially no readers and no writers.
         */
        DataConnection(DataObjectInterface<T>* doi)
            : buf( doi ), mconnected(false)
        {
            // See DataObjectInterface constructor docs.
            doi->deref();
        }

        /**
         * Create a Data Connection with initially one reader and one writer.
         */
        DataConnection(WriteDataPort<DataType>* writer,ReadDataPort<DataType>* reader, DataObjectInterface<T>* doi)
            : buf( doi ), mconnected(false)
        {
            // See DataObjectInterface constructor docs.
            doi->deref();
            readers.push_back(reader);
            writers.push_back(writer);
        }

        /**
         * The cleanup of DataConnection does nothing. The only cause
         * of deletion is that no Port holds a pointer to this
         * connection (refcount becomes zero), hence, all readers and
         * writers are already disconnected.
         */
        ~DataConnection() { }

        virtual DataSourceBase::shared_ptr getDataSource() const {
            return buf;
        }

        virtual BufferBase::shared_ptr getBuffer() const {
            return BufferBase::shared_ptr();
        }

        void setImplementation( DataObjectInterface<T>* doi ) {
            if ( doi ) {
                buf = doi;
            }
        }

        bool connect();

        virtual bool connected() const { return mconnected; }

        virtual bool disconnect();
        
        virtual DataObjectInterface<T>* data() { return buf.get(); }

        virtual const DataObjectInterface<T>* data() const { return buf.get(); }

        virtual bool addReader(PortInterface* r);

        virtual bool removeReader(PortInterface* r);

        virtual bool addWriter(PortInterface* w);

        virtual bool removeWriter(PortInterface* w);
        
    };
}

#endif
#ifndef ORO_DATA_CONNECTION_INLINE
#define ORO_DATA_CONNECTION_INLINE
#include "DataPort.hpp"

namespace RTT
{
    template<class T>
    bool DataConnection<T>::connect()
    { 
        // note, this method is not thread-safe.
        if (!mconnected) { 

            // first check existing connections:
            for ( typename WList::iterator it( writers.begin() ); it != writers.end(); ++it)
                if ((*it)->connected())
                    return false;
            for ( typename RList::iterator it( readers.begin() ); it != readers.end(); ++it)
                if ((*it)->connected())
                    return false;

            // now do connect:
            for ( typename WList::iterator it( writers.begin() ); it != writers.end(); ++it)
                (*it)->connect(this);
            for ( typename RList::iterator it( readers.begin() ); it != readers.end(); ++it)
                (*it)->connect(this);
            mconnected = true;
            return true;
        }
        return false;
    }

    template<class T>
    bool DataConnection<T>::disconnect()
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
    bool DataConnection<T>::addReader(PortInterface* r)
    {
        ReadDataPort<DataType>* newr = dynamic_cast<ReadDataPort<DataType>*>(r);
        if ( newr == 0 ) return false;
        readers.push_back( newr );
        if ( this->connected() )
            newr->connect(this);
        return true;
    }

    template<class T>
    bool DataConnection<T>::removeReader(PortInterface* r) {
        typename RList::iterator it( find(readers.begin(), readers.end(), r ) );
        if ( it != readers.end() ) {
            readers.erase(it);
            return true;
        }
        return false;
    }

    template<class T>
    bool DataConnection<T>::addWriter(PortInterface* w)
    {
        WriteDataPort<DataType>* neww = dynamic_cast<WriteDataPort<DataType>*>(w);
        if ( neww == 0 ) return false;
        writers.push_back( neww );
        if ( this->connected() )
            neww->connect(this);
        return true;
    }

    template<class T>
    bool DataConnection<T>::removeWriter(PortInterface* w) {
        typename WList::iterator it( find(writers.begin(), writers.end(), w ) );
        if ( it != writers.end() ) {
            writers.erase(it);
            return true;
        }
        return false;
    }
        
}

#endif

