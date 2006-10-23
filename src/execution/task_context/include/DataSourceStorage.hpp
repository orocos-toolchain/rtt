#ifndef ORO_TASK_DATASOURCE_STORAGE_HPP
#define ORO_TASK_DATASOURCE_STORAGE_HPP

#include <boost/function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_arity.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include "CommandC.hpp"

namespace RTT
{
    namespace detail
    {
        // Partial specialisations for storing a void or not a void.
        template<class R>
        struct DataSourceResultStorage
        {
            typename ValueDataSource<R>::shared_ptr value;
            DataSourceResultStorage()
                : value(new ValueDataSource<R>() )
            {}
        };

        template<>
        struct DataSourceResultStorage<void>
        {
        };

        // Partial specialisations for storing a reference or not reference
        template<class A>
        struct DataSourceArgStorage
        {
            typename ValueDataSource<A>::shared_ptr value;
            DataSourceArgStorage()
                : value(new ValueDataSource<A>() )
            {}
        };

#if 0
        // references not used for now:
        // the result will not be communicated back to user code.
        template<class A>
        struct DataSourceArgStorage<A&>
        {
            ReferenceDataSource<A&>::shared_ptr value;
            DataSourceArgStorage(A& a)
                : value(new ReferenceDataSource<A&>(a) )
            {}
        };
#endif


        template<int, class T>
        struct DataSourceStorageImpl;

        /**
         * When no arguments are to be stored.
         */
        template<class DataType>
        struct DataSourceStorageImpl<0, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            template<class ContainerT>
            void initArgs(ContainerT& ) {}
        };

        /**
         * Stores one data source.
         */
        template<class DataType>
        struct DataSourceStorageImpl<1, DataType>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            DataSourceArgStorage<arg1_type> ma1;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( ma1.value );
            }

            void store(arg1_type a1) {
                ma1.value->set(a1);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<2, DataType>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( ma1.value );
                cc.arg( ma2.value );
            }
            void store(arg1_type a1, arg2_type a2) {
                ma1.value->set(a1);
                ma2.value->set(a2);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<3, DataType>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( ma1.value );
                cc.arg( ma2.value );
                cc.arg( ma3.value );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3) {
                ma1.value->set(a1);
                ma2.value->set(a2);
                ma3.value->set(a3);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<4, DataType>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            typedef typename boost::function_traits<DataType>::arg4_type   arg4_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;
            DataSourceArgStorage<arg4_type> ma4;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( ma1.value );
                cc.arg( ma2.value );
                cc.arg( ma3.value );
                cc.arg( ma4.value );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) {
                ma1.value->set(a1);
                ma2.value->set(a2);
                ma3.value->set(a3);
                ma4.value->set(a4);
            }
        };


        /**
         * A helper-class for the Command implementation which stores the
         * command and condition function objects. It can store both
         * pointers to member functions (with their object pointer) and
         * plain C functions.
         */
        template<class DataType>
        struct DataSourceStorage
            : public DataSourceStorageImpl<boost::function_traits<DataType>::arity, DataType>
        {
        };
    }
}
#endif
