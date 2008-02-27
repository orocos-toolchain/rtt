/***************************************************************************
  tag: FMTC  do nov 2 13:06:13 CET 2006  DataSourceStorage.hpp 

                        DataSourceStorage.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
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
 
 
#ifndef ORO_TASK_DATASOURCE_STORAGE_HPP
#define ORO_TASK_DATASOURCE_STORAGE_HPP

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include "boost/function_types/function_type.hpp"
#include "boost/function_types/function_type_arity.hpp"
#include "CommandC.hpp"

namespace RTT
{
    namespace detail
    {
        // Partial specialisations for storing a void, not a void or reference
        template<class R>
        struct DataSourceResultStorage
        {
            typename ValueDataSource<R>::shared_ptr result;

            DataSourceResultStorage()
                : result(new ValueDataSource<R>() )
            {
            }

            template<class ContainerT>
            void initRet(ContainerT& cc) {
                cc.ret(DataSourceBase::shared_ptr(result));
            }

            R getResult() {
                return result->get();
            }
        };

        template<>
        struct DataSourceResultStorage<void>
        {
            DataSourceResultStorage()
            {
            }

            template<class ContainerT>
            void initRet(ContainerT& ) {}

            void getResult() {}
        };

        //! @bug return of references are not supported yet in RemoteMethod.
        //! The user receives a reference to a local variable.
        template<class R>
        struct DataSourceResultStorage<R&>
        {
            typename ValueDataSource<R>::shared_ptr result;

            DataSourceResultStorage()
                : result(new ValueDataSource<R>() )
            {
            }

            template<class ContainerT>
            void initRet(ContainerT& cc) {
                cc.ret(DataSourceBase::shared_ptr(result));
            }

            R getResult() {
                return result->get();
            }
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

        //! @bug reference arguments are not supported yet in RemoteMethod and RemoteCommand.
        //! The result will not be communicated back to user code.
        template<class A>
        struct DataSourceArgStorage<A&>
        {
            // strips the reference !
            typename ValueDataSource<A>::shared_ptr value;
            DataSourceArgStorage()
                : value(new ValueDataSource<A>() )
            {}
        };

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
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            DataSourceArgStorage<arg1_type> ma1;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( DataSourceBase::shared_ptr(ma1.value.get()) );
            }

            void store(arg1_type a1) {
                ma1.value->set(a1);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<2, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( DataSourceBase::shared_ptr(ma2.value) );
            }
            void store(arg1_type a1, arg2_type a2) {
                ma1.value->set(a1);
                ma2.value->set(a2);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<3, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( DataSourceBase::shared_ptr(ma3.value) );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3) {
                ma1.value->set(a1);
                ma2.value->set(a2);
                ma3.value->set(a3);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<4, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
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
                cc.arg( DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( DataSourceBase::shared_ptr(ma3.value) );
                cc.arg( DataSourceBase::shared_ptr(ma4.value) );
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
