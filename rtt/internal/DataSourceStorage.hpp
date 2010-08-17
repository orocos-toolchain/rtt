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
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_arity.hpp>
#include "DataSources.hpp"
#include "BindStorage.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * Analogous to RStore, but specific for DataSourceStorage.
         * Lacks the executed check and exec function.
         * It provides storage for a return value, that will be set
         * using a ReferenceDataSource and be read in collect/collectIfDone.
         */
        template<class T>
        struct DSRStore {
            T arg;
            DSRStore() : arg() {}

            T& result() { return arg; }
            operator T&() { return arg; }
        };

        template<class T>
        struct DSRStore<T&>
        {
            typedef typename boost::remove_const<T>::type result_type;
            result_type arg;
            DSRStore() : arg() {}
            result_type& result() { return arg; } // non const return
            operator result_type&() { return arg; }
        };

        template<>
        struct DSRStore<void> {
            DSRStore() {}
            void result() { return; }
        };

        template<>
        struct is_arg_return<DSRStore<void> > : public mpl::false_
        {};

        template<class T>
        struct is_arg_return<DSRStore<T> > : public mpl::true_
        {};


        //! Partial specialisations for storing a void, not a void or reference
        //! Wraps around RStore.
        template<class R>
        struct DataSourceResultStorage
        {
            typedef typename remove_cr<R>::type ds_type;
            DSRStore<R> retn;
            typename ReferenceDataSource<ds_type>::shared_ptr result;

            DataSourceResultStorage()
                : result( new ReferenceDataSource<ds_type>(retn.result()) )
            {
            }

            template<class ContainerT>
            void initRet(ContainerT& cc) {
                cc.ret(base::DataSourceBase::shared_ptr(result));
            }

            R getResult() {
                return retn.result();
            }
        };

        template<>
        struct DataSourceResultStorage<void>
        {
            typedef void result_type;
            DSRStore<void> retn;
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
        struct DataSourceResultStorage<R const&>
        {
            typedef R const& result_type;
            typedef R ds_type;
            DSRStore<result_type> retn;
            typename ReferenceDataSource<ds_type>::shared_ptr result;

            DataSourceResultStorage()
                : result( new ReferenceDataSource<ds_type>( retn.result() ) )
            {
            }

            template<class ContainerT>
            void initRet(ContainerT& cc) {
                cc.ret(base::DataSourceBase::shared_ptr(result));
            }

            result_type getResult() {
                return result->rvalue();
            }
        };

        //! Partial specialisations for storing a reference or not reference
        //! Wraps around AStore
        template<class A>
        struct DataSourceArgStorage
        {
            AStore<A> arg;
            typedef typename remove_cr<A>::type ds_type;
            typename LateReferenceDataSource<ds_type>::shared_ptr value;
            DataSourceArgStorage()
                : value( new LateReferenceDataSource<ds_type>() )
            {}
            void newarg(A a) { arg(a); value->setPointer(&arg.get()); }
        };

        template<class A>
        struct DataSourceArgStorage<A const&>
        {
            AStore<A const&> arg;
            // without const&:
            typename LateConstReferenceDataSource<A>::shared_ptr value;
            DataSourceArgStorage()
                : value( new LateConstReferenceDataSource<A>() )
            {}
            void newarg(A const& a) { arg(a); value->setPointer(&arg.get());}
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
            typedef typename boost::function_traits<DataType>::result_type result_type;
            bf::vector< DSRStore<result_type>& > vStore;
            DataSourceStorageImpl() :  vStore(boost::ref(this->retn)) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn) {}
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
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            DataSourceArgStorage<arg1_type> ma1;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore<arg1_type>& > vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value.get()) );
            }

            void store(arg1_type a1) {
                ma1.newarg(a1);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<2, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>& > vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg,ma2.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg,ma2.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma2.value) );
            }
            void store(arg1_type a1, arg2_type a2) {
                ma1.newarg(a1);
                ma2.newarg(a2);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<3, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>& > vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma3.value) );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3) {
                ma1.newarg(a1);
                ma2.newarg(a2);
                ma3.newarg(a3);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<4, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            typedef typename boost::function_traits<DataType>::arg4_type   arg4_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;
            DataSourceArgStorage<arg4_type> ma4;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>&, AStore<arg4_type>& > vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma3.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma4.value) );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) {
                ma1.newarg(a1);
                ma2.newarg(a2);
                ma3.newarg(a3);
                ma4.newarg(a4);
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
