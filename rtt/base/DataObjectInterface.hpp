/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataObjectInterface.hpp

                        DataObjectInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef CORELIB_DATAOBJECTINTERFACE_HPP
#define CORELIB_DATAOBJECTINTERFACE_HPP


#include "../internal/DataSource.hpp"

namespace RTT
{ namespace base {


    /**
     * @brief A DataObjectInterface extends the internal::AssignableDataSource with
     * implementations of multi-threaded read/write solutions. It is initially not
     * reference counted, such that DataObjects may be created on the stack.
     * Store a DataObject in a shared_ptr and use this->deref() to get a reference counted version.
     * This dual policy was introduced to be consistent with the other buffer
     * implementations.
     *
     * @see DataObject
     * @param T The \a DataType which can be Get() or Set() with this DataObject.
     * @ingroup Ports
     */
    template <class T>
    class DataObjectInterface
        : public internal::AssignableDataSource<T>
    {
    public:
        /**
         * If you plan to use a reference counted DataObject, use this
         * type to store it and apply this->deref() to enable reference counting.
         */
        typedef typename boost::intrusive_ptr<DataObjectInterface<T> > shared_ptr;

        /**
         * Create a DataObject which is initially not reference counted.
         */
        DataObjectInterface() {
            this->ref();
        }

        /**
         * Destructor.
         */
        virtual ~DataObjectInterface() {}

        /**
         * The type of the data.
         */
        typedef T DataType;

        /**
         * Get a copy of the Data of this data object.
         *
         * @param pull A copy of the data.
         */
        virtual void Get( DataType& pull ) const = 0;

        /**
         * Get a copy of the data of this data object.
         *
         * @return A copy of the data.
         */
        virtual DataType Get() const = 0;

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        virtual void Set( const DataType& push ) = 0;

        /**
         * Provides a data sample to initialize this data object.
         * As such enough storage
         * space can be allocated before the actual writing begins.
         *
         * @param sample
         */
        virtual void data_sample( const DataType& sample ) = 0;

        /**
         * Normally, value() does not trigger a get(), but for
         * DataObjects, this is actually the sanest thing to
         * do.
         */
        typename internal::DataSource<T>::result_t value() const {
            return this->Get();
        }

        virtual typename internal::DataSource<DataType>::result_t get() const {
            return this->Get();
        }

        virtual void set( typename internal::AssignableDataSource<DataType>::param_t t ) {
            this->Set( t );
        }

        virtual typename internal::AssignableDataSource<DataType>::reference_t set() {
            // return null reference, allowed by API.
            typename internal::DataSource<DataType>::value_t* tmp = 0;
            return typename internal::AssignableDataSource<DataType>::reference_t(*tmp);
        }

        virtual typename internal::AssignableDataSource<DataType>::const_reference_t rvalue() const {
            // return null reference, allowed by API.
            typename internal::DataSource<DataType>::value_t* tmp = 0;
            return typename internal::AssignableDataSource<DataType>::const_reference_t(*tmp);
        }

    };
}}

#endif

