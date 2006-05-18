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
    void ValueDataSource<const std::string&>::set(  AssignableDataSource<const std::string&>::param_t t );


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

        CommandInterface* updateCommand( const DataSourceBase* other) 
        {
            DataSourceBase::const_ptr r( other );
            const DataSource<T>* t = DataSource<T>::narrow( r.get() );
            if ( t )
                return new detail::AssignContainerCommand<T,APred>( this, t );
            const DataSource< typename AssignableDataSource<T>::const_reference_t >* ct
                = DataSource< typename AssignableDataSource<T>::const_reference_t >::narrow( r.get() );
            if ( ct )
                return new detail::AssignContainerCommand<T,APred, typename AssignableDataSource<T>::const_reference_t >( this, ct );

            return 0;
        }

        CommandInterface* updatePartCommand( DataSourceBase* index, DataSourceBase* rhs )
        {
            DataSourceBase::shared_ptr r( rhs );
            DataSourceBase::shared_ptr i( index );
            DataSource<SetType>* t = AdaptDataSource<SetType>()( r );
            if ( ! t )
                throw bad_assignment();
            DataSource<Index>* ind = AdaptDataSource<Index>()( i );
            if ( ! ind )
                throw bad_assignment();
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

    virtual value_t value() const
      {
        first_arg_t a = ma->value();
        second_arg_t b = mb->value();
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

      virtual BinaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
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

    virtual value_t value() const
      {
        first_arg_t a = ma->value();
        second_arg_t b = mb->value();
        third_arg_t c = mc->value();
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

      virtual TernaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
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

    virtual value_t value() const
      {
        first_arg_t a = ma->value();
        second_arg_t b = mb->value();
        third_arg_t c = mc->value();
        fourth_arg_t d = md->value();
        fifth_arg_t e = me->value();
        sixth_arg_t f = mf->value();
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

      virtual SixaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
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

    virtual value_t value() const
      {
        return fun( ma->value() );
      }

    void reset()
      {
        ma->reset();
      }

    virtual UnaryDataSource<function>* clone() const
      {
          return new UnaryDataSource<function>(ma.get(), fun);
      }

    virtual UnaryDataSource<function>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new UnaryDataSource<function>( ma->copy( alreadyCloned ), fun );
      }
  };

}

#include "DataSources.inl"

#endif

