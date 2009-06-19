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
#include "MethodBase.hpp"
#include "BindStorage.hpp"

namespace RTT
{
    namespace detail
    {
        template<class FunctionT>
        class LocalMethodImpl
            : public MethodBase<FunctionT>
        {
        protected:
            boost::function<FunctionT> mmeth;
        public:
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef boost::function_traits<Signature> traits;

            void readArguments() {}

            bool execute() {
                return false;
            }

            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type invoke()
            {
                return mmeth();
            }

            /**
             * Invoke this operator if the method has one argument.
             */
            template<class T1>
            result_type invoke(T1 t)
            {
                return mmeth(t);
            }

            /**
             * Invoke this operator if the method has two arguments.
             */
            template<class T1, class T2>
            result_type invoke(T1 t1, T2 t2)
            {
                return mmeth(t1, t2);
            }

            /**
             * Invoke this operator if the method has three arguments.
             */
            template<class T1, class T2, class T3>
            result_type invoke(T1 t1, T2 t2, T3 t3)
            {
                return mmeth(t1, t2, t3);
            }

            /**
             * Invoke this operator if the method has four arguments.
             */
            template<class T1, class T2, class T3, class T4>
            result_type invoke(T1 t1, T2 t2, T3 t3, T4 t4)
            {
                return mmeth(t1, t2, t3, t4);
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
            LocalMethod(M meth, ObjectType object)
            {
                this->mmeth = detail::MethodBinder<Signature>()(meth, object);
            }

            /**
             * Construct a LocalMethod from a function pointer or function object.
             *
             * @param name the name of this method
             * @param meth an pointer to a function or function object.
             */
            template<class M>
            LocalMethod(M meth)
            {
                this->mmeth = meth;
            }

            boost::function<Signature> getMethodFunction() const
            {
                return this->mmeth;
            }

            ActionInterface* clone() const
            {
                return new LocalMethod<Signature>(*this);
            }

            MethodBase<Signature>* cloneI() const
            {
                return new LocalMethod<Signature>(*this);
            }
        };
    }
}

#endif
