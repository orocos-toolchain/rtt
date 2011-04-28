#ifndef DATAOBJECTDATASOURCE_HPP_
#define DATAOBJECTDATASOURCE_HPP_

#include "DataSource.hpp"
#include "../base/DataObjectInterface.hpp"

namespace RTT
{
    namespace internal {
        /**
         * A DataSource which is used to read a DataObject.
         * When it returns a const reference, it returns a
         * reference to a copy, since we can't get a reference
         * to the internals of a DataObject.
         * @param T The result data type of get().
         */
        template<typename T>
        class DataObjectDataSource
            : public DataSource<T>
        {
            typename base::DataObjectInterface<T>::shared_ptr mobject;
            mutable T mcopy;
        public:
            typedef boost::intrusive_ptr<DataObjectDataSource<T> > shared_ptr;

            DataObjectDataSource(typename base::DataObjectInterface<T>::shared_ptr obj)
            : mobject(obj)
              {}

            virtual ~DataObjectDataSource() { }

            bool evaluate() const {
                mobject->Get( mcopy );
                return true;
            }

            typename DataSource<T>::result_t get() const
            {
                mobject->Get(mcopy);
                return mcopy;
            }

            typename DataSource<T>::result_t value() const
            {
                return mcopy;
            }

            typename DataSource<T>::const_reference_t rvalue() const
            {
                return mcopy;
            }

            virtual void reset() { }

            virtual DataObjectDataSource<T>* clone() const {
                return new DataObjectDataSource(mobject);
            }
            virtual DataObjectDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                return new DataObjectDataSource(mobject);
            }
        };
    }
}

#endif /* DATAOBJECTDATASOURCE_HPP_ */
