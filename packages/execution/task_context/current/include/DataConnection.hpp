/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  DataConnection.hpp 

                        DataConnection.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_DATA_CONNECTION_HPP
#define ORO_EXECUTION_DATA_CONNECTION_HPP

#include "DataPort.hpp"
#include "DataConnectionInterface.hpp"

namespace RTT
{
    /**
     * A local connection with a DataObject, which is used to connect multiple
     * Ports to that DataObject.
     */
    template<class T, class DataObjectType = DataObjectLockFree<T> >
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
        DataConnection(WriteDataPort<DataType>* writer,ReadDataPort<DataType>* reader, const T& initial_value)
            : buf( new DataObjectType("DataObject", initial_value, true) ), mconnected(false)
        {
            readers.push_back(reader);
            writers.push_back(writer);
        }

        DataConnection(WriteDataPort<DataType>* writer,ReadDataPort<DataType>* reader)
            : buf( new DataObjectType("DataObject") ), mconnected(false)
        {
            readers.push_back(reader);
            writers.push_back(writer);
        }

        ~DataConnection() { }

        virtual DataSourceBase::shared_ptr getDataSource() const {
            return buf;
        }

        bool connect()
        { 
            if (!mconnected) { 
                for ( typename WList::iterator it( writers.begin() ); it != writers.end(); ++it)
                    (*it)->connect(this);
                for ( typename RList::iterator it( readers.begin() ); it != readers.end(); ++it)
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
        
        virtual DataObjectInterface<T>* data() { return buf.get(); }

        virtual const DataObjectInterface<T>* data() const { return buf.get(); }

        virtual bool addReader(PortInterface* r)
        {
            ReadDataPort<DataType>* newr = dynamic_cast<ReadDataPort<DataType>*>(r);
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
            WriteDataPort<DataType>* neww = dynamic_cast<WriteDataPort<DataType>*>(w);
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
