
#ifndef CORELIB_DATAOBJECT_UNSYNC_HPP
#define CORELIB_DATAOBJECT_UNSYNC_HPP


#include "DataObjectInterface.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief A class which provides unprotected (\b not thread-safe) access to one typed element of data.
     *
     * @ingroup Ports
     */
    template<class T>
    class DataObjectUnSync
        : public DataObjectInterface<T>
    {
        /**
         * One element of Data.
         */
        T data;
    public:
        /**
         * Construct a DataObjectUnSync by name.
         *
         * @param _name The name of this DataObject.
         */
        DataObjectUnSync( const T& initial_value = T() )
            : data(initial_value) {}

        /**
         * The type of the data.
         */
        typedef T DataType;

        virtual void Get( DataType& pull ) const { pull = data; }

        virtual DataType Get() const { DataType cache;  Get(cache); return cache; }

        virtual void Set( const DataType& push ) { data = push; }

        virtual void data_sample( const DataType& sample ) {
            Set(sample);
        }

        virtual DataObjectUnSync<DataType>* clone() const {
            return new DataObjectUnSync<DataType>();
        }

        virtual DataObjectUnSync<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) const {
            return const_cast<DataObjectUnSync<DataType>*>(this);
        }
    };
}}

#endif

