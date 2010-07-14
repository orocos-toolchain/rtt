#ifndef ORO_ARRAYPARTDATASOURCE_HPP_
#define ORO_ARRAYPARTDATASOURCE_HPP_

#include "DataSource.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * A DataSource which is used to manipulate a reference to a
         * part of a data source holding a C-style array of elements.
         *
         * This data source only works on a fixed reference in memory of
         * a given array. In case the array's place in memory
         * varies, you need the OffsetPartDataSource, which can access
         * an element at any offset from a given memory location.
         *
         * @param T The data type of an element.
         */
        template<typename T>
        class ArrayPartDataSource
        : public AssignableDataSource<T>
        {
            // a reference to a value_t
            typename AssignableDataSource<T>::value_t* mref;
            // [] index
            DataSource<unsigned int>::shared_ptr mindex;
            // parent data source, for updating after set().
            base::DataSourceBase::shared_ptr mparent;
        public:
            ~ArrayPartDataSource() {}

            typedef boost::intrusive_ptr<ArrayPartDataSource<T> > shared_ptr;

            /**
             * The part is constructed from a reference to an existing array of elements and
             * an index for accessing the n'th element.
             * @param ref    Reference to the first element of the array
             * @param index  Datasource pointing to the index for use in operator[]
             * @param parent Parent data source for sending updated() messages. May be null.
             */
            ArrayPartDataSource( typename AssignableDataSource<T>::reference_t ref,
                                    DataSource<unsigned int>::shared_ptr index,
                                    base::DataSourceBase::shared_ptr parent )
                : mref(&ref), mindex(index), mparent(parent)
            {
            }

            typename DataSource<T>::result_t get() const
            {
                return mref[ mindex->get() ];
            }

            typename DataSource<T>::result_t value() const
            {
                return mref[ mindex->get() ];
            }

            void set( typename AssignableDataSource<T>::param_t t )
            {
                mref[ mindex->get() ] = t;
                updated();
            }

            typename AssignableDataSource<T>::reference_t set()
            {
                return mref[ mindex->get() ];
            }

            typename AssignableDataSource<T>::const_reference_t rvalue() const
            {
                return mref[ mindex->get() ];
            }

            void updated() {
                if (mparent) mparent->updated();
            }

            virtual ArrayPartDataSource<T>* clone() const {
                return new ArrayPartDataSource<T>( *mref, mindex, mparent);
            }

            virtual ArrayPartDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const {
                // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
                if ( replace[this] != 0 ) {
                    assert ( dynamic_cast<ArrayPartDataSource<T>*>( replace[this] ) == static_cast<ArrayPartDataSource<T>*>( replace[this] ) );
                    return static_cast<ArrayPartDataSource<T>*>( replace[this] );
                }
                // Other pieces in the code rely on insertion in the map :
                replace[this] = new ArrayPartDataSource<T>(*mref, mindex->copy(replace), mparent->copy(replace));
                // return this instead of a copy.
                return static_cast<ArrayPartDataSource<T>*>(replace[this]);

            }
        };

    }
}

#endif /* ORO_PARTDATASOURCE_HPP_ */
