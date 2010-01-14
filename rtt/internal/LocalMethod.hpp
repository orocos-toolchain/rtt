/***************************************************************************
  tag: FMTC  do nov 2 13:06:05 CET 2006  LocalMethod.hpp

                        LocalMethod.hpp -  description
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


#ifndef ORO_LOCAL_METHOD_HPP
#define ORO_LOCAL_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "Invoker.hpp"
#include "../base/MethodBase.hpp"
#include "BindStorage.hpp"
#include "../SendStatus.hpp"
#include "../SendHandle.hpp"
#include "../ExecutionEngine.hpp"
#include "MethodBinder.hpp"
#include <boost/fusion/include/vector_tie.hpp>

#include <iostream>
// For doing I/O
#include <boost/fusion/sequence/io.hpp>
using namespace std;

namespace RTT
{
    namespace internal
    {
        /**
         * Implements call, send, collect, collectIfDone for all function arities.
         * call is implemented using boost::function, the others are functions
         * of this class.
         *
         * You can pass arguments by (const) value and by (const) reference. Passing
         * pointers is 'accidentally' supported in-process, it will not work with out-of-process
         * calls. The send takes all arguments (C++ doesn't know pure out args), the collect()
         * only takes the return value and the 'reference' arguments (if any)
         *
         * If this object is disposed, the collection of results is no longer possible
         * and this will generate a SendFailure at caller side.
         */
        template<class FunctionT>
        class LocalMethodImpl
            : public base::MethodBase<FunctionT>,
              protected BindStorage<FunctionT>
        {
        protected:
            ExecutionEngine* myengine;
            ExecutionEngine* caller;
            typedef BindStorage<FunctionT> Store;

        public:
            LocalMethodImpl() : myengine(0), caller(0) {}
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef typename boost::function_traits<Signature>::result_type result_reference;
            typedef boost::function_traits<Signature> traits;

            void executeAndDispose() {
                using namespace std;
                if (!this->retn.isExecuted()) {
                    this->exec(); // calls BindStorage.
                    //cout << "executed method"<<endl;
                    if (caller){
                        caller->process(this);
                    }
                } else {
                    //cout << "received method done msg."<<endl;
                    // Already executed, are in caller.
                    // nop, we will check ret in collect()
                    // This is the place to call call-back functions,
                    // since we're in the caller's (or proxy's) EE.
                }
                return;
            }

            void dispose() {
            }

            ExecutionEngine* getMessageProcessor() const { return myengine; }

            // We need a handle object !
            SendHandle<Signature> send_impl() {
                assert(myengine);
                base::MethodBase<Signature>* cl = this->cloneI();
                //std::cout << "Sending clone..."<<std::endl;
                if ( myengine->process( cl ) )
                    return SendHandle<Signature>( cl );
                else {
                    delete cl;
                    return SendHandle<Signature>();
                }
            }

            template<class T1>
            SendHandle<Signature> send_impl( T1 a1 ) {
                assert(myengine);
                // bind types from Storage<Function>
                this->store( a1 );
                return send_impl();
            }

            template<class T1, class T2>
            SendHandle<Signature> send_impl( T1 a1, T2 a2 ) {
                // bind types from Storage<Function>
                this->store( a1, a2 );
                return send_impl();
                //return (myengine->process( this ) != 0) ? SendSuccess : SendFailure;
            }

            template<class T1, class T2, class T3>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3 ) {
                // bind types from Storage<Function>
                this->store( a1, a2, a3 );
                return send_impl();
                //return (myengine->process( this ) != 0) ? SendSuccess : SendFailure;
            }

            template<class T1, class T2, class T3, class T4>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                // bind types from Storage<Function>
                this->store( a1, a2, a3, a4 );
                return send_impl();
                //return (myengine->process( this ) != 0) ? SendSuccess : SendFailure;
            }

            SendStatus collectIfDone_impl() {
                if ( this->retn.isExecuted()) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            // collect_impl belongs in LocalMethodImpl because it would need
            // to be repeated in each BindStorage spec.
            template<class T1>
            SendStatus collectIfDone_impl( T1& a1 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2>
            SendStatus collectIfDone_impl( T1& a1, T2& a2 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1,a2) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                }
                return SendNotReady;
            }

            template<class T1, class T2, class T3>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1,a2,a3) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1,a2,a3,a4) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            SendStatus collect_impl() {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl();
            }
            template<class T1>
            SendStatus collect_impl( T1& a1 ) {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl(a1);
            }

            template<class T1, class T2>
            SendStatus collect_impl( T1& a1, T2& a2 ) {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl(a1,a2);
            }

            template<class T1, class T2, class T3>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3 ) {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl(a1,a2,a3);
            }

            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type call_impl()
            {
                // if component or 3rd party thread, use that one, otherwise,
                // just execute the method. NOTE: we will use a different class for each case.
                if ( myengine ) {
                    SendHandle<Signature> h = send_impl();
                    if ( h.collect() == SendSuccess )
                        return h.ret();
                    else
                        throw SendFailure;
                } else
                    return this->mmeth();
            }


            /**
             * Invoke this operator if the method has one argument.
             */
            template<class T1>
            result_type call_impl(T1 a1)
            {
                SendHandle<Signature> h;
                if ( myengine ) {
                    h = send_impl(a1);
                    // collect_impl may take diff number of arguments than
                    // call_impl/ret_impl(), so we use generic collect() + ret_impl()
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1);
                    else
                        throw SendFailure;
                } else
                    return this->mmeth(a1);
            }

            template<class T1, class T2>
            result_type call_impl(T1 a1, T2 a2)
            {
                SendHandle<Signature> h;
                if ( myengine ) {
                    h = send_impl(a1,a2);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2);
                    else
                        throw SendFailure;
                } else
                    return this->mmeth(a1,a2);
            }

            template<class T1, class T2, class T3>
            result_type call_impl(T1 a1, T2 a2, T3 a3)
            {
                SendHandle<Signature> h;
                if ( myengine ) {
                    h = send_impl(a1,a2,a3);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3);
                    else
                        throw SendFailure;
                } else
                    return this->mmeth(a1,a2,a3);
            }

            template<class T1, class T2, class T3, class T4>
            result_type call_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                SendHandle<Signature> h;
                if ( myengine ) {
                    h = send_impl(a1,a2,a3,a4);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3,a4);
                    else
                        throw SendFailure;
                } else
                    return this->mmeth(a1,a2,a3,a4);
            }

            result_type ret_impl()
            {
                return this->retn.result(); // may return void.
            }

            /**
             * This function has the same signature of call() and
             * returns the stored return value, and tries to return
             * all arguments.
             */
            template<class T1>
            result_type ret_impl(T1 a1)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1> vArgs( boost::ref(a1) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

            template<class T1,class T2>
            result_type ret_impl(T1 a1, T2 a2)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2> vArgs( boost::ref(a1), boost::ref(a2) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg< boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

            template<class T1,class T2, class T3>
            result_type ret_impl(T1 a1, T2 a2, T3 a3)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

            template<class T1,class T2, class T3, class T4>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

        };

        /**
         * A method which executes a local function.
         *
         * Usage:
         @code
         LocalMethod<double(int, double)> mymeth( &Class::foo, &c);
         double result = mymeth( 3, 1.9);
         @endcode
        */
        template<class FunctionT>
        struct LocalMethod
            : public Invoker<FunctionT,LocalMethodImpl<FunctionT> >
        {
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef boost::function_traits<Signature> traits;

            /**
             * Create an empty LocalMethod object.
             * Use assignment to initialise it.
             * @see method
             */
            LocalMethod()
            {}

            /**
             * Construct a LocalMethod from a class member pointer and an
             * object of that class.
             *
             * @param name The name of this method
             * @param meth A pointer to a class member function
             * @param object An object of the class which has \a meth as member function.
             */
            template<class M, class ObjectType>
            LocalMethod(M meth, ObjectType object, ExecutionEngine* ee, ExecutionEngine* caller)
            {
                this->mmeth = MethodBinder<Signature>()(meth, object);
                this->myengine = ee;
                this->caller = caller;
            }

            /**
             * Construct a LocalMethod from a function pointer or function object.
             *
             * @param name the name of this method
             * @param meth an pointer to a function or function object.
             */
            template<class M>
            LocalMethod(M meth, ExecutionEngine* ee, ExecutionEngine* caller)
            {
                this->mmeth = meth;
                this->myengine = ee;
                this->caller = caller;
            }

            boost::function<Signature> getMethodFunction() const
            {
                return this->mmeth;
            }

            base::ActionInterface* clone() const
            {
                return new LocalMethod<Signature>(*this);
            }

            base::MethodBase<Signature>* cloneI() const
            {
                return new LocalMethod<Signature>(*this);
            }
        };
    }
}

#endif
