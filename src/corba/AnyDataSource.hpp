#ifndef ORO_CORBA_ANYDATASOURCE_HPP
#define ORO_CORBA_ANYDATASOURCE_HPP

#include "DataSource.hpp"
#include <tao/corba.h>

namespace RTT
{
    namespace Corba
    {
        /**
         * A DataSource which holds an any value and
         * returns it in its get() method. It can not be changed after creation.
         */
        class AnyDataSource
            : public DataSource<CORBA::Any_var>
        {
            /**
             * The any.
             */
            CORBA::Any_var mdata;

        public:
            /**
             * Use shared_ptr.
             */
            ~AnyDataSource();

            typedef boost::intrusive_ptr<AnyDataSource> shared_ptr;

            AnyDataSource( CORBA::Any_ptr value );

            DataSource<CORBA::Any_var>::result_t get() const;

            DataSource<CORBA::Any_var>::result_t value() const;

            virtual AnyDataSource* clone() const;

            virtual AnyDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;

            virtual int serverProtocol() const;

        };
    }
}
#endif

