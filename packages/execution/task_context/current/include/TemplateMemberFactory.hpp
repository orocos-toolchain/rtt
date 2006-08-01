/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TemplateMemberFactory.hpp 

                        TemplateMemberFactory.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

#include "DataSource.hpp"
#include "TemplateFactory.hpp"

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>

#ifdef MEMBER

/**
 * @file This file contains the TemplateMemberFactory template,
 * which is a template designed to reduce the amount of boilerplate
 * code necessary to add a DataSourceFactory and MethodFactory to an OROCOS component.
 *
 * This is an internal file, not to be used directly by users.
 */
namespace RTT
{

#ifndef FUNCTORDATASOURCES
#define FUNCTORDATASOURCES
    namespace detail {


        /**
         * Used to partially specialise the case when returning a void from a functor.
         */
        template<typename R, typename FunctorT>
        struct FunctionForwarder
        {
            typedef typename boost::remove_const<R>::type result_type;

            FunctorT gen;
            result_type res;

            result_type result() { return res; }

            FunctionForwarder(FunctorT& f)
                :gen(f)
            {}

            R invoke() {
                res = gen();
                return res;
            }

            template<typename Arg1T>
            R invoke(DataSource<Arg1T>* arg1) {
                // The "Forwarding problem" : gen is a boost::bind functor
                // and can not be given non-const temporaries or literal constants.
                // thus _to_be_sure_, we _must_ copy the result to a local variable
                // and then pass that variable on. Fortunately, if Arg1T is a reference,
                // this does not involve a value copy and if Arg1T is a value, 
                // the gen( ) function takes a reference to it, thus, again,
                // no additional copy is made.
                Arg1T a1 = arg1->get();
                res = gen(a1);
                arg1->updated();
                return res;
            }
            template<typename Arg1T, typename Arg2T>
            R invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                res = gen(a1,a2);
                arg1->updated();
                arg2->updated();
                return res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T>
            R invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                res = gen(a1,a2,a3);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                return res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
            R invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                res = gen(a1,a2,a3,a4);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                return res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
            R invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4, DataSource<Arg5T>* arg5) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                Arg5T a5 = arg5->get();
                res = gen(a1,a2,a3,a4,a5);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                arg5->updated();
                return res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T, typename Arg6T>
            R invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4, DataSource<Arg5T>* arg5, DataSource<Arg6T>* arg6) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                Arg5T a5 = arg5->get();
                Arg6T a6 = arg6->get();
                res = gen(a1,a2,a3,a4,a5,a6);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                arg5->updated();
                arg6->updated();
                return res;
            }
        };

        template<typename FunctorT>
        struct FunctionForwarder<void, FunctorT>
        {
            FunctorT gen;
            typedef void result_type;

            FunctionForwarder(FunctorT& f)
                :gen(f)
            {}

            result_type result() {}

            void invoke() {
                gen();
            }

            template< typename Arg1T>
            void invoke(DataSource<Arg1T>* arg1) {
                // The "Forwarding problem" : gen is a boost::bind functor
                // and can not be given non-const temporaries or literal constants.
                // thus _to_be_sure_, we _must_ copy the result to a local variable
                // and then pass that variable on. Fortunately, if Arg1T is a reference,
                // this does not involve a value copy and if Arg1T is a value, 
                // the gen( ) function takes a reference to it, thus, again,
                // no additional copy is made.
                Arg1T a1 = arg1->get();
                gen(a1);
                arg1->updated();
            }
            template< typename Arg1T, typename Arg2T>
            void invoke(DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                gen(a1,a2);
                arg1->updated();
                arg2->updated();
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T>
            void invoke(DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                gen(a1,a2,a3);
                arg1->updated();
                arg2->updated();
                arg3->updated();
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
            void invoke(DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                gen(a1,a2,a3,a4);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
            void invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4, DataSource<Arg5T>* arg5) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                Arg5T a5 = arg5->get();
                gen(a1,a2,a3,a4,a5);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                arg5->updated();
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T, typename Arg6T>
            void invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4, DataSource<Arg5T>* arg5, DataSource<Arg6T>* arg6) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                Arg5T a5 = arg5->get();
                Arg6T a6 = arg6->get();
                gen(a1,a2,a3,a4,a5,a6);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                arg5->updated();
                arg6->updated();
            }
        };

        /**
         * Specialise reference types.
         */
        template<typename R, typename FunctorT>
        struct FunctionForwarder<R&, FunctorT>
        {
            // The trick is to store the result (a reference) of the function
            // in a pointer.
            typedef R& result_type;
            typedef R*  store_type;

            FunctorT gen;
            store_type res;

            result_type result() { 
                return *res; 
            }

            FunctionForwarder(FunctorT& f)
                :gen(f), res(0)
            {}

            result_type invoke() {
                res = &(gen());
                return *res;
            }

            template<typename Arg1T>
            result_type invoke(DataSource<Arg1T>* arg1) {
                // The "Forwarding problem" : gen is a boost::bind functor
                // and can not be given non-const temporaries or literal constants.
                // thus _to_be_sure_, we _must_ copy the result to a local variable
                // and then pass that variable on. Fortunately, if Arg1T is a reference,
                // this does not involve a value copy and if Arg1T is a value, 
                // the gen( ) function takes a reference to it, thus, again,
                // no additional copy is made.
                Arg1T a1 = arg1->get();
                res = &gen(a1);
                arg1->updated();
                return *res;
            }
            template<typename Arg1T, typename Arg2T>
            result_type invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                res = &gen(a1,a2);
                arg1->updated();
                arg2->updated();
                return *res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T>
            result_type invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                res = &gen(a1,a2,a3);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                return *res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
            result_type invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                res = &gen(a1,a2,a3,a4);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                return *res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
            result_type invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4, DataSource<Arg5T>* arg5) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                Arg5T a5 = arg5->get();
                res = &gen(a1,a2,a3,a4,a5);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                arg5->updated();
                return *res;
            }

            template< typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T, typename Arg6T>
            result_type invoke( DataSource<Arg1T>* arg1, DataSource<Arg2T>* arg2, DataSource<Arg3T>* arg3, DataSource<Arg4T>* arg4, DataSource<Arg5T>* arg5, DataSource<Arg6T>* arg6) {
                Arg1T a1 = arg1->get();
                Arg2T a2 = arg2->get();
                Arg3T a3 = arg3->get();
                Arg4T a4 = arg4->get();
                Arg5T a5 = arg5->get();
                Arg6T a6 = arg6->get();
                res = &gen(a1,a2,a3,a4,a5,a6);
                arg1->updated();
                arg2->updated();
                arg3->updated();
                arg4->updated();
                arg5->updated();
                arg6->updated();
                return *res;
            }
        };



    /**
     * These classes are generic DataSources that take a
     * functor, and a number of DataSources corresponding with the
     * arguments of the functor, and use it to get data from..
     * @{
     */
    template<typename FunctorT>
    struct FunctorDataSource0
      : public DataSource< typename ReturnType<typename FunctorT::result_type>::type >
    {
      typedef typename ReturnType<typename FunctorT::result_type>::type value_t;
      mutable FunctionForwarder<value_t,FunctorT> ff;
    public:
        typedef boost::intrusive_ptr< FunctorDataSource0<FunctorT> > shared_ptr;

        FunctorDataSource0( FunctorT g )
            : ff( g )
        {
        }

        typename DataSource<value_t>::result_t get() const
        {
          return ff.invoke();
        }

        typename DataSource<value_t>::result_t value() const
        {
          return ff.result();
        }

        virtual FunctorDataSource0* clone() const
        {
            return new FunctorDataSource0( ff.gen );
        }
        virtual FunctorDataSource0* copy( std::map<const DataSourceBase*, DataSourceBase*>& /*alreadyCloned*/ ) const
        {
          return new FunctorDataSource0<FunctorT>( ff.gen );
        }
    };

  template<typename FunctorT, typename Arg1T>
  struct FunctorDataSource1
      : public DataSource< typename FunctorT::result_type >
  {
    typedef typename FunctorT::result_type value_t;
    mutable FunctionForwarder<value_t,FunctorT> ff;
    typename DataSource<Arg1T>::shared_ptr arg1;
  public:
      typedef boost::intrusive_ptr< FunctorDataSource1<FunctorT,Arg1T> > shared_ptr;

      FunctorDataSource1( FunctorT g, DataSource<Arg1T>* a1 = 0 )
          : ff( g ), arg1( a1 )
      {
      }

      void setArguments(DataSource<Arg1T>* a1) 
      {
          arg1 = a1;
      }

      value_t value() const
      {
          return ff.result();
      }

      value_t get() const
      {
          return ff.invoke( arg1.get() );
      }

    virtual FunctorDataSource1<FunctorT, Arg1T>* clone() const
      {
        return new FunctorDataSource1<FunctorT, Arg1T>( ff.gen, arg1.get() );
      }
    virtual FunctorDataSource1<FunctorT, Arg1T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource1<FunctorT, Arg1T>( ff.gen, arg1->copy( alreadyCloned ) );
      }
  };

    /** coded for inState("string")
     * the component pointer itself is also stored in a datasource, such that if this
     * datasource is copied, the component pointer is updated.
     */
    template<typename ComponentT, typename FunctorT>
    class FunctorDataSourceDS0
      : public DataSource< typename FunctorT::result_type >
    {
    public:
        typedef typename FunctorT::result_type value_t;
        typedef typename DataSource<value_t>::value_t plain_t;
    private:
        typename DataSource<boost::weak_ptr<ComponentT> >::shared_ptr ds;
        mutable FunctorT gen;
        mutable plain_t res;
        static plain_t empty_return;
    public:
        typedef boost::intrusive_ptr< FunctorDataSourceDS0<ComponentT,FunctorT> > shared_ptr;

      FunctorDataSourceDS0(DataSource<boost::weak_ptr<ComponentT> >*c, FunctorT g )
        : ds(c), gen( g )
        {
        }

        value_t get() const
        {
            boost::shared_ptr<ComponentT> c = ds->get().lock();
            if(c) {
                ComponentT* ct = c.get();
                res = gen( ct );
                return res;
            }else
                return empty_return;
        }

        value_t value() const 
        {
            return res;
        }

        virtual FunctorDataSourceDS0<ComponentT,FunctorT>* clone() const
        {
            return new FunctorDataSourceDS0<ComponentT,FunctorT>( ds.get(),  gen );
        }
      virtual FunctorDataSourceDS0<ComponentT,FunctorT>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
          return new FunctorDataSourceDS0<ComponentT, FunctorT>( ds->copy(alreadyCloned),  gen );
        }
    };
        // static empty return type.
        template<typename ComponentT, typename FunctorT>
        typename FunctorDataSourceDS0<ComponentT, FunctorT>::plain_t
        FunctorDataSourceDS0<ComponentT, FunctorT>::empty_return;

        /**
         * Can not encapsulate void functions !
         */
  template<typename ComponentT, typename FunctorT, typename Arg1T>
  class FunctorDataSourceDS1
      : public DataSource< typename FunctorT::result_type >
  {
  public:
    typedef typename FunctorT::result_type value_t;
    typedef typename DataSource<value_t>::value_t plain_t;
  private:
      typename DataSource<boost::weak_ptr<ComponentT> >::shared_ptr ds;
      mutable FunctorT gen;
      typename DataSource<Arg1T>::shared_ptr arg1;
      mutable plain_t res;
      static plain_t empty_return;
  public:
        typedef boost::intrusive_ptr< FunctorDataSourceDS1<ComponentT,FunctorT,Arg1T> > shared_ptr;

    FunctorDataSourceDS1(DataSource<boost::weak_ptr<ComponentT> >* c, FunctorT g, DataSource<Arg1T>* a1 = 0 )
      : ds(c), gen( g ), arg1( a1 )
      {
      }

    value_t get() const
      {
        Arg1T a = arg1->get();
        boost::shared_ptr<ComponentT> c = ds->get().lock(); 
        if (c) {
            ComponentT* ct = c.get();
            res = gen( ct, a );
            arg1->updated();
            return res;
        } else
            return empty_return;
      }

      void setArguments(DataSource<Arg1T>* a1) 
      {
          arg1 = a1;
      }

        value_t value() const 
        {
            return res;
        }

    virtual FunctorDataSourceDS1<ComponentT, FunctorT, Arg1T>* clone() const
      {
        return new FunctorDataSourceDS1<ComponentT, FunctorT, Arg1T>( ds.get(), gen, arg1.get() );
      }
    virtual FunctorDataSourceDS1<ComponentT, FunctorT, Arg1T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSourceDS1<ComponentT, FunctorT, Arg1T>( ds->copy(alreadyCloned),  gen, arg1->copy( alreadyCloned ) );
      }
  };
        // static empty return type.
        template<typename ComponentT, typename FunctorT, typename A>
        typename FunctorDataSourceDS1<ComponentT, FunctorT, A>::plain_t
        FunctorDataSourceDS1<ComponentT, FunctorT, A>::empty_return;

  template<typename FunctorT, typename Arg1T, typename Arg2T>
  struct FunctorDataSource2
      : public DataSource< typename FunctorT::result_type >
  {
    typedef typename FunctorT::result_type value_t;
    mutable FunctionForwarder<value_t,FunctorT> ff;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
  public:
      typedef boost::intrusive_ptr< FunctorDataSource2<FunctorT,Arg1T,Arg2T> > shared_ptr;

      FunctorDataSource2( FunctorT g, DataSource<Arg1T>* a1 = 0, DataSource<Arg2T>* a2 = 0 )
          : ff( g ), arg1( a1 ), arg2(a2)
      {
      }

      void setArguments(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) 
      {
          arg1 = a1;
          arg2 = a2;
      }

      value_t value() const
      {
          return ff.result();
      }

      value_t get() const
      {
          return ff.invoke( arg1.get(), arg2.get() );
      }

    virtual FunctorDataSource2<FunctorT, Arg1T, Arg2T>* clone() const
      {
        return new FunctorDataSource2<FunctorT, Arg1T, Arg2T>( ff.gen, arg1.get(), arg2.get() );
      }
    virtual FunctorDataSource2<FunctorT, Arg1T, Arg2T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource2<FunctorT, Arg1T, Arg2T>( ff.gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  class FunctorDataSource3
      : public DataSource< typename FunctorT::result_type >
  {
    typedef typename FunctorT::result_type value_t;
    mutable FunctionForwarder<value_t,FunctorT> ff;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
    typename DataSource<Arg3T>::shared_ptr arg3;
  public:
      typedef boost::intrusive_ptr< FunctorDataSource3<FunctorT,Arg1T,Arg2T,Arg3T> > shared_ptr;
      
      FunctorDataSource3( FunctorT g, DataSource<Arg1T>* a1 = 0, DataSource<Arg2T>* a2 = 0, DataSource<Arg3T>* a3 = 0)
          : ff( g ), arg1( a1 ), arg2(a2), arg3(a3)
      {
      }

      void setArguments(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) 
      {
          arg1 = a1;
          arg2 = a2;
          arg3 = a3;
      }


      value_t value() const
      {
          return ff.result();
      }

      value_t get() const
      {
          return ff.invoke( arg1.get(), arg2.get(), arg3.get() );
      }

    virtual FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>* clone() const
      {
        return new FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>( ff.gen, arg1.get(), arg2.get(), arg3.get() );
      }
    virtual FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>( ff.gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned), arg3->copy( alreadyCloned) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  class FunctorDataSource4
      : public DataSource< typename FunctorT::result_type >
  {
    typedef typename FunctorT::result_type value_t;
      mutable FunctionForwarder<value_t,FunctorT> ff;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
    typename DataSource<Arg3T>::shared_ptr arg3;
    typename DataSource<Arg4T>::shared_ptr arg4;
  public:
      typedef boost::intrusive_ptr< FunctorDataSource4<FunctorT,Arg1T,Arg2T,Arg3T,Arg4T> > shared_ptr;

      FunctorDataSource4( FunctorT g, DataSource<Arg1T>* a1 = 0, DataSource<Arg2T>* a2 = 0,
                          DataSource<Arg3T>* a3 = 0, DataSource<Arg4T>* a4 = 0)
          : ff( g ), arg1( a1 ), arg2(a2), arg3(a3), arg4(a4)
      {
      }

      value_t value() const
      {
          return ff.result();
      }

      void setArguments(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) 
      {
          arg1 = a1;
          arg2 = a2;
          arg3 = a3;
          arg4 = a4;
      }

      value_t get() const
      {
          return ff.invoke( arg1.get(), arg2.get(), arg3.get(), arg4.get() );
      }

    virtual FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>* clone() const
      {
        return new FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( ff.gen, arg1.get(), arg2.get(),
                                                                             arg3.get(), arg4.get() );
      }
    virtual FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( ff.gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned), arg3->copy( alreadyCloned), arg4->copy( alreadyCloned) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
  class FunctorDataSource5
      : public DataSource< typename FunctorT::result_type >
  {
    typedef typename FunctorT::result_type value_t;
      mutable FunctionForwarder<value_t,FunctorT> ff;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
    typename DataSource<Arg3T>::shared_ptr arg3;
    typename DataSource<Arg4T>::shared_ptr arg4;
    typename DataSource<Arg5T>::shared_ptr arg5;
  public:
      typedef boost::intrusive_ptr< FunctorDataSource5<FunctorT,Arg1T,Arg2T,Arg3T,Arg4T,Arg5T> > shared_ptr;

      FunctorDataSource5( FunctorT g, DataSource<Arg1T>* a1 = 0, DataSource<Arg2T>* a2 = 0,
                          DataSource<Arg3T>* a3 = 0, DataSource<Arg4T>* a4 = 0,
                          DataSource<Arg5T>* a5 = 0)
          : ff( g ), arg1( a1 ), arg2(a2), arg3(a3), arg4(a4), arg5(a5)
      {
      }

      value_t value() const
      {
          return ff.result();
      }

      void setArguments(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2,
                        DataSource<Arg3T>* a3, DataSource<Arg4T>* a4,
                        DataSource<Arg5T>* a5) 
      {
          arg1 = a1;
          arg2 = a2;
          arg3 = a3;
          arg4 = a4;
          arg5 = a5;
      }

      value_t get() const
      {
          return ff.invoke( arg1.get(), arg2.get(), arg3.get(), arg4.get(), arg5.get() );
      }

    virtual FunctorDataSource5<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T>* clone() const
      {
        return new FunctorDataSource5<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T>( ff.gen, arg1.get(), arg2.get(),
                                                                             arg3.get(), arg4.get(), arg5.get() );
      }
    virtual FunctorDataSource5<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource5<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T>( ff.gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned), arg3->copy( alreadyCloned), arg4->copy( alreadyCloned), arg5->copy(alreadyCloned) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T, typename Arg6T>
  class FunctorDataSource6
      : public DataSource< typename FunctorT::result_type >
  {
    typedef typename FunctorT::result_type value_t;
      mutable FunctionForwarder<value_t,FunctorT> ff;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
    typename DataSource<Arg3T>::shared_ptr arg3;
    typename DataSource<Arg4T>::shared_ptr arg4;
    typename DataSource<Arg5T>::shared_ptr arg5;
    typename DataSource<Arg6T>::shared_ptr arg6;
  public:
      typedef boost::intrusive_ptr< FunctorDataSource6<FunctorT,Arg1T,Arg2T,Arg3T,Arg4T,Arg5T,Arg6T> > shared_ptr;

      FunctorDataSource6( FunctorT g, DataSource<Arg1T>* a1 = 0, DataSource<Arg2T>* a2 = 0,
                          DataSource<Arg3T>* a3 = 0, DataSource<Arg4T>* a4 = 0,
                          DataSource<Arg5T>* a5 = 0, DataSource<Arg6T>* a6 = 0)
          : ff( g ), arg1( a1 ), arg2(a2), arg3(a3), arg4(a4), arg5(a5),arg6(a6)
      {
      }

      value_t value() const
      {
          return ff.result();
      }

      void setArguments(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2,
                        DataSource<Arg3T>* a3, DataSource<Arg4T>* a4,
                        DataSource<Arg5T>* a5, DataSource<Arg6T>* a6) 
      {
          arg1 = a1;
          arg2 = a2;
          arg3 = a3;
          arg4 = a4;
          arg5 = a5;
          arg6 = a6;
      }

      value_t get() const
      {
          return ff.invoke( arg1.get(), arg2.get(), arg3.get(), arg4.get(), arg5.get(), arg6.get() );
      }

    virtual FunctorDataSource6<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T>* clone() const
      {
        return new FunctorDataSource6<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T>( ff.gen, arg1.get(), arg2.get(),
                                                                             arg3.get(), arg4.get(), arg5.get(), arg6.get() );
      }
    virtual FunctorDataSource6<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource6<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T>( ff.gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned), arg3->copy( alreadyCloned), arg4->copy( alreadyCloned), arg5->copy(alreadyCloned), arg6->copy(alreadyCloned) );
      }
  };



        template<int, class F>
        struct FunctorDataSourceI;

        template<class F>
        struct FunctorDataSourceI<0,F>
            : public FunctorDataSource0<F>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource0<F>(f) {}
        };

        template<class F>
        struct FunctorDataSourceI<1,F>
            : public FunctorDataSource1<F, typename F::arg1_type>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource1<F, typename F::arg1_type>(f) {}
        };

        template<class F>
        struct FunctorDataSourceI<2,F>
            : public FunctorDataSource2<F, typename F::arg1_type, typename F::arg2_type>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource2<F, typename F::arg1_type, typename F::arg2_type>(f) {}
        };

        template<class F>
        struct FunctorDataSourceI<3,F>
            : public FunctorDataSource3<F, typename F::arg1_type, typename F::arg2_type, typename F::arg3_type>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource3<F, typename F::arg1_type, typename F::arg2_type, typename F::arg3_type>(f) {}
        };

        template<class F>
        struct FunctorDataSourceI<4,F>
            : public FunctorDataSource4<F, typename F::arg1_type, typename F::arg2_type,typename F::arg3_type, typename F::arg4_type>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource4<F, typename F::arg1_type, typename F::arg2_type,typename F::arg3_type, typename F::arg4_type>(f) {}
        };

        template<class F>
        struct FunctorDataSourceI<5,F>
            : public FunctorDataSource5<F, typename F::arg1_type, typename F::arg2_type,
                                        typename F::arg3_type, typename F::arg4_type,
                                        typename F::arg5_type>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource5<F, typename F::arg1_type, typename F::arg2_type,
                                     typename F::arg3_type, typename F::arg4_type,
                                     typename F::arg5_type>(f) {}
        };


        template<class F>
        struct FunctorDataSourceI<6,F>
            : public FunctorDataSource6<F, typename F::arg1_type, typename F::arg2_type,
                                        typename F::arg3_type, typename F::arg4_type,
                                        typename F::arg5_type, typename F::arg6_type>
        {
            FunctorDataSourceI( F f )
                : FunctorDataSource6<F, typename F::arg1_type, typename F::arg2_type,
                                     typename F::arg3_type, typename F::arg4_type,
                                     typename F::arg5_type, typename F::arg6_type>(f) {}
        };

        template<class F>
        struct FunctorDataSource
            : public FunctorDataSourceI<F::arity, F>
        {
            FunctorDataSource( F f )
                : FunctorDataSourceI<F::arity, F>(f) {}
        };

  /**
   * @}
   */

  /**
   * @internal 
   * @defgroup datasourcefunctorhelper Helper functions for the use of FunctorDataSource.
   * This
   * is provided because for classes, C++ compilers never deduce
   * template parameters, only for functions.  This is a common
   * paradigm, std::back_inserter does the same thing.
   * @{
   */
  template<typename FunctorT>
  DataSource<typename FunctorT::result_type >*
  newFunctorDataSource( FunctorT g )
  {
    return new FunctorDataSource0<FunctorT>( g );
  };

  template<typename FunctorT, typename Arg1T>
  DataSource<typename FunctorT::result_type>*
  newFunctorDataSource( FunctorT g, DataSource<Arg1T>* a )
  {
    return new FunctorDataSource1<FunctorT, Arg1T>( g, a );
  };

  template<typename CompT, typename FunctorT>
  DataSource<typename FunctorT::result_type>*
  newFunctorDataSource( DataSource<boost::weak_ptr<CompT> >* c, FunctorT g )
  {
    return new FunctorDataSourceDS0<CompT, FunctorT>(c, g );
  };

  template<typename CompT, typename FunctorT, typename Arg1T>
  DataSource<typename FunctorT::result_type>*
  newFunctorDataSource( DataSource<boost::weak_ptr<CompT> >* c,  FunctorT g, DataSource<Arg1T>* a )
  {
    return new FunctorDataSourceDS1<CompT, FunctorT, Arg1T>(c, g, a );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T>
  DataSource<typename FunctorT::result_type>*
  newFunctorDataSource( FunctorT g, DataSource<Arg1T>* a, DataSource<Arg2T>* a2 )
  {
    return new FunctorDataSource2<FunctorT, Arg1T, Arg2T>( g, a, a2 );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type>*
  newFunctorDataSource( FunctorT g, DataSource<Arg1T>* a, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3 )
  {
    return new FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>( g, a, a2, a3 );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type>*
  newFunctorDataSource( FunctorT g, DataSource<Arg1T>* a, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4 )
  {
    return new FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( g, a, a2, a3, a4 );
  };

  /**
   * @}
   */

  /**
   * @internal 
   * @defgroup FunctorDataSourceGenerators Creates a DataSource object.
   * These are the classes responsible for the creation of
   * FunctorDataSource's from component member functions.  They are
   * implemented as functors themselves, because that's how we will be
   * using them.
   * TODO: add more overloads to support a larger number of arguments.
   * @{
   */
  template<typename ComponentT, typename ResultT,
           typename FunctorT>
  class FunctorDataSourceGenerator0
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGenerator0( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c ) const
      {
        return newFunctorDataSource( boost::bind( fun, c ) );
      };
  };

  template<typename ComponentT, typename ResultT, typename Arg1T,
           typename FunctorT>
  class FunctorDataSourceGenerator1
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGenerator1( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a ) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1 ), a );
      };
  };

  template<typename ComponentT, typename ResultT,
           typename FunctorT>
  class FunctorDataSourceGeneratorDS0
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGeneratorDS0( FunctorT f )
      : fun( f )
      {
      }
    DataSource<ResultT>* operator()( DataSource<boost::weak_ptr<ComponentT> >* c) const
      {
        return newFunctorDataSource( c, boost::bind( fun, _1 ) );
      }
  };

  template<typename ComponentT, typename ResultT, typename Arg1T,
           typename FunctorT>
  class FunctorDataSourceGeneratorDS1
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGeneratorDS1( FunctorT f )
      : fun( f )
      {
      }
    DataSource<ResultT>* operator()( DataSource<boost::weak_ptr<ComponentT> >* c, Arg1T a ) const
      {
        return newFunctorDataSource( c, boost::bind( fun, _1, a ) );
      }
    DataSource<ResultT>* operator()( DataSource<boost::weak_ptr<ComponentT> >* c, DataSource<Arg1T>* a ) const
      {
        return newFunctorDataSource( c, boost::bind( fun, _1, _2 ), a );
      }
  };

    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T,
           typename FunctorT>
  class FunctorDataSourceGenerator2
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGenerator2( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a, Arg2T a2 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a, a2 ) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a, DataSource<Arg2T>* a2 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1, _2 ), a, a2 );
      };
  };

    template<typename ComponentT, typename ResultT,
             typename Arg1T, typename Arg2T,
             typename Arg3T,
           typename FunctorT>
  class FunctorDataSourceGenerator3
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGenerator3( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a, Arg2T a2, Arg3T a3 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a, a2, a3) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a, DataSource<Arg2T>* a2
      , DataSource<Arg3T>* a3 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1, _2, _3 ), a, a2, a3);
      };
  };

    template<typename ComponentT, typename ResultT,
             typename Arg1T, typename Arg2T,
             typename Arg3T, typename Arg4T,
           typename FunctorT>
  class FunctorDataSourceGenerator4
  {
    FunctorT fun;
  public:
      typedef ResultT result_type;
    FunctorDataSourceGenerator4( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a, Arg2T a2, Arg3T a3, Arg4T a4 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a, a2, a3, a4 ) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a, DataSource<Arg2T>* a2
      , DataSource<Arg3T>* a3, DataSource<Arg4T>* a4 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1, _2, _3, _4 ), a, a2, a3, a4 );
      };
  };
  /**
   * @}
   */

  /**
   * @internal 
   * @defgroup functordshelpers FunctorDataSourceGenerator.
   * Helper functions to let the
   * compiler do the type deducing for us.
   * @{
   */
  template<typename ComponentT, typename ResultT, typename FunctorT>
  FunctorDataSourceGenerator0<ComponentT, ResultT, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator0<ComponentT, ResultT, FunctorT>( fun );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T,
           typename FunctorT>
  FunctorDataSourceGenerator1<ComponentT, ResultT, Arg1T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator1<
      ComponentT, ResultT, Arg1T, FunctorT>( fun );
  };

  template<typename ComponentT, typename ResultT,
           typename FunctorT>
  FunctorDataSourceGeneratorDS0<ComponentT, ResultT, FunctorT>
  fun_datasource_gen_ds( FunctorT fun )
  {
    return FunctorDataSourceGeneratorDS0<
      ComponentT, ResultT, FunctorT>( fun );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T,
           typename FunctorT>
  FunctorDataSourceGeneratorDS1<ComponentT, ResultT, Arg1T, FunctorT>
  fun_datasource_gen_ds( FunctorT fun )
  {
    return FunctorDataSourceGeneratorDS1<
      ComponentT, ResultT, Arg1T, FunctorT>( fun );
  };

    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T,
           typename FunctorT>
  FunctorDataSourceGenerator2<ComponentT, ResultT, Arg1T, Arg2T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator2<
      ComponentT, ResultT, Arg1T, Arg2T, FunctorT>( fun );
  };

    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T,
           typename FunctorT>
  FunctorDataSourceGenerator3<ComponentT, ResultT, Arg1T, Arg2T, Arg3T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator3<
      ComponentT, ResultT, Arg1T, Arg2T, Arg3T, FunctorT>( fun );
  };
    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T,
           typename FunctorT>
  FunctorDataSourceGenerator4<ComponentT, ResultT, Arg1T, Arg2T, Arg3T, Arg4T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator4<
      ComponentT, ResultT, Arg1T, Arg2T, Arg3T, Arg4T, FunctorT>( fun );
  };
  /**
   * @}
   */
    }
