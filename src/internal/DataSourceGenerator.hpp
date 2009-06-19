/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  DataSourceGenerator.hpp

                        DataSourceGenerator.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef ORO_EXECUTION_DATASOURCE_GENERATOR_HPP
#define ORO_EXECUTION_DATASOURCE_GENERATOR_HPP

#include "DataSources.hpp"
#include <boost/utility/enable_if.hpp>
#include <boost/ref.hpp>

namespace RTT
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

        template<class T>
        struct DSWrap<T, typename boost::enable_if< boost::is_reference<T> >::type > {
            DataSourceBase::shared_ptr operator()(T t) { return new ReferenceDataSource<T>( t ); }
        }; // datasource type

        template<class T>
        struct DSWrap<T, typename boost::enable_if< boost::is_reference_wrapper<T> >::type > {
            typedef typename boost::unwrap_reference<T>::type RT;
            DataSourceBase::shared_ptr operator()(T t) { return new ReferenceDataSource<RT>( t ); }
        }; // datasource type
    }

    using boost::ref;

    /**
     * A function object which returns a vector of DataSources.
     * You can provide plain value types (int, double,...) or DataSourceBase derived
     * types. If you want the DataSource to be a reference to
     * an existing variable, use the \a ref() function. For example :
     * @verbatim

     DataSourceBase::shared_ptr arg2 = ...
     ValueDataSource<ClassX>::shared_ptr arg3 = ...
     double arg4 = 1.234;
     vector<DataSourceBase::shared_ptr> arguments = GenerateDataSource()( 12.34, arg2.get(), arg3.get(), ref(arg4) );
     assert ( arguments.size() == 4 );

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
