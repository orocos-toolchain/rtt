/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  DataSource.hpp

                        DataSource.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef ORO_EXECTUTION_DATASOURCE_HPP
#define ORO_EXECTUTION_DATASOURCE_HPP

#include "corelib/DataSource.hpp"

namespace ORO_Execution
{
    using ORO_CoreLib::DataSourceBase;
    using ORO_CoreLib::DataSource;
    using ORO_CoreLib::AssignableDataSource;

  /**
   * A simple, yet very useful DataSource, which keeps a value, and
   * returns it in its get() method. 
   * The VariableDataSource is an AssignableDataSource, which
   * thus can be changed.
   * @param T The result data type of get().
   */
  template<typename T>
  class VariableDataSource
    : public AssignableDataSource<T>
  {
      typename DataSource<T>::value_t mdata;
  protected:
      /**
       * Use shared_ptr.
       */
      ~VariableDataSource() {}
  public:
      typedef boost::intrusive_ptr<VariableDataSource<T> > shared_ptr;

      VariableDataSource( T data )
          : mdata( data )
      {
      }

      VariableDataSource( )
          : mdata()
      {
      }

      typename DataSource<T>::result_t get() const
      {
          return mdata;
      }

      void set( typename AssignableDataSource<T>::param_t t )
      {
          mdata = t;
      }

      typename AssignableDataSource<T>::reference_t set() {
          return mdata;
      }

      virtual VariableDataSource<T>* clone() const
      {
          return new VariableDataSource<T>(mdata);
      }

      virtual VariableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
          if ( i == alreadyCloned.end() ) {
              VariableDataSource<T>* n = new VariableDataSource<T>( mdata );
              alreadyCloned[this] = n;
              return n;
          }
          assert( dynamic_cast<VariableDataSource<T>*>( i->second ) == static_cast<VariableDataSource<T>*>( i->second ) );
          return static_cast<VariableDataSource<T>*>( i->second );
      }
  };

  /**
   * Specialisation for const std::string& to keep capacity when set( ... ) is called.
   */
  template<>
  class VariableDataSource< const std::string& >
    : public AssignableDataSource<const std::string&>
  {
      typedef const std::string& T;
      DataSource<T>::value_t mdata;
  protected:
      /**
       * Use shared_ptr.
       */
      ~VariableDataSource() {}
  public:
      typedef boost::intrusive_ptr<VariableDataSource<T> > shared_ptr;

      VariableDataSource( T data )
          : mdata( data )
      {
      }

      VariableDataSource( )
          : mdata()
      {
      }

      DataSource<T>::result_t get() const
      {
          return mdata;
      }

      void set(  AssignableDataSource<T>::param_t t )
      {
          mdata = t.c_str();
      }

      AssignableDataSource<T>::reference_t set() {
          return mdata;
      }

      virtual VariableDataSource<T>* clone() const
      {
          return new VariableDataSource<T>(mdata);
      }

      virtual VariableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
          if ( i == alreadyCloned.end() ) {
              VariableDataSource<T>* n = new VariableDataSource<T>( mdata );
              alreadyCloned[this] = n;
              return n;
          }
          assert( dynamic_cast<VariableDataSource<T>*>( i->second ) == static_cast<VariableDataSource<T>*>( i->second ) );
          return static_cast<VariableDataSource<T>*>( i->second );
      }
  };

  /**
   * A DataSource which holds a constant value and
   * returns it in its get() method. It can not be changed after creation.
   * @param T Any type of data, except being a non-const reference.
   */
  template<typename T>
  class ConstantDataSource
    : public DataSource<T>
  {
      /**
       * Assure that mdata is const, such that T is forced
       * to not be a non-const reference.
       */
      typename boost::add_const<typename DataSource<T>::value_t>::type mdata;
  protected:
      /**
       * Use shared_ptr.
       */
      ~ConstantDataSource() {}
  public:
      typedef boost::intrusive_ptr< ConstantDataSource<T> > shared_ptr;

      ConstantDataSource( T value )
          : mdata( value )
      {
      }

      typename DataSource<T>::result_t get() const
      {
          return mdata;
      }

      virtual ConstantDataSource<T>* clone() const
      {
          return new ConstantDataSource<T>(mdata);
      }

      virtual ConstantDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          // no copy needed, share this with all instances.
          return this;
      }
  };

  /**
   * A generic binary composite DataSource.  It takes a function
   * object which is a model of the STL Adaptable Binary Function
   * concept, and two DataSource's with result types matching the
   * argument types of the given function.  Its get() method then
   * applies the function to the results of the get() methods of the
   * two DataSource's given.
   *
   * The parser system uses a hell of a lot of these for evaluating
   * expressions, comparisons etc.  I must say I think this is pretty
   * cool ;)  C++ rocks ! ;)
   */
  template<typename function>
  class BinaryDataSource
    : public DataSource<typename function::result_type>
  {
    typedef typename function::result_type value_t;
    typedef typename function::first_argument_type  first_arg_t;
    typedef typename function::second_argument_type second_arg_t;
    typename DataSource<first_arg_t>::shared_ptr ma;
    typename DataSource<second_arg_t>::shared_ptr mb;
    function fun;
  public:
    typedef boost::intrusive_ptr<BinaryDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f which is given argument \a a and \a b.
       */ 
    BinaryDataSource( typename DataSource<first_arg_t>::shared_ptr a,
                      typename DataSource<second_arg_t>::shared_ptr b,
                      function f )
      : ma( a ), mb( b ), fun( f )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = ma->get();
        second_arg_t b = mb->get();
        return fun( a, b );
      }

    virtual void reset()
      {
        ma->reset();
        mb->reset();
      }

      virtual BinaryDataSource<function>* clone() const
      {
          return new BinaryDataSource<function>(ma.get(), mb.get(), fun);
      }

      virtual BinaryDataSource<function>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          return new BinaryDataSource<function>( ma->copy( alreadyCloned ), mb->copy( alreadyCloned ), fun );
      }
  };

  /**
   * A DataSource which returns the return value of a ternary function.
   */
  template<typename function>
  class TernaryDataSource
    : public DataSource<typename function::result_type>
  {
    typedef typename function::result_type value_t;
    typedef typename function::first_argument_type first_arg_t;
    typedef typename function::second_argument_type second_arg_t;
    typedef typename function::third_argument_type third_arg_t;
    typename DataSource<first_arg_t>::shared_ptr ma;
    typename DataSource<second_arg_t>::shared_ptr mb;
    typename DataSource<third_arg_t>::shared_ptr mc;
    function fun;
  public:
    typedef boost::intrusive_ptr<TernaryDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f which is given argument \a a to \a c.
       */ 
    TernaryDataSource( typename DataSource<first_arg_t>::shared_ptr a,
                       typename DataSource<second_arg_t>::shared_ptr b,
                       typename DataSource<third_arg_t>::shared_ptr c,
                       function f )
      : ma( a ), mb( b ), mc( c ), fun( f )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = ma->get();
        second_arg_t b = mb->get();
        third_arg_t c = mc->get();
        return fun( a, b, c );
      }

    virtual void reset()
      {
        ma->reset();
        mb->reset();
        mc->reset();
      }

      virtual TernaryDataSource<function>* clone() const
      {
          return new TernaryDataSource<function>(ma.get(), mb.get(), mc.get(), fun);
      }

      virtual TernaryDataSource<function>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          return new TernaryDataSource<function>( ma->copy( alreadyCloned ), mb->copy( alreadyCloned ), mc->copy( alreadyCloned ), fun );
      }

  };

  /**
   * A DataSource which returns the return value of a sixary function.
   */
  template<typename function>
  class SixaryDataSource
    : public DataSource<typename function::result_type>
  {
    typedef typename function::result_type value_t;
    typedef typename function::first_argument_type first_arg_t;
    typedef typename function::second_argument_type second_arg_t;
    typedef typename function::third_argument_type third_arg_t;
    typedef typename function::fourth_argument_type fourth_arg_t;
    typedef typename function::fifth_argument_type fifth_arg_t;
    typedef typename function::sixth_argument_type sixth_arg_t;
    typename DataSource<first_arg_t>::shared_ptr ma;
    typename DataSource<second_arg_t>::shared_ptr mb;
    typename DataSource<third_arg_t>::shared_ptr mc;
    typename DataSource<fourth_arg_t>::shared_ptr md;
    typename DataSource<fifth_arg_t>::shared_ptr me;
    typename DataSource<sixth_arg_t>::shared_ptr mf;
    function fun;
  public:
    typedef boost::intrusive_ptr<SixaryDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f which is given argument \a a to \a f.
       */ 
    SixaryDataSource(
                     typename DataSource<first_arg_t>::shared_ptr a,
                     typename DataSource<second_arg_t>::shared_ptr b,
                     typename DataSource<third_arg_t>::shared_ptr c,
                     typename DataSource<fourth_arg_t>::shared_ptr d,
                     typename DataSource<fifth_arg_t>::shared_ptr e,
                     typename DataSource<sixth_arg_t>::shared_ptr f,
                       function _fun )
      : ma( a ), mb( b ), mc( c ),md( d ), me( e ), mf( f ),
        fun( _fun )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = ma->get();
        second_arg_t b = mb->get();
        third_arg_t c = mc->get();
        fourth_arg_t d = md->get();
        fifth_arg_t e = me->get();
        sixth_arg_t f = mf->get();
        return fun( a, b, c, d, e, f );
      }

    virtual void reset()
      {
        ma->reset();
        mb->reset();
        mc->reset();
        md->reset();
        me->reset();
        mf->reset();
      }

      virtual SixaryDataSource<function>* clone() const
      {
          return new SixaryDataSource<function>(ma.get(), mb.get(), mc.get(),
                                                md.get(), me.get(), mf.get(),
                                                fun);
      }

      virtual SixaryDataSource<function>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          return new SixaryDataSource<function>( ma->copy( alreadyCloned ), mb->copy( alreadyCloned ),
                                                 mc->copy( alreadyCloned ), md->copy( alreadyCloned ),
                                                 me->copy( alreadyCloned ), mf->copy( alreadyCloned ), fun );
      }
  };

  /**
   * A DataSource which returns the return value of a unary function.
   */
  template <typename function>
  class UnaryDataSource
    : public DataSource<typename function::result_type>
  {
    typedef typename function::result_type value_t;
    typedef typename function::argument_type arg_t;
    typename DataSource<arg_t>::shared_ptr ma;
    function fun;
  public:
    typedef boost::intrusive_ptr<UnaryDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f which is given argument \a a.
       */ 
    UnaryDataSource( typename DataSource<arg_t>::shared_ptr a, function f )
      : ma( a ), fun( f )
      {
      }

    virtual value_t get() const
      {
        return fun( ma->get() );
      }

    void reset()
      {
        ma->reset();
      }

    virtual UnaryDataSource<function>* clone() const
      {
          return new UnaryDataSource<function>(ma.get(), fun);
      }

    virtual UnaryDataSource<function>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
          return new UnaryDataSource<function>( ma->copy( alreadyCloned ), fun );
      }
  };

    namespace detail {
        // backwards compatibility with 0.20.0
        using ::ORO_Execution::VariableDataSource;
        using ::ORO_Execution::UnaryDataSource;
        using ::ORO_Execution::BinaryDataSource;
        using ::ORO_Execution::TernaryDataSource;
        using ::ORO_Execution::SixaryDataSource;
    }
}

#endif
