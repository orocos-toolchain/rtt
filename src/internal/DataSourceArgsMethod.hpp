/***************************************************************************
  tag: FMTC  do nov 2 13:06:04 CET 2006  DataSourceArgsMethod.hpp

                        DataSourceArgsMethod.hpp -  description
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


#ifndef ORO_DATASOURCE_ARGS_METHOD_HPP
#define ORO_DATASOURCE_ARGS_METHOD_HPP

#include "DataSource.hpp"
#include "FunctorDataSource.hpp"
#include <boost/function.hpp>

namespace RTT
{
    namespace internal
    {
        /**
         * A method which gets its arguments from a data source and
         * is a datasource itself.
         */
        template<class SignatureT, class FunctorT = FunctorDataSource<boost::function<SignatureT> > >
        class DataSourceArgsMethod
            : public DataSource< typename boost::function_traits<SignatureT>::result_type >
        {
            typename FunctorT::shared_ptr mmeth;
        public:
            typedef boost::function_traits<SignatureT> traits;
            typedef SignatureT Signature;

            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef DataSource<result_type> Base;

            DataSourceArgsMethod(boost::function<Signature> meth)
                : mmeth( new FunctorT(meth) )
            {
            }

            DataSourceArgsMethod( typename FunctorT::shared_ptr ds)
                : mmeth( ds )
            {
            }

            DataSourceArgsMethod<Signature,FunctorT>* create() const
            {
                return clone();
            }

            template<class Arg1T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1);
                return r;
            }

            template<class Arg1T, class Arg2T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4, DataSource<Arg5T>* a5) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4, a5);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T, class Arg6T>
            DataSourceArgsMethod<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4, DataSource<Arg5T>* a5, DataSource<Arg6T>* a6) const
            {
                DataSourceArgsMethod<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4, a5, a6);
                return r;
            }


            result_type operator()() {
                return mmeth->get();
            }

            virtual result_type get() const {
                return mmeth->get();
            }

            virtual result_type value() const {
                return mmeth->value();
            }

            virtual DataSourceArgsMethod<Signature,FunctorT>* clone() const {
                return new DataSourceArgsMethod( typename FunctorT::shared_ptr(mmeth->clone()) );
            }

            virtual DataSource<result_type>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return new DataSourceArgsMethod<Signature,FunctorT>( typename FunctorT::shared_ptr(mmeth->copy(alreadyCloned)) );
            }

            boost::function<Signature> getMethodFunction() const {
                return mmeth.ff.gen;
            }
        };
    }
}


#endif
