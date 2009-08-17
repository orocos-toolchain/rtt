/***************************************************************************
  tag: FMTC  do nov 2 13:06:05 CET 2006  DataSourceArgsEvent.hpp

                        DataSourceArgsEvent.hpp -  description
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


#ifndef ORO_DATASOURCE_ARGS_EVENT_HPP
#define ORO_DATASOURCE_ARGS_EVENT_HPP

#include "DataSource.hpp"
#include "LocalEvent.hpp"
#include "FunctorDataSource.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * A event which gets its arguments from a data source and
         * is an action object.
         */
        template<class SignatureT, class FunctorT = FunctorDataSource<boost::function<SignatureT> > >
        class DataSourceArgsEvent
            : public base::ActionInterface
        {
            typename FunctorT::shared_ptr mmeth;
        public:
            typedef boost::function_traits<SignatureT> traits;
            typedef SignatureT Signature;

            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef base::ActionInterface Base;

            DataSourceArgsEvent(boost::function<Signature> meth)
                : mmeth( new FunctorT(meth) )
            {
            }

            DataSourceArgsEvent( typename FunctorT::shared_ptr ds)
                : mmeth( ds )
            {
            }

            DataSourceArgsEvent<Signature,FunctorT>* create() const
            {
                return clone();
            }

            template<class Arg1T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1);
                return r;
            }

            template<class Arg1T, class Arg2T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4, DataSource<Arg5T>* a5) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4, a5);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T, class Arg5T, class Arg6T>
            DataSourceArgsEvent<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4, DataSource<Arg5T>* a5, DataSource<Arg6T>* a6) const
            {
                DataSourceArgsEvent<Signature,FunctorT>* r =  this->clone();
                r->mmeth->setArguments(a1, a2, a3, a4, a5, a6);
                return r;
            }


            bool execute() {
                mmeth->get();
                return true;
            }

            void readArguments() {
                // the arguments are read in mmeth.get() above.
            }

            virtual DataSourceArgsEvent<Signature,FunctorT>* clone() const {
                return new DataSourceArgsEvent( typename FunctorT::shared_ptr(mmeth->clone()) );
            }

            virtual base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return new DataSourceArgsEvent<Signature,FunctorT>( typename FunctorT::shared_ptr(mmeth->copy(alreadyCloned)) );
            }

            boost::function<Signature> getEventFunction() const {
                return mmeth.ff.gen;
            }
        };
    }
}


#endif
