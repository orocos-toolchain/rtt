/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  DataSources.hpp 

                        DataSources.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_CORELIB_DATASOURCES_HPP
#define ORO_CORELIB_DATASOURCES_HPP

#include "DataSource.hpp"
#include "DataSourceAdaptor.hpp"
#include "AssignCommand.hpp"

namespace ORO_CoreLib
{

    /**
     * A simple, yet very useful DataSource, which keeps a value, and
     * returns it in its get() method. 
     * This is an AssignableDataSource, which
     * thus can be changed. 
     * @param T The result data type of get().
     */
    template<typename T>
    class ValueDataSource
        : public AssignableDataSource<T>
    {
    protected:
        typename DataSource<T>::value_t mdata;

        /**
         * Use shared_ptr.
         */
        ~ValueDataSource();
    public:
        typedef boost::intrusive_ptr<ValueDataSource<T> > shared_ptr;

        ValueDataSource( T data );

        ValueDataSource( );

        typename DataSource<T>::result_t get() const;

        typename DataSource<T>::result_t value() const;

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set();

        typename AssignableDataSource<T>::const_reference_t rvalue() const;

        virtual ValueDataSource<T>* clone() const;

        virtual ValueDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replace ) const;
    };

    /**
     * Specialisation for const std::string& to keep capacity when set( ... ) is called.
     */
    template<>
    void ValueDataSource<std::string>::set(  AssignableDataSource<std::string>::param_t t );


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
        ~ConstantDataSource();
    public:
        typedef boost::intrusive_ptr< ConstantDataSource<T> > shared_ptr;

        ConstantDataSource( T value );

        typename DataSource<T>::result_t get() const;

        typename DataSource<T>::result_t value() const;

        virtual ConstantDataSource<T>* clone() const;

        virtual ConstantDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

    /**
     * A DataSource which is used to manipulate a reference to an
     * external value.
     * @param T The result data type of get().
     */
    template<typename T>
    class ReferenceDataSource
        : public AssignableDataSource<T>
    {
        // a reference to a value_t
        typename AssignableDataSource<T>::reference_t mref;
    protected:
        /**
         * Use shared_ptr.
         */
        ~ReferenceDataSource();
    public:
        typedef boost::intrusive_ptr<ReferenceDataSource<T> > shared_ptr;

        ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref );

        typename DataSource<T>::result_t get() const;

        typename DataSource<T>::result_t value() const;

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set();

        typename AssignableDataSource<T>::const_reference_t rvalue() const;

        virtual ReferenceDataSource<T>* clone() const;

        virtual ReferenceDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

    /**
     * A ValueDataSource of which individual parts can be updated
     * using an index.
     * @param T The type of the container.
     * @param Index The type of the index into the container.
     * @param SetType The type of an element of the container.
     * @param IPred A predicate which checks if the index is valid.
     * @param APred A predicate which checks if the container may be assigned to another container.
     */
    template<typename T, typename Index, typename SetType, typename IPred, typename APred>
    class IndexedValueDataSource
        : public ValueDataSource<T>
    {
    public:
        typedef boost::intrusive_ptr<IndexedValueDataSource<T, Index, SetType, IPred, APred> > shared_ptr;

        IndexedValueDataSource( T idata )
            : ValueDataSource<T>(idata) {}

        IndexedValueDataSource()
        {}

        CommandInterface* updateCommand( DataSourceBase* other) 
        {
            DataSourceBase::const_ptr r( other );
            typedef typename AssignableDataSource<T>::copy_t copy_t;
            DataSource< copy_t >* ct = AdaptDataSource<copy_t>()( other );
            if ( ct )
                return new detail::AssignContainerCommand<T,APred,copy_t >( this, ct );

#ifndef ORO_EMBEDDED
            throw bad_assignment();
#else
            return 0;
#endif
        }

        CommandInterface* updatePartCommand( DataSourceBase* index, DataSourceBase* rhs )
        {
            DataSourceBase::shared_ptr r( rhs );
            DataSourceBase::shared_ptr i( index );
            DataSource<SetType>* t = AdaptDataSource<SetType>()( r );
            if ( ! t ) {
#ifndef ORO_EMBEDDED
                throw bad_assignment();
#else
                return 0;
#endif
            }
            DataSource<Index>* ind = AdaptDataSource<Index>()( i );
            if ( ! ind ) {
#ifndef ORO_EMBEDDED
                throw bad_assignment();
#else
                return 0;
#endif
            }
            typename AssignableDataSource<T>::shared_ptr mthis(this);
            return new detail::AssignIndexCommand<T, Index, SetType, IPred>( mthis, ind ,t );
        }

        IndexedValueDataSource<T, Index, SetType,IPred,APred>* clone() const
        {
            return new IndexedValueDataSource( this->mdata );
        }

        IndexedValueDataSource<T, Index, SetType,IPred,APred>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replace) const
        {
            // if somehow a copy exists, return the copy, otherwise return this.
            if ( replace[this] != 0 ) {
                assert ( (dynamic_cast<IndexedValueDataSource<T, Index, SetType,IPred,APred>*>( replace[this] )
                         == static_cast<IndexedValueDataSource<T, Index, SetType,IPred,APred>*>( replace[this] ) ) );
                return static_cast<IndexedValueDataSource<T, Index, SetType,IPred,APred>*>( replace[this] );
            }
            // Other pieces in the code rely on insertion in the map :
            replace[this] = const_cast<IndexedValueDataSource<T, Index, SetType,IPred,APred>*>(this);
            // return this instead of a copy.
            return const_cast<IndexedValueDataSource<T, Index, SetType,IPred,APred>*>(this);
        }
    };

    namespace detail {
        /**
         * A special DataSource only to be used for if you understand
         * the copy()/clone() semantics very well.
         *
         * It has different copy semantics in comparison to 'bound' DataSources
         * (like ValueDataSource), being that, it returns new copies in copy(),
         * and returns a clone() of the contained datasource in clone().
         * A clone() reduces this 'UnboundDataSource' to a 'BoundType' data source.
         * @param BoundType The type to inherit from and to return in clone().
         * Typically a ValueDataSource or IndexedValueDataSource type.
         */
        template<typename BoundType>
        class UnboundDataSource
            : public BoundType
        {
        public:
            typedef typename BoundType::result_t T;
            typedef boost::intrusive_ptr< UnboundDataSource<BoundType> > shared_ptr;

            UnboundDataSource( T data );

            UnboundDataSource( );

            ~UnboundDataSource() {
            }

            virtual BoundType* clone() const {
                return BoundType::clone();
            }

            virtual UnboundDataSource<BoundType>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replace) const;
        };

    }

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
    typename DataSource<first_arg_t>::shared_ptr mdsa;
    typename DataSource<second_arg_t>::shared_ptr mdsb;
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
      : mdsa( a ), mdsb( b ), fun( f )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = mdsa->get();
        second_arg_t b = mdsb->get();
        return fun( a, b );
      }

    virtual value_t value() const
      {
        first_arg_t a = mdsa->value();
        second_arg_t b = mdsb->value();
        return fun( a, b );
      }

    virtual void reset()
      {
        mdsa->reset();
        mdsb->reset();
      }

      virtual BinaryDataSource<function>* clone() const
      {
          return new BinaryDataSource<function>(mdsa.get(), mdsb.get(), fun);
      }

      virtual BinaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new BinaryDataSource<function>( mdsa->copy( alreadyCloned ), mdsb->copy( alreadyCloned ), fun );
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
    typename DataSource<first_arg_t>::shared_ptr mdsa;
    typename DataSource<second_arg_t>::shared_ptr mdsb;
    typename DataSource<third_arg_t>::shared_ptr mdsc;
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
      : mdsa( a ), mdsb( b ), mdsc( c ), fun( f )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = mdsa->get();
        second_arg_t b = mdsb->get();
        third_arg_t c = mdsc->get();
        return fun( a, b, c );
      }

    virtual value_t value() const
      {
        first_arg_t a = mdsa->value();
        second_arg_t b = mdsb->value();
        third_arg_t c = mdsc->value();
        return fun( a, b, c );
      }

    virtual void reset()
      {
        mdsa->reset();
        mdsb->reset();
        mdsc->reset();
      }

      virtual TernaryDataSource<function>* clone() const
      {
          return new TernaryDataSource<function>(mdsa.get(), mdsb.get(), mdsc.get(), fun);
      }

      virtual TernaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new TernaryDataSource<function>( mdsa->copy( alreadyCloned ), mdsb->copy( alreadyCloned ), mdsc->copy( alreadyCloned ), fun );
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
    typename DataSource<first_arg_t>::shared_ptr mdsa;
    typename DataSource<second_arg_t>::shared_ptr mdsb;
    typename DataSource<third_arg_t>::shared_ptr mdsc;
    typename DataSource<fourth_arg_t>::shared_ptr mdsd;
    typename DataSource<fifth_arg_t>::shared_ptr mdse;
    typename DataSource<sixth_arg_t>::shared_ptr mdsf;
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
      : mdsa( a ), mdsb( b ), mdsc( c ),mdsd( d ), mdse( e ), mdsf( f ),
        fun( _fun )
      {
      }

    virtual value_t get() const
      {
        first_arg_t a = mdsa->get();
        second_arg_t b = mdsb->get();
        third_arg_t c = mdsc->get();
        fourth_arg_t d = mdsd->get();
        fifth_arg_t e = mdse->get();
        sixth_arg_t f = mdsf->get();
        return fun( a, b, c, d, e, f );
      }

    virtual value_t value() const
      {
        first_arg_t a = mdsa->value();
        second_arg_t b = mdsb->value();
        third_arg_t c = mdsc->value();
        fourth_arg_t d = mdsd->value();
        fifth_arg_t e = mdse->value();
        sixth_arg_t f = mdsf->value();
        return fun( a, b, c, d, e, f );
      }

    virtual void reset()
      {
        mdsa->reset();
        mdsb->reset();
        mdsc->reset();
        mdsd->reset();
        mdse->reset();
        mdsf->reset();
      }

      virtual SixaryDataSource<function>* clone() const
      {
          return new SixaryDataSource<function>(mdsa.get(), mdsb.get(), mdsc.get(),
                                                mdsd.get(), mdse.get(), mdsf.get(),
                                                fun);
      }

      virtual SixaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new SixaryDataSource<function>( mdsa->copy( alreadyCloned ), mdsb->copy( alreadyCloned ),
                                                 mdsc->copy( alreadyCloned ), mdsd->copy( alreadyCloned ),
                                                 mdse->copy( alreadyCloned ), mdsf->copy( alreadyCloned ), fun );
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
    typename DataSource<arg_t>::shared_ptr mdsa;
    function fun;
  public:
    typedef boost::intrusive_ptr<UnaryDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f which is given argument \a a.
       */ 
    UnaryDataSource( typename DataSource<arg_t>::shared_ptr a, function f )
      : mdsa( a ), fun( f )
      {
      }

    virtual value_t get() const
      {
        return fun( mdsa->get() );
      }

    virtual value_t value() const
      {
        return fun( mdsa->value() );
      }

    void reset()
      {
        mdsa->reset();
      }

    virtual UnaryDataSource<function>* clone() const
      {
          return new UnaryDataSource<function>(mdsa.get(), fun);
      }

    virtual UnaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new UnaryDataSource<function>( mdsa->copy( alreadyCloned ), fun );
      }
  };

}

#include "DataSources.inl"

#endif

