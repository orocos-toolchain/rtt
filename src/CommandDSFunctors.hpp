/***************************************************************************
  tag: FMTC  do nov 2 13:06:10 CET 2006  CommandDSFunctors.hpp

                        CommandDSFunctors.hpp -  description
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


#ifndef ORO_COMMAND_DS_FUNCTORS_HPP
#define ORO_COMMAND_DS_FUNCTORS_HPP

#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include "DataSource.hpp"

namespace RTT
{
    namespace detail {

        template<typename FunctionT>
        class FunctorDS0
        {
        public:
            typedef boost::function<FunctionT> FunctionImpl;
            typedef FunctionT Function;
            typedef typename boost::remove_pointer<typename FunctionImpl::arg1_type>::type CompT;
            FunctionImpl fun;
            typename DataSource<boost::weak_ptr<CompT> >::shared_ptr ds;
            typedef boost::weak_ptr<CompT> CompW;
            typedef boost::shared_ptr<CompT> CompS;

            FunctorDS0( DataSource<CompW>* c, FunctionImpl f )
                : fun( f ), ds(c)
            {}

            void setArguments( DataSourceBase* = 0, DataSourceBase* = 0, DataSourceBase* = 0, DataSourceBase* = 0  ) {}
            void readArguments(){}

            bool execute()
            {
                // the Component pointer is stored in a DataSource
                CompS c = ds->get().lock();
                if (c) {
                    CompT* ct = c.get();
                    return fun( ct );
                } else {
                    return false; // destroyed.
                }
            }

            bool evaluate()
            {
                // logical XOR :
                boost::shared_ptr<CompT> c = ds->get().lock();
                if (c){
                    CompT* ct = c.get();
                    return fun( ct );
                } else
                    return false;
            }

            FunctorDS0<FunctionT> copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorDS0( ds->copy(alreadyCloned), fun );
            }
        };

        /**
         * A functor that stores a function needing one argument,
         * and one DataSource to get the data from.
         */
        template<typename FunctionT>
        class FunctorDS1
        {
        public:
            typedef FunctionT Function;
            typedef boost::function<FunctionT> FunctionImpl;
            // arg1_type is the component type.
            typedef typename boost::remove_pointer<typename FunctionImpl::arg1_type>::type CompT;
            typedef typename FunctionImpl::arg2_type Arg2T;
            FunctionImpl fun;
            typename DataSource<Arg2T>::shared_ptr aa;
            typename DataSource<boost::weak_ptr<CompT> >::shared_ptr ds;

            FunctorDS1( DataSource<boost::weak_ptr<CompT> >* c, FunctionImpl f, DataSource<Arg2T>* a = 0)
                : fun( f ), aa( a ), ds(c)
            {
            }

            void setArguments( DataSource<Arg2T>* a, DataSourceBase* = 0, DataSourceBase* = 0, DataSourceBase* = 0  )
            {
                aa = a;
            }

            void readArguments()
            {
                aa->evaluate();
            }

            bool execute()
            {
                boost::shared_ptr<CompT> c =  ds->get().lock();
                if (c){
                    CompT* ct = c.get();
                    Arg2T a = aa->value();
                    bool r= fun( ct, a );
                    return r;
                } else
                    return false;
            }

            bool evaluate()
            {
                Arg2T a =  aa->value();
                boost::shared_ptr<CompT> c = ds->get().lock();
                // logical XOR :
                if (c) {
                    CompT* ct = c.get();
                    bool r= fun( ct, a );
                    aa->updated();
                    return r;
                } else
                    return false;
            }

            FunctorDS1<FunctionT> copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorDS1( ds->copy(alreadyCloned), fun, aa->copy( alreadyCloned ) );
            }
        };

    }
}

#endif
