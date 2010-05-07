/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  DataSources.hpp

                        DataSources.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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


#ifndef ORO_CORELIB_DATASOURCES_HPP
#define ORO_CORELIB_DATASOURCES_HPP

#include "DataSource.hpp"
#include "DataSourceAdaptor.hpp"
#include "DataSourceTypeInfo.hpp"
#include "AssignCommand.hpp"
#include <vector>

namespace RTT
{
    namespace internal {

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
        mutable typename DataSource<T>::value_t mdata;

    public:
        /**
         * Use shared_ptr.
         */
        ~ValueDataSource();

        typedef boost::intrusive_ptr<ValueDataSource<T> > shared_ptr;

        ValueDataSource( T data );

        ValueDataSource( );

        typename DataSource<T>::result_t get() const
		{
			return mdata;
		}

        typename DataSource<T>::result_t value() const
		{
			return mdata;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return mdata;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return mdata;
		}

        virtual ValueDataSource<T>* clone() const;

        virtual ValueDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const;
    };

    /**
     * Specialisation for std::string to keep capacity when set( ... ) is called.
     */
    template<>
    RTT_API void ValueDataSource<std::string>::set(  AssignableDataSource<std::string>::param_t t );

    /**
     * Specialisation for std::string to keep capacity when clone() is called.
     */
    template<>
    RTT_API ValueDataSource<std::string>::ValueDataSource(std::string t );

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

    public:
        /**
         * Use shared_ptr.
         */
        ~ConstantDataSource();

        typedef boost::intrusive_ptr< ConstantDataSource<T> > shared_ptr;

        ConstantDataSource( T value );

        typename DataSource<T>::result_t get() const
		{
			return mdata;
		}

        typename DataSource<T>::result_t value() const
		{
			return mdata;
		}

        virtual ConstantDataSource<T>* clone() const;