#endif // FUNCTORDATASOURCES

  /**
   * This is the DataSource factory class that is generated
   * by the newDataSourceFactory function..  Use the helper function
   * newDataSourceFactory.
   */
  template<typename ComponentT>
  class TEMPLATE_FACTORY_NAME
    : public MemberFactoryInterface,
      private TemplateFactory<ComponentT, DataSourceBase*>
  {
    typedef TemplateFactory<ComponentT, DataSourceBase*> _TF;
  public:
    TEMPLATE_FACTORY_NAME( ComponentT* c )
      : _TF( c )
      {
      };

    std::vector<std::string> getNames() const
      {
        return _TF::getNames();
      };

    bool hasMember( const std::string& name ) const
      {
        return _TF::hasMember( name );
      };

    std::string getResultType( const std::string& method ) const
      {
        return _TF::getResultType( method );
      }

    std::string getDescription( const std::string& method ) const
      {
        return _TF::getDescription( method );
      };

    PropertyBag getArgumentSpec( const std::string& method ) const
      {
        return _TF::getArgumentSpec( method );
      };

    std::vector< ArgumentDescription > getArgumentList( const std::string& method ) const
      {
          return _TF::getArgumentList( method );
      }

       int getArity(const std::string& method ) const 
      {
          return _TF::getArity(method);
      }

    DataSourceBase* create(
      const std::string& name,
      const PropertyBag& args ) const
      {
        return _TF::produce( name, args );
      };

    DataSourceBase* create(
      const std::string& name,
      const std::vector<DataSourceBase*>& args ) const
      {
        return _TF::produce( name, args );
      };

      DataSourceBase* create(
                             const std::string& name,
                             const std::vector<DataSourceBase::shared_ptr>& args ) const
      {
          return _TF::produce( name, args );
      };


    void add( const std::string& name,
              detail::TemplateFactoryPart<ComponentT, DataSourceBase*>* part )
      {
        _TF::add( name, part );
      }
  };

  /**
   * This is the function that generates the DataSourceFactory you
   * want.
   */
  template<typename ComponentT>
  TEMPLATE_FACTORY_NAME<ComponentT>*
  NEW_MEMBER_FACTORY( ComponentT* t )
  {
    return new TEMPLATE_FACTORY_NAME<ComponentT>( t );
  };


  /**
   * The data function is the function you should use to specify a
   * certain property to be added to your TemplateDataSourceFactory..
   * Several overloads are provided, for supporting various numbers of
   * arguments..
   *
   * There is also an overload, which allows you to provide access to
   * a member variable, thus avoiding the need for a getter
   * function..
   *
   * as the second argument, you should pass a description for the
   * data, and the following arguments are a name and description for
   * each argument of the data.
   *
   * @todo more overloads to support a larger number of arguments.
   * We support zero to four arguments.
   * @{
   */
  // A component's variable :
  template<typename ComponentT, typename ResultT>
  detail::TemplateFactoryPart<
    typename detail::CompType<ComponentT>::type,
    DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*var) , const char* desc )
  {
      // in this special case, we need to ensure that no const slips in in ComponentT
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*>( detail::fun_datasource_gen<typename detail::CompType<ComponentT>::type, typename detail::ReturnType<ResultT>::type >(
                          boost::bind<ResultT>(boost::mem_fn<ResultT, ComponentT>( var ), _1 ) ), desc );
  };

  // A component's method :
  template<typename ComponentT, typename ResultT>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)() const, const char* desc )
  {
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*>( detail::fun_datasource_gen<ComponentT, typename detail::ReturnType<ResultT>::type >(
                          boost::mem_fn( fun ) ), desc );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T ) const, const char* desc,
        const char* a1n, const char* a1d )
  {
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T>(
        detail::fun_datasource_gen<ComponentT, 
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type>(
          boost::mem_fn( fun ) ), desc, a1n, a1d );
  };

  template<typename ComponentT, typename ResultT>
  detail::TemplateFactoryPart< DataSource<boost::weak_ptr<typename detail::CompType<ComponentT>::type> >,
                      DataSourceBase*>*
  MEMBER_DS( ResultT (ComponentT::*fun)() const, const char* desc)
  {
    return detail::fun_fact_ds<typename detail::CompType<ComponentT>::type, DataSourceBase*>
        (
         detail::fun_datasource_gen_ds< typename detail::CompType<ComponentT>::type, typename detail::ReturnType<ResultT>::type >( boost::mem_fn( fun ) ),
         desc
         );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T>
  detail::TemplateFactoryPart< DataSource<boost::weak_ptr<typename detail::CompType<ComponentT>::type> >,
                      DataSourceBase*>*
  MEMBER_DS( ResultT (ComponentT::*fun)( Arg1T ) const, const char* desc,
        const char* a1n, const char* a1d )
  {
    return detail::fun_fact_ds<typename detail::CompType<ComponentT>::type, DataSourceBase*, typename detail::ArgType<Arg1T>::type>
        (
         detail::fun_datasource_gen_ds< typename detail::CompType<ComponentT>::type,
         typename detail::ReturnType<ResultT>::type,
         typename detail::ArgType<Arg1T>::type >( boost::mem_fn( fun ) ),
         desc, a1n, a1d 
         );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T ) const, const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d)
  {
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T>(
        detail::fun_datasource_gen<ComponentT,
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T, Arg3T ) const, const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d,
        const char* a3n, const char* a3d)
  {
      return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T, Arg3T>(
        detail::fun_datasource_gen<ComponentT,
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type,
        typename detail::ArgType<Arg3T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d, a3n, a3d);
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T, Arg3T, Arg4T ) const, const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d,
        const char* a3n, const char* a3d,
        const char* a4n, const char* a4d)
  {
      return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T, Arg3T, Arg4T>(
        detail::fun_datasource_gen<ComponentT,
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type,
        typename detail::ArgType<Arg3T>::type,
        typename detail::ArgType<Arg4T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d, a3n, a3d, a4n, a4d );
  };

    /**
     * Same methods, but without 'const'
     */
  template<typename ComponentT, typename ResultT>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)() , const char* desc )
  {
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*>( detail::fun_datasource_gen<ComponentT, typename detail::ReturnType<ResultT>::type >(
                          boost::mem_fn( fun ) ), desc );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T ) , const char* desc,
        const char* a1n, const char* a1d )
  {
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T>(
        detail::fun_datasource_gen<ComponentT, 
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type>(
          boost::mem_fn( fun ) ), desc, a1n, a1d );
  };

  template<typename ComponentT, typename ResultT>
  detail::TemplateFactoryPart< DataSource<boost::weak_ptr<typename detail::CompType<ComponentT>::type> >,
                      DataSourceBase*>*
  MEMBER_DS( ResultT (ComponentT::*fun)() , const char* desc)
  {
    return detail::fun_fact_ds<typename detail::CompType<ComponentT>::type, DataSourceBase*>
        (
         detail::fun_datasource_gen_ds< typename detail::CompType<ComponentT>::type, typename detail::ReturnType<ResultT>::type >( boost::mem_fn( fun ) ),
         desc
         );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T>
  detail::TemplateFactoryPart< DataSource<boost::weak_ptr<typename detail::CompType<ComponentT>::type> >,
                      DataSourceBase*>*
  MEMBER_DS( ResultT (ComponentT::*fun)( Arg1T ) , const char* desc,
        const char* a1n, const char* a1d )
  {
    return detail::fun_fact_ds<typename detail::CompType<ComponentT>::type, DataSourceBase*, typename detail::ArgType<Arg1T>::type>
        (
         detail::fun_datasource_gen_ds< typename detail::CompType<ComponentT>::type,
         typename detail::ReturnType<ResultT>::type,
         typename detail::ArgType<Arg1T>::type >( boost::mem_fn( fun ) ),
         desc, a1n, a1d 
         );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T ) , const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d)
  {
    return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T>(
        detail::fun_datasource_gen<ComponentT,
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T, Arg3T ) , const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d,
        const char* a3n, const char* a3d)
  {
      return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T, Arg3T>(
        detail::fun_datasource_gen<ComponentT,
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type,
        typename detail::ArgType<Arg3T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d, a3n, a3d);
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  detail::TemplateFactoryPart<typename detail::CompType<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T, Arg3T, Arg4T ) , const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d,
        const char* a3n, const char* a3d,
        const char* a4n, const char* a4d)
  {
      return detail::fun_fact<typename detail::CompType<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T, Arg3T, Arg4T>(
        detail::fun_datasource_gen<ComponentT,
        typename detail::ReturnType<ResultT>::type,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type,
        typename detail::ArgType<Arg3T>::type,
        typename detail::ArgType<Arg4T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d, a3n, a3d, a4n, a4d );
  };

  /**
   * @}
   */

}

#else
// This is a nice work-around for Doxygen.
#include "TemplateMethodFactory.hpp"
#include "TemplateDataSourceFactory.hpp"
#endif // defined(MEMBER)
