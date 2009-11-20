/***************************************************************************
  tag: FMTC  do nov 2 13:06:11 CET 2006  CommandFunctors.hpp

                        CommandFunctors.hpp -  description
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


#ifndef ORO_COMMANDFUNCTORS_HPP
#define ORO_COMMANDFUNCTORS_HPP

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/type_traits/function_traits.hpp>

#include "../base/ActionInterface.hpp"
#include "../base/ConditionInterface.hpp"
#include "DataSource.hpp"
#include "DataSource.hpp"

namespace RTT
{



    namespace internal {

        template<int, class F>
        class FunctorImpl;

        /**
         * This trivial version (no arguments to manage) is
         * not used by CommandFunctor and ConditionFunctor.
         */
        template<typename FunctionT>
        class FunctorImpl<0, FunctionT>
        {
        public:
            FunctionT fun;

            FunctorImpl( FunctionT f)
                : fun( f )
            {
            }

            void setArguments( base::DataSourceBase* = 0, base::DataSourceBase* = 0, base::DataSourceBase* = 0, base::DataSourceBase* = 0  )
            {
            }

            void readArguments()
            {
            }

            bool execute()
            {
                return fun();
            }

            bool evaluate()
            {
                return fun();
            }

            FunctorImpl<0,FunctionT> copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorImpl( fun );
            }
        };

        /**
         * A functor that stores a function needing one argument,
         * and one DataSource to get the data from.
         */
        template<typename FunctionT>
        class FunctorImpl<1, FunctionT>
        {
        public:
            typedef typename FunctionT::arg1_type Arg1T;
            FunctionT fun;
            typename DataSource<Arg1T>::shared_ptr aa;

            FunctorImpl( FunctionT f, DataSource<Arg1T>* a = 0 )
                : fun( f ), aa(a)
            {
            }

            void setArguments( DataSource<Arg1T>* a, base::DataSourceBase* = 0, base::DataSourceBase* = 0, base::DataSourceBase* = 0  )
            {
                aa = a;
            }

            void readArguments()
            {
                aa->evaluate();
            }

            bool execute()
            {
                Arg1T a = aa->value();
                return fun( a );
            }

            bool evaluate()
            {
                Arg1T a = aa->value();
                bool r = fun( a );
                aa->updated();
                return r;
            }

            FunctorImpl<1,FunctionT> copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorImpl( fun, aa->copy( alreadyCloned ) );
            }
        };

        template<typename FunctionT>
        class FunctorImpl<2, FunctionT>
        {
        public:
            typedef typename FunctionT::arg1_type Arg1T;
            typedef typename FunctionT::arg2_type Arg2T;
            FunctionT fun;
            typename DataSource<Arg1T>::shared_ptr aa;
            typename DataSource<Arg2T>::shared_ptr bb;

            FunctorImpl( FunctionT f, DataSource<Arg1T>* a = 0, DataSource<Arg2T>* b = 0  )
                : fun( f ), aa(a), bb(b)
            {
            }

            void setArguments( DataSource<Arg1T>* a, DataSource<Arg2T>* b, base::DataSourceBase* = 0, base::DataSourceBase* = 0  )
            {
                aa = a;
                bb = b;
            }

            void readArguments()
            {
                aa->evaluate();
                bb->evaluate();
            }

            bool execute()
            {
                Arg1T a = aa->value();
                Arg2T b = bb->value();
                return fun( a, b );
            }

            bool evaluate()
            {
                Arg1T a = aa->value();
                Arg2T b = bb->value();
                bool r = fun( a, b);
                aa->updated();
                bb->updated();
                return r;
            }

            FunctorImpl<2,FunctionT> copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorImpl( fun, aa->copy( alreadyCloned ), bb->copy(alreadyCloned));
            }
        };


        template<typename FunctionT>
        class FunctorImpl<3, FunctionT>
        {
        public:
            typedef typename FunctionT::arg1_type Arg1T;
            typedef typename FunctionT::arg2_type Arg2T;
            typedef typename FunctionT::arg3_type Arg3T;
            FunctionT fun;
            typename DataSource<Arg1T>::shared_ptr aa;
            typename DataSource<Arg2T>::shared_ptr bb;
            typename DataSource<Arg3T>::shared_ptr cc;

            FunctorImpl( FunctionT f, DataSource<Arg1T>* a = 0, DataSource<Arg2T>* b = 0, DataSource<Arg3T>* c = 0)
                : fun( f ), aa(a), bb(b), cc(c)
            {
            }

            void setArguments( DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c, base::DataSourceBase* = 0  )
            {
                aa = a;
                bb = b;
                cc = c;
            }

            void readArguments()
            {
                aa->evaluate();
                bb->evaluate();
                cc->evaluate();
            }

            bool execute()
            {
                Arg1T a = aa->value();
                Arg2T b = bb->value();
                Arg3T c = cc->value();
                return fun( a, b, c);
            }

            bool evaluate()
            {
                Arg1T a = aa->value();
                Arg2T b = bb->value();
                Arg3T c = cc->value();
                bool r = fun( a, b, c);
                aa->updated();
                bb->updated();
                cc->updated();
                return r;
            }

            FunctorImpl<3,FunctionT> copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorImpl( fun, aa->copy( alreadyCloned ), bb->copy(alreadyCloned), cc->copy(alreadyCloned) );
            }
        };


        template<typename FunctionT>
        class FunctorImpl<4, FunctionT>
        {
        public:
            typedef typename FunctionT::arg1_type Arg1T;
            typedef typename FunctionT::arg2_type Arg2T;
            typedef typename FunctionT::arg3_type Arg3T;
            typedef typename FunctionT::arg4_type Arg4T;
            FunctionT fun;
            typename DataSource<Arg1T>::shared_ptr aa;
            typename DataSource<Arg2T>::shared_ptr bb;
            typename DataSource<Arg3T>::shared_ptr cc;
            typename DataSource<Arg4T>::shared_ptr dd;

            FunctorImpl( FunctionT f, DataSource<Arg1T>* a = 0, DataSource<Arg2T>* b = 0, DataSource<Arg3T>* c = 0, DataSource<Arg4T>* d = 0  )
                : fun( f ), aa(a), bb(b), cc(c), dd(d)
            {
            }

            void setArguments( DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c, DataSource<Arg4T>* d  )
            {
                aa = a;
                bb = b;
                cc = c;
                dd = d;
            }

            void readArguments()
            {
                aa->evaluate();
                bb->evaluate();
                cc->evaluate();
                dd->evaluate();
            }

            bool execute()
            {
                Arg1T a = aa->value();
                Arg2T b = bb->value();
                Arg3T c = cc->value();
                Arg4T d = dd->value();
                return fun( a, b, c, d );
            }

            bool evaluate()
            {
                Arg1T a = aa->value();
                Arg2T b = bb->value();
                Arg3T c = cc->value();
                Arg4T d = dd->value();
                bool r = fun( a, b, c, d );
                aa->updated();
                bb->updated();
                cc->updated();
                dd->updated();
                return r;
            }

            FunctorImpl<4,FunctionT> copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return FunctorImpl( fun, aa->copy( alreadyCloned ), bb->copy(alreadyCloned), cc->copy(alreadyCloned), dd->copy(alreadyCloned) );
            }
        };

        /**
         * A functor that stores a function,
         * and DataSources to get the data from. It can be used
         * to store Commands or Conditions. It is up to the user
         * to call the meaningful functions of the interface according
         * to the stored function type.
         * @param FunctionT a boost::function<T> or similar.
         */
        template< class FunctionT>
        struct Functor
            : public FunctorImpl<boost::function_traits<FunctionT>::arity,boost::function<FunctionT> >
        {
            typedef FunctionT Function;
            typedef boost::function<FunctionT> FunctionImpl;

            Functor<FunctionT>(FunctionImpl impl)
                : FunctorImpl<boost::function_traits<FunctionT>::arity,FunctionImpl>(impl)
            {}

            // Allow construction from base class.
            Functor<FunctionT>(FunctorImpl<boost::function_traits<FunctionT>::arity,FunctionImpl> impl)
                : FunctorImpl<boost::function_traits<FunctionT>::arity,FunctionImpl>(impl)
            {}
        };


        /**
         * A functor with the ActionInterface.
         * @param Signature The function signature of this command.
         * @param FunctorT The function that stores the command.
         */
        template<typename SignatureT, typename FunctorT = Functor<SignatureT> >
        class CommandFunctor
            :public base::ActionInterface
        {
        public:
            typedef typename FunctorT::FunctionImpl Function;
            typedef SignatureT Signature;
            typedef FunctorT Functor;

            FunctorT com;

            /**
             *
             */
            CommandFunctor(Function impl)
                : com(impl)
            {
            }

            CommandFunctor(FunctorT impl)
                : com(impl)
            {
            }

            virtual void readArguments() { com.readArguments(); }

            virtual bool execute() { return com.execute(); }

            virtual CommandFunctor<Signature,FunctorT>* clone() const
            {
                return new CommandFunctor( com );
            }

            virtual CommandFunctor<Signature,FunctorT>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return new CommandFunctor( FunctorT(com.copy(alreadyCloned)) );
            }
        };

        /**
         * A functor with the base::ActionInterface, for the case where
         * the functor is a bool(void).
         */
        class CommandFunction
            :public base::ActionInterface
        {
        public:
            typedef boost::function<bool(void)> Function;

            Function com;

            /**
             *
             */
            CommandFunction(Function impl)
                : com(impl)
            {
            }

            virtual void readArguments() { }

            virtual bool execute() { return com(); }

            virtual CommandFunction* clone() const
            {
                return new CommandFunction( com );
            }
        };


        /**
         * A functor with the ConditionInterface.
         * @param Signature The function signature of this condition
         * @param FunctorT The function that stores the command.
         */
        template<typename SignatureT, typename FunctorT = Functor<SignatureT> >
        class ConditionFunctor
            :public base::ConditionInterface
        {
        public:
            typedef typename FunctorT::FunctionImpl Function;
            typedef SignatureT Signature;
            typedef FunctorT Functor;

            FunctorT con;
            bool minvert;

            /**
             *
             */
            ConditionFunctor(Function impl, bool invert=false)
                : con(impl), minvert(invert)
            {
            }

            ConditionFunctor(FunctorT impl, bool invert=false)
                : con(impl), minvert(invert)
            {
            }

            virtual bool evaluate() { return con.evaluate() != minvert; }

            virtual ConditionFunctor<Signature,FunctorT>* clone() const
            {
                return new ConditionFunctor( con, minvert );
            }

            virtual ConditionFunctor<Signature,FunctorT>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
            {
                return new ConditionFunctor( FunctorT(con.copy(alreadyCloned)), minvert );
            }
        };

        /**
         * A functor with the base::ConditionInterface, for the case where
         * the functor is a bool(void).
         */
        class ConditionFunction
            :public base::ConditionInterface
        {
        public:
            typedef boost::function<bool(void)> Function;

            Function con;
            bool minvert;

            /**
             *
             */
            ConditionFunction(Function impl, bool invert=false)
                : con(impl), minvert(invert)
            {
            }

            virtual bool evaluate() { return con() != minvert; }

            virtual ConditionFunction* clone() const
            {
                return new ConditionFunction( con, minvert );
            }
        };
    }
}

#endif
