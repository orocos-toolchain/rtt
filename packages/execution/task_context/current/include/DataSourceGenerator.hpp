#ifndef ORO_EXECUTION_DATASOURCE_GENERATOR_HPP
#define ORO_EXECUTION_DATASOURCE_GENERATOR_HPP

#include "DataSource.hpp"
#include <boost/utility/enable_if.hpp>

namespace ORO_Execution
{
    namespace detail
    {
        template<class T, class Enable = void >
        struct DSWrap {
            DataSourceBase::shared_ptr operator()(T t) { return new ConstantDataSource<T>( t );}
        }; // normal type

        template<class T>
        struct DSWrap<T, typename boost::enable_if< boost::is_base_and_derived<DataSourceBase,T> >::type > {
            DataSourceBase::shared_ptr operator()(T t) { return t; }
        }; // datasource type
    }

    /**
     * A function object which returns a vector of DataSources.
     * You can provide plain value types (int, double,...) or DataSourceBase derived
     * types. For example : 
     * @verbatim
     
     DataSourceBase::shared_ptr arg2 = ...
     VariableDataSource<ClassX>::shared_ptr arg3 = ...
     vector<DataSourceBase::shared_ptr> arguments = GenerateDataSource()( 12.34, arg2.get(), arg3.get() );
     assert ( arguments.size() == 3 );

       @endverbatim
     */
    struct GenerateDataSource
    {
        template<class A1>
        std::vector<DataSourceBase::shared_ptr> operator()(A1 a1)
        {
            std::vector<DataSourceBase::shared_ptr> res;
            res.push_back( detail::DSWrap<A1>()(a1));
            return res;
        }

        template<class A1, class A2>
        std::vector<DataSourceBase::shared_ptr> operator()(A1 a1, A2 a2 )
        {
            std::vector<DataSourceBase::shared_ptr> res = operator()(a1);
            res.push_back( detail::DSWrap<A2>()(a2));
            return res;
        }

        template<class A1, class A2, class A3>
        std::vector<DataSourceBase::shared_ptr> operator()(A1 a1, A2 a2, A3 a3 )
        {
            std::vector<DataSourceBase::shared_ptr> res = operator()(a1,a2);
            res.push_back( detail::DSWrap<A3>()(a3));
            return res;
        }

        template<class A1, class A2, class A3, class A4>
        std::vector<DataSourceBase::shared_ptr> operator()(A1 a1, A2 a2, A3 a3, A4 a4 )
        {
            std::vector<DataSourceBase::shared_ptr> res = operator()(a1,a2,a3);
            res.push_back( detail::DSWrap<A4>()(a4));
            return res;
        }
    };
}

#endif
