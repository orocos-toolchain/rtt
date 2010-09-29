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

        //! @bug return of references are not supported yet in RemoteOperationCaller.
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

        //! Partial specialisations for storing a (const) reference or not reference
        //! The trick here is to take a (const) reference data source to something that was
        //! already a reference, and copy to a value data source for something that
        //! is plain value. As such, we keep the same 'lifetime' expectations as
        //! with standard C++ method calls.
        //! Wraps around AStore
        template<class A>
        struct DataSourceArgStorage
        {
            typedef typename remove_cr<A>::type ds_type;
            typedef AStore<A&> Store;
            typename ValueDataSource<ds_type>::shared_ptr value;
            AStore<A&> arg;
            DataSourceArgStorage()
                : value( new ValueDataSource<ds_type>() )
            {}
            // We store the copy of 'a' in the data source, such that
            // that copy is always valid memory (refcounted). If we
            // would store it in 'arg' and use a LateReferenceDataSource,
            // we would loose/corrupt the data if this object is destroyed.
            // This is acceptable for ref/constref cases, but not for
            // value cases, which are often living on the stack and by
            // definition short lived.
            void newarg(A a) { arg( value->set() ); value->set(a); }
        };

        //! Partial specialisations for storing a reference.
        template<class A>
        struct DataSourceArgStorage<A &>
        {
            typedef AStore<A&> Store;
            AStore<A&> arg;
            typedef typename remove_cr<A>::type ds_type;
            typename LateReferenceDataSource<ds_type>::shared_ptr value;
            DataSourceArgStorage()
                : value( new LateReferenceDataSource<ds_type>() )
            {}
            void newarg(A& a) { arg(a); value->setPointer(&arg.get()); }
        };

        //! Partial specialisations for storing a const reference.
        template<class A>
        struct DataSourceArgStorage<A const&>
        {
            typedef AStore<A const&> Store;
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
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;

            // the list of all our storage. We need this vector for returning the
            // results to collect(). We use boost::fusion magic to copy selected parts
            // from R/AStore back to the args of collect(...).
            bf::vector< DSRStore<result_type>&, AStore1& > vStore;
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
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;
            typedef typename DataSourceArgStorage<arg2_type>::Store AStore2;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore1&, AStore2& > vStore;
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
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;
            typedef typename DataSourceArgStorage<arg2_type>::Store AStore2;
            typedef typename DataSourceArgStorage<arg3_type>::Store AStore3;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore1&, AStore2&, AStore3& > vStore;
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
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;
            typedef typename DataSourceArgStorage<arg2_type>::Store AStore2;
            typedef typename DataSourceArgStorage<arg3_type>::Store AStore3;
            typedef typename DataSourceArgStorage<arg4_type>::Store AStore4;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore1&, AStore2&, AStore3&, AStore4& > vStore;
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

        template<class DataType>
        struct DataSourceStorageImpl<5, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            typedef typename boost::function_traits<DataType>::arg4_type   arg4_type;
            typedef typename boost::function_traits<DataType>::arg5_type   arg5_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;
            DataSourceArgStorage<arg4_type> ma4;
            DataSourceArgStorage<arg5_type> ma5;
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;
            typedef typename DataSourceArgStorage<arg2_type>::Store AStore2;
            typedef typename DataSourceArgStorage<arg3_type>::Store AStore3;
            typedef typename DataSourceArgStorage<arg4_type>::Store AStore4;
            typedef typename DataSourceArgStorage<arg5_type>::Store AStore5;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore1&, AStore2&, AStore3&, AStore4& , AStore5& > vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg,ma5.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg,ma5.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma3.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma4.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma5.value) );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5) {
                ma1.newarg(a1);
                ma2.newarg(a2);
                ma3.newarg(a3);
                ma4.newarg(a4);
                ma5.newarg(a5);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<6, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            typedef typename boost::function_traits<DataType>::arg4_type   arg4_type;
            typedef typename boost::function_traits<DataType>::arg5_type   arg5_type;
            typedef typename boost::function_traits<DataType>::arg6_type   arg6_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;
            DataSourceArgStorage<arg4_type> ma4;
            DataSourceArgStorage<arg5_type> ma5;
            DataSourceArgStorage<arg6_type> ma6;
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;
            typedef typename DataSourceArgStorage<arg2_type>::Store AStore2;
            typedef typename DataSourceArgStorage<arg3_type>::Store AStore3;
            typedef typename DataSourceArgStorage<arg4_type>::Store AStore4;
            typedef typename DataSourceArgStorage<arg5_type>::Store AStore5;
            typedef typename DataSourceArgStorage<arg6_type>::Store AStore6;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore1&, AStore2&, AStore3&, AStore4& , AStore5& , AStore6&> vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg,ma5.arg,ma6.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg,ma5.arg,ma6.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma3.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma4.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma5.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma6.value) );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6) {
                ma1.newarg(a1);
                ma2.newarg(a2);
                ma3.newarg(a3);
                ma4.newarg(a4);
                ma5.newarg(a5);
                ma6.newarg(a6);
            }
        };

        template<class DataType>
        struct DataSourceStorageImpl<7, DataType>
            : public DataSourceResultStorage<typename boost::function_traits<DataType>::result_type>
        {
            typedef typename boost::function_traits<DataType>::result_type result_type;
            typedef typename boost::function_traits<DataType>::arg1_type   arg1_type;
            typedef typename boost::function_traits<DataType>::arg2_type   arg2_type;
            typedef typename boost::function_traits<DataType>::arg3_type   arg3_type;
            typedef typename boost::function_traits<DataType>::arg4_type   arg4_type;
            typedef typename boost::function_traits<DataType>::arg5_type   arg5_type;
            typedef typename boost::function_traits<DataType>::arg6_type   arg6_type;
            typedef typename boost::function_traits<DataType>::arg7_type   arg7_type;
            DataSourceArgStorage<arg1_type> ma1;
            DataSourceArgStorage<arg2_type> ma2;
            DataSourceArgStorage<arg3_type> ma3;
            DataSourceArgStorage<arg4_type> ma4;
            DataSourceArgStorage<arg5_type> ma5;
            DataSourceArgStorage<arg6_type> ma6;
            DataSourceArgStorage<arg7_type> ma7;
            typedef typename DataSourceArgStorage<arg1_type>::Store AStore1;
            typedef typename DataSourceArgStorage<arg2_type>::Store AStore2;
            typedef typename DataSourceArgStorage<arg3_type>::Store AStore3;
            typedef typename DataSourceArgStorage<arg4_type>::Store AStore4;
            typedef typename DataSourceArgStorage<arg5_type>::Store AStore5;
            typedef typename DataSourceArgStorage<arg6_type>::Store AStore6;
            typedef typename DataSourceArgStorage<arg7_type>::Store AStore7;

            // the list of all our storage.
            bf::vector< DSRStore<result_type>&, AStore1&, AStore2&, AStore3&, AStore4&, AStore5&, AStore6&, AStore7&> vStore;
            DataSourceStorageImpl() : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg,ma5.arg,ma6.arg,ma7.arg) {}
            DataSourceStorageImpl(const DataSourceStorageImpl& orig) : vStore(this->retn,ma1.arg,ma2.arg,ma3.arg,ma4.arg,ma5.arg,ma6.arg,ma7.arg) {}

            template<class ContainerT>
            void initArgs(ContainerT& cc) {
                cc.arg( base::DataSourceBase::shared_ptr(ma1.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma2.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma3.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma4.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma5.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma6.value) );
                cc.arg( base::DataSourceBase::shared_ptr(ma7.value) );
            }
            void store(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7) {
                ma1.newarg(a1);
                ma2.newarg(a2);
                ma3.newarg(a3);
                ma4.newarg(a4);
                ma5.newarg(a5);
                ma6.newarg(a6);
                ma7.newarg(a7);
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