        virtual ConstantDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    };

        /**
         * A DataSource which is used to read a const reference to an
         * external value.
         * @param T The result data type of get().
         */
        template<typename T>
        class ConstReferenceDataSource
            : public DataSource<T>
        {
            // a reference to a value_t
            typename AssignableDataSource<T>::const_reference_t mref;
        public:
            /**
             * Use shared_ptr.
             */
            ~ConstReferenceDataSource();

            typedef boost::intrusive_ptr<ConstReferenceDataSource<T> > shared_ptr;

            ConstReferenceDataSource( typename AssignableDataSource<T>::const_reference_t ref );

            typename DataSource<T>::result_t get() const
            {
                return mref;
            }

            typename DataSource<T>::result_t value() const
            {
                return mref;
            }

            virtual ConstReferenceDataSource<T>* clone() const;

            virtual ConstReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
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
    public:
        /**
         * Use shared_ptr.
         */
        ~ReferenceDataSource();

        typedef boost::intrusive_ptr<ReferenceDataSource<T> > shared_ptr;

        ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref );

        typename DataSource<T>::result_t get() const
		{
			return mref;
		}

        typename DataSource<T>::result_t value() const
		{
			return mref;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return mref;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return mref;
		}

        virtual ReferenceDataSource<T>* clone() const;

        virtual ReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    };

        /**
         * A DataSource which is used to execute an action
         * and then return the value of another DataSource.
         * @param T The result data type of get().
         */
        template<typename T>
        class ActionAliasDataSource
            : public DataSource<T>
        {
            base::ActionInterface* action;
            typename DataSource<T>::shared_ptr alias;
        public:
            typedef boost::intrusive_ptr<ActionAliasDataSource<T> > shared_ptr;

            ActionAliasDataSource(base::ActionInterface* act, DataSource<T>* ds)
            : action(act), alias(ds)
              {}

            ~ActionAliasDataSource() {}

            bool evaluate() const {
                // since get() may return a copy, we override evaluate() to
                // call alias->get() with alias->evaluate().
                action->readArguments();
                bool r = action->execute();
                action->reset();
                // alias may only be evaluated after action was executed.
                alias->evaluate();
                return r;
            }

            typename DataSource<T>::result_t get() const
            {
                action->readArguments();
                action->execute();
                action->reset();
                return alias->get();
            }

            typename DataSource<T>::result_t value() const
            {
                return alias->value();
            }

            virtual void reset() { alias->reset(); }

            virtual ActionAliasDataSource<T>* clone() const {
                return new ActionAliasDataSource(action, alias.get());
            }
            virtual ActionAliasDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                return new ActionAliasDataSource( action->copy(alreadyCloned), alias->copy(alreadyCloned) );
            }
        };


            /**
             * An AssignableDataSource which is used to execute an action
             * and then return the value of another DataSource.
             * @param T The result data type of get().
             */
            template<typename T>
            class ActionAliasAssignableDataSource
                : public AssignableDataSource<T>
            {
                base::ActionInterface* action;
                typename AssignableDataSource<T>::shared_ptr alias;
            public:
                typedef boost::intrusive_ptr<ActionAliasDataSource<T> > shared_ptr;

                ActionAliasAssignableDataSource(base::ActionInterface* act, AssignableDataSource<T>* ds)
                : action(act), alias(ds)
                  {}

                ~ActionAliasAssignableDataSource() {}

                bool evaluate() const {
                    // since get() may return a copy, we override evaluate() to
                    // call alias->get() with alias->evaluate().
                    action->readArguments();
                    bool r = action->execute();
                    action->reset();
                    // alias may only be evaluated after action was executed.
                    alias->evaluate();
                    return r;
                }

                typename DataSource<T>::result_t get() const
                {
                    action->readArguments();
                    action->execute();
                    action->reset();
                    return alias->get();
                }

                typename DataSource<T>::result_t value() const
                {
                    return alias->value();
                }

                void set( typename AssignableDataSource<T>::param_t t ) {
                    alias->set( t );
                }

                typename AssignableDataSource<T>::reference_t set()
                {
                    return alias->set();
                }

                typename AssignableDataSource<T>::const_reference_t rvalue() const
                {
                    return alias->rvalue();
                }

                virtual void reset() { alias->reset(); }

                virtual ActionAliasAssignableDataSource<T>* clone() const {
                    return new ActionAliasAssignableDataSource(action, alias.get());
                }
                virtual ActionAliasAssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                    return new ActionAliasAssignableDataSource( action->copy(alreadyCloned), alias->copy(alreadyCloned) );
                }
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

        base::ActionInterface* updateAction( base::DataSourceBase* other)
        {
            base::DataSourceBase::const_ptr r( other );
            typedef typename AssignableDataSource<T>::copy_t copy_t;
            DataSource< copy_t >* ct = AdaptDataSource<copy_t>()( other );
            if ( ct )
                return new AssignContainerCommand<T,APred,copy_t >( this, ct );

#ifndef ORO_EMBEDDED
            throw bad_assignment();
#else
            return 0;
#endif
        }

        base::ActionInterface* updatePartCommand( base::DataSourceBase* index, base::DataSourceBase* rhs )
        {
            base::DataSourceBase::shared_ptr r( rhs );
            base::DataSourceBase::shared_ptr i( index );
            DataSource<SetType>* t = AdaptDataSource<SetType>()( DataSourceTypeInfo<SetType>::getTypeInfo()->convert(r) );
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
            return new AssignIndexCommand<T, Index, SetType, IPred>( mthis, ind ,t );
        }

        IndexedValueDataSource<T, Index, SetType,IPred,APred>* clone() const
        {
            return new IndexedValueDataSource( this->mdata );
        }

        IndexedValueDataSource<T, Index, SetType,IPred,APred>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace) const
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

        /**
         * A special DataSource only to be used for if you understand
         * the copy()/clone() semantics very well.
         *
         * UnboundDataSource is required to represent variables ('var') in program scripts.
         * Otherwise copy/clone done in ParsedStateMachine will not work as expected.
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

            virtual UnboundDataSource<BoundType>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace) const;
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

      virtual BinaryDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
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

      virtual TernaryDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
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

      virtual SixaryDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
          return new SixaryDataSource<function>( mdsa->copy( alreadyCloned ), mdsb->copy( alreadyCloned ),
                                                 mdsc->copy( alreadyCloned ), mdsd->copy( alreadyCloned ),
                                                 mdse->copy( alreadyCloned ), mdsf->copy( alreadyCloned ), fun );
      }
  };

  /**
   * A DataSource which returns the return value of a unary function.
   * The return value of get() and the input argument are infered from the
   * \a function signature type.
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

    virtual UnaryDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
          return new UnaryDataSource<function>( mdsa->copy( alreadyCloned ), fun );
      }
  };

  /**
   * A generic N-arity composite DataSource. It collects data sources of the same type
   * and passes the values found on to \a function by means of a std::vector.
   * @param function Is a struct which defines value_t operator()(const std::vector<arg_t>&),
   * where \a value_t is defined as \a function::result_type
   * and \a arg_t is defined as \a function::argument_type.
   */
  template<typename function>
  class NArityDataSource
    : public DataSource<typename function::result_type>
  {
      typedef typename function::result_type value_t;
      typedef typename function::argument_type  arg_t;
      mutable std::vector<arg_t> margs;
      std::vector<typename DataSource<arg_t>::shared_ptr > mdsargs;
      function fun;
  public:
      typedef boost::intrusive_ptr<NArityDataSource<function> > shared_ptr;

      /**
       * Create a DataSource which returns the return value of a function
       * \a f.
       */
      NArityDataSource( function f = function() )
          : fun( f )
      {
      }

      /**
       * Create a DataSource which returns the return value of a function
       * \a f.
       */
      NArityDataSource( function f, const std::vector<typename DataSource<arg_t>::shared_ptr >& dsargs )
          : margs( dsargs.size() ), mdsargs(dsargs), fun( f )
      {
      }

      void add( typename DataSource<arg_t>::shared_ptr ds ) {
          mdsargs.push_back(ds);
          margs.push_back( ds->value() );
      }

      virtual value_t get() const
      {
          assert( mdsargs.size() == margs.size() );
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              margs[i] = mdsargs[i]->get();
          return fun( margs );
      }

      virtual value_t value() const
      {
          assert( mdsargs.size() == margs.size() );
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              margs[i] = mdsargs[i]->value();
          return fun( margs ); // fun is allowed to return margs directly.
      }

      virtual void reset()
      {
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              mdsargs[i]->reset();
      }

      virtual NArityDataSource<function>* clone() const
      {
          return new NArityDataSource<function>(fun, mdsargs);
      }

      virtual NArityDataSource<function>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
          std::vector<typename DataSource<arg_t>::shared_ptr > newargs( mdsargs.size() );
          for( unsigned int i=0; i !=mdsargs.size(); ++i)
              newargs[i] = mdsargs[i]->copy(alreadyCloned);
          return new NArityDataSource<function>( fun, newargs );
      }
  };
    }
}

#include "DataSources.inl"

#endif

