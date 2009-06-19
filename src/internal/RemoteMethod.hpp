/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  RemoteMethod.hpp

                        RemoteMethod.hpp -  description
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


#ifndef ORO_REMOTE_METHOD_HPP
#define ORO_REMOTE_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "MethodBase.hpp"
#include "MethodC.hpp"
#include "DataSourceStorage.hpp"
#include "Invoker.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A Method which executes a remote function directly.
         * @param MethodT The function signature of the method. For example,
         * bool( int, Frame, double)
         *
         */
        template<class MethodT>
        class RemoteMethodImpl
            : public MethodBase<MethodT>,
              protected detail::DataSourceStorage<MethodT>
        {
        protected:
            MethodC mmeth;
        public:
            typedef typename boost::function_traits<MethodT>::result_type result_type;

            /**
             * The defaults are reset by the constructor.
             */
            RemoteMethodImpl()
                : mmeth()
            {}

            /**
             * Call this operator if the RemoteMethod takes no arguments.
             *
             * @return true if ready and succesfully sent.
             */
            result_type invoke() {
                mmeth.execute();
                return this->detail::DataSourceStorage<MethodT>::getResult();
            }

            template<class T1>
            result_type invoke( T1 a1 ) {
                this->store( a1 );
                mmeth.execute();
                return this->detail::DataSourceStorage<MethodT>::getResult();
            }

            template<class T1, class T2>
            result_type invoke( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                mmeth.execute();
                return this->detail::DataSourceStorage<MethodT>::getResult();
            }

            template<class T1, class T2, class T3>
            result_type invoke( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                mmeth.execute();
                return this->detail::DataSourceStorage<MethodT>::getResult();
            }

            template<class T1, class T2, class T3, class T4>
            result_type invoke( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                mmeth.execute();
                return this->detail::DataSourceStorage<MethodT>::getResult();
            }
        };


        /**
         * A Method which is dispatched remotely to a MethodProcessor.
         * @param MethodT The function signature of the method. For example,
         * bool( int, Frame, double)
         *
         */
        template<class MethodT>
        class RemoteMethod
            : public Invoker<MethodT,RemoteMethodImpl<MethodT> >
        {
        public:
            typedef MethodT Signature;

            /**
             * Create a RemoteMethod object which executes a remote method
             *
             * @param name The name of this method.
             * @param com The OperationFactory for methods.
             */
            RemoteMethod(MethodFactory* of, std::string name)
            {
                // create the method.
                this->mmeth = MethodC(of, name);
                // add the arguments to the method.
                this->initArgs( this->mmeth );
                this->initRet(  this->mmeth );
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                return this->mmeth.ready();
            }

            virtual bool execute() {
                return this->mmeth.execute();
            }

            /**
             * Creates a clone of this RemoteMethod object.
             * Use this method to get a new method object
             * which has its own state information.
             *
             * @return
             */
            virtual ActionInterface* clone() const {
                return new RemoteMethod(*this);
            }

            virtual MethodBase<MethodT>* cloneI() const {
                return new RemoteMethod(*this);
            }
        };
    }
}
#endif
