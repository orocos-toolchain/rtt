/***************************************************************************
  tag: Peter Soetens  Fri Feb 24 17:02:57 CET 2006  EventCallBack.hpp

                        EventCallBack.hpp -  description
                           -------------------
    begin                : Fri February 24 2006
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


#ifndef ORO_EXEC_EVENT_CALLBACK_HPP
#define ORO_EXEC_EVENT_CALLBACK_HPP

#include <vector>
#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include "../boost/function_types/function_type_arity.hpp"
#include "../boost/function_types/function_type_parameter.hpp"
#include "DataSource.hpp"
#include "DataSources.hpp"

namespace RTT {
    namespace internal {

        /**
         * Class used by ConnectionC to 'wrap' user functions
         * to a form usable by the interface::EventService, namely a void(void)
         * function with a list of AssignableDataSources which store
         * the event arguments.
         */
        struct RTT_API EventCallBack
        {
            virtual ~EventCallBack() {}
            virtual void callback() = 0;
            virtual std::vector<base::DataSourceBase::shared_ptr> args() = 0;
        };

        template<class Function, int arity>
        struct CallBackWrapperFunction;

        // If we could detect the arity _and_ type type of each argument
        // of a boost::bind object, this implementation would suffice.
        // See the commented out code in ConnectionC.hpp
        template<class Function>
        struct CallBackWrapperFunction<Function,0>
            :public EventCallBack
        {
            CallBackWrapperFunction( Function& foo ) : mfoo(foo) {}
            Function mfoo;

            virtual void callback()
                {
                    mfoo();
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                return std::vector<base::DataSourceBase::shared_ptr>();
            }
        };

        template<class Function>
        struct CallBackWrapperFunction<Function,1>
            :public EventCallBack
        {
            typedef typename boost::function_types::function_type_parameter_c<Function,0>::type T;
            CallBackWrapperFunction( Function& foo )
                : mfoo(foo), marg( new ValueDataSource<T>() ) {}
            Function mfoo;
            typename AssignableDataSource<T>::shared_ptr marg;

            virtual void callback()
                {
                    mfoo( marg->set() );
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                std::vector<base::DataSourceBase::shared_ptr> a;
                a.push_back( marg );
                return a;
            }
        };

        template<class Function>
        struct CallBackWrapperFunction<Function,2>
            :public EventCallBack
        {
            typedef typename boost::function_types::function_type_parameter_c<Function,0>::type T1;
            typedef typename boost::function_types::function_type_parameter_c<Function,1>::type T2;
            CallBackWrapperFunction( Function& foo)
                : mfoo(foo),
                marg1( new ValueDataSource<T1>() ),
                marg2( new ValueDataSource<T2>() ) {}

            Function mfoo;
            typename AssignableDataSource<T1>::shared_ptr marg1;
            typename AssignableDataSource<T2>::shared_ptr marg2;

            virtual void callback()
                {
                    mfoo( marg1->set(), marg2->set() );
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                std::vector<base::DataSourceBase::shared_ptr> a;
                a.push_back( marg1 );
                a.push_back( marg2 );
                return a;
            }
        };

        template<class Function>
        struct CallBackWrapperFunction<Function, 3>
            :public EventCallBack
        {
            typedef typename boost::function_types::function_type_parameter_c<Function,0>::type T1;
            typedef typename boost::function_types::function_type_parameter_c<Function,1>::type T2;
            typedef typename boost::function_types::function_type_parameter_c<Function,2>::type T3;
            CallBackWrapperFunction( Function foo)
                : mfoo(foo),
                marg1( new ValueDataSource<T1>() ),
                marg2( new ValueDataSource<T2>() ),
                marg3( new ValueDataSource<T3>() ) {}
            Function mfoo;
            typename AssignableDataSource<T1>::shared_ptr marg1;
            typename AssignableDataSource<T2>::shared_ptr marg2;
            typename AssignableDataSource<T3>::shared_ptr marg3;

            virtual void callback()
                {
                    mfoo( marg1->set(), marg2->set(), marg3->set() );
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                std::vector<base::DataSourceBase::shared_ptr> a;
                a.push_back( marg1 );
                a.push_back( marg2 );
                a.push_back( marg3 );
                return a;
            }
        };

        template<class Function, int arity>
        struct CallBackWrapper;

        // No bind arity detection work around
        template<class Function>
        struct CallBackWrapper<Function,0>
            :public EventCallBack
        {
            template<class Type>
            CallBackWrapper( Type t, Function& foo )
                : mfoo( boost::bind(foo, t) ) {}
            boost::function<void(void)> mfoo;

            virtual void callback()
                {
                    mfoo();
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                return std::vector<base::DataSourceBase::shared_ptr>();
            }
        };

        template<class Function>
        struct CallBackWrapper<Function,1>
            :public EventCallBack
        {
            typedef typename boost::function_types::function_type_parameter_c<Function,0>::type T;

            template<class Type>
            CallBackWrapper( Type t, Function& foo )
                : mfoo( boost::bind(foo, t, _1) ), marg( new ValueDataSource<T>() )
            {}
            boost::function<void(T)> mfoo;

            typename AssignableDataSource<T>::shared_ptr marg;

            virtual void callback()
                {
                    mfoo( marg->set() );
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                std::vector<base::DataSourceBase::shared_ptr> a;
                a.push_back( marg );
                return a;
            }
        };

        template<class Function>
        struct CallBackWrapper<Function,2>
            :public EventCallBack
        {
            typedef typename boost::function_types::function_type_parameter_c<Function,0>::type T1;
            typedef typename boost::function_types::function_type_parameter_c<Function,1>::type T2;

            boost::function<void(T1,T2)> mfoo;
            template<class Type>
            CallBackWrapper( Type t, Function& foo )
                : mfoo( boost::bind(foo, t, _1, _2) ),
                marg1( new ValueDataSource<T1>() ),
                marg2( new ValueDataSource<T2>() ) {}

            typename AssignableDataSource<T1>::shared_ptr marg1;
            typename AssignableDataSource<T2>::shared_ptr marg2;

            virtual void callback()
                {
                    mfoo( marg1->set(), marg2->set() );
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                std::vector<base::DataSourceBase::shared_ptr> a;
                a.push_back( marg1 );
                a.push_back( marg2 );
                return a;
            }
        };

        template<class Function>
        struct CallBackWrapper<Function, 3>
            :public EventCallBack
        {
            typedef typename boost::function_types::function_type_parameter_c<Function,0>::type T1;
            typedef typename boost::function_types::function_type_parameter_c<Function,1>::type T2;
            typedef typename boost::function_types::function_type_parameter_c<Function,2>::type T3;

            boost::function<void(T1,T2,T3)> mfoo;
            template<class Type>
            CallBackWrapper( Type t, Function& foo )
                : mfoo( boost::bind(foo, t, _1, _2, _3) ),
                marg1( new ValueDataSource<T1>() ),
                marg2( new ValueDataSource<T2>() ),
                marg3( new ValueDataSource<T3>() ) {}

            typename AssignableDataSource<T1>::shared_ptr marg1;
            typename AssignableDataSource<T2>::shared_ptr marg2;
            typename AssignableDataSource<T3>::shared_ptr marg3;

            virtual void callback()
                {
                    mfoo( marg1->set(), marg2->set(), marg3->set() );
                }

            virtual std::vector<base::DataSourceBase::shared_ptr> args() {
                std::vector<base::DataSourceBase::shared_ptr> a;
                a.push_back( marg1 );
                a.push_back( marg2 );
                a.push_back( marg3 );
                return a;
            }
        };
    }
}
#endif
