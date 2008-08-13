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

#include "ConnectionInterface.hpp"
#include "DataObjectInterfaces.hpp"

namespace RTT
{
    /**
     * A local connection with a DataObject, which is used to connect multiple
     * Ports to that DataObject.
     *
     * @see ConnectionInterface why connection management is not thread-safe
     * in this class.
     */
    template<class T>
    class DataConnection
        :public ConnectionInterface
    {
        typedef T DataType;
        typename DataObjectInterface<T>::shared_ptr buf;
    public:
        typedef boost::intrusive_ptr<DataConnection> shared_ptr;
        /**
         * Create a Data Connection with initially no readers and no writers.
         */
        DataConnection(DataObjectInterface<T>* doi)
            : buf( doi )
        {
            // See DataObjectInterface constructor docs.
            doi->deref();
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

        virtual const TypeInfo* getTypeInfo() const {
            return detail::DataSourceTypeInfo<T>::getTypeInfo();
        }

        void setImplementation( DataObjectInterface<T>* doi ) {
            if ( doi ) {
                buf = doi;
            }
        }

        DataObjectInterface<T>* data() { return buf.get(); }

        const DataObjectInterface<T>* data() const { return buf.get(); }
    };
}

#endif

