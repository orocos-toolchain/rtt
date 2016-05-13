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

#include "mystd.hpp"
#include "DataSource.hpp"
#include "DataSourceTypeInfo.hpp"
#include "Reference.hpp"
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

        typename DataSource<T>::const_reference_t rvalue() const
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
            typename DataSource<T>::const_reference_t mref;
        public:
            /**
             * Use shared_ptr.
             */
            ~ConstReferenceDataSource();

            typedef boost::intrusive_ptr<ConstReferenceDataSource<T> > shared_ptr;

            ConstReferenceDataSource( typename DataSource<T>::const_reference_t ref );

            typename DataSource<T>::result_t get() const
            {
                return mref;
            }

            typename DataSource<T>::result_t value() const
            {
                return mref;
            }

            typename DataSource<T>::const_reference_t rvalue() const
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
	    : public AssignableDataSource<T>, public Reference
    {
        // a pointer to a value_t
        T* mptr;
    public:
        /**
         * Use shared_ptr.
         */
        ~ReferenceDataSource();

        typedef boost::intrusive_ptr<ReferenceDataSource<T> > shared_ptr;

        ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref );

        void setReference(void* ref)
        {
            mptr = static_cast<T*>(ref);
        }
        bool setReference(base::DataSourceBase::shared_ptr dsb)
        {
            typename AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast<AssignableDataSource<T> >(dsb);
            if (ads) {
		    ads->evaluate();
		    mptr = &ads->set();
		    return true;
	    } else {
		    return false;
	    }
        }

        typename DataSource<T>::result_t get() const
		{
			return *mptr;
		}

        typename DataSource<T>::result_t value() const
		{
			return *mptr;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return *mptr;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return *mptr;
		}

        virtual ReferenceDataSource<T>* clone() const;

        virtual ReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    };

        /**
         * A DataSource which is used to mirror another
         * datasource. Used to strip the 'assignable'
         * property of a data source.
         * @param T The result data type of get().
         */
        template<typename T>
        class AliasDataSource
            : public DataSource<T>
        {
            typename DataSource<T>::shared_ptr alias;
        public:
            typedef boost::intrusive_ptr<AliasDataSource<T> > shared_ptr;

            AliasDataSource(DataSource<T>* ds)
            : alias(ds)
              {}

            ~AliasDataSource() { }

            bool evaluate() const {
                return alias->evaluate();
            }

            typename DataSource<T>::result_t get() const
            {
                return alias->get();
            }

            typename DataSource<T>::result_t value() const
            {
                return alias->value();
            }

            typename DataSource<T>::const_reference_t rvalue() const
            {
                return alias->rvalue();
            }

            virtual void reset() { alias->reset(); }

            virtual AliasDataSource<T>* clone() const {
                return new AliasDataSource(alias.get());
            }
            virtual AliasDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                return new AliasDataSource(alias->copy(alreadyCloned) );
            }
        };


    /**
     * A DataSource that holds a fixed size array,
     * using the types::carray class.
     * @param T A carray<U>
     */
    template<typename T>
    class ArrayDataSource
        : public AssignableDataSource<T>
    {
    protected:
        typename T::value_type* mdata;
        T marray;

    public:
        /**
         * Use shared_ptr.
         */
        ~ArrayDataSource();

        typedef boost::intrusive_ptr<ArrayDataSource<T> > shared_ptr;

        /**
         * Create and allocate an Array of a fixed size.
         * In case you create an empty/default ArrayDataSource, you can
         * assign it an array size later-on with newArray( size )
         */
        ArrayDataSource( std::size_t size = 0);

        /**
         * Creates an ArrayDataSource and initializes the array
         * with the contents of another carray.
         * A deep copy is made from odata.
         */
        ArrayDataSource( T const& odata );
        /**
         * Clears the array of this data source and creates a new one.
         * Note that all references to this array will become invalid
         * (types::carray may make shallow copies!) so only use this
         * if you are sure no other object has a reference to the
         * contained array.
         */
        void newArray( std::size_t size );

        typename DataSource<T>::result_t get() const
		{
			return marray;
		}

        typename DataSource<T>::result_t value() const
		{
			return marray;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return marray;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return marray;
		}

        virtual ArrayDataSource<T>* clone() const;

        virtual ArrayDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const;
    };

        /**
         * A DataSource which is used to manipulate a reference to an
         * external value, by means of a pointer, which can be set after
         * the data source was created. It's the responsibility of the creator
         * of this object that the data source is not used before the pointer
         * is set using setPointer.
         * @param T The result data type of get().
         */
        template<typename T>
        class LateReferenceDataSource
            : public AssignableDataSource<T>
        {
            // a reference to a value_t
            typename AssignableDataSource<T>::value_t* mptr;
        public:

            typedef boost::intrusive_ptr<LateReferenceDataSource<T> > shared_ptr;

            LateReferenceDataSource( typename AssignableDataSource<T>::value_t* ptr = 0)
            :mptr(ptr) {}

            void setPointer( typename AssignableDataSource<T>::value_t* ptr ) {
                mptr = ptr;
            }

            typename DataSource<T>::result_t get() const
            {
                return *mptr;
            }

            typename DataSource<T>::result_t value() const
            {
                return *mptr;
            }

            void const* getRawDataConst()
            {
                return mptr;
            }
            void* getRawData()
            {
                return mptr;
            }

            void set( typename AssignableDataSource<T>::param_t t ) {
                *mptr = t;
            }

            typename AssignableDataSource<T>::reference_t set()
            {
                return *mptr;
            }

            typename AssignableDataSource<T>::const_reference_t rvalue() const
            {
                return *mptr;
            }

            virtual LateReferenceDataSource<T>* clone() const {
                return new LateReferenceDataSource<T>( mptr );
            }

            virtual LateReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>&  ) const {
                return const_cast<LateReferenceDataSource<T>* >(this);
            }
        };

            /**
             * A DataSource which is used to manipulate a const reference to an
             * external value, by means of a pointer, which can be set after
             * the data source was created. It's the responsibility of the creator
             * of this object that the data source is not used before the pointer
             * is set using setPointer.
             * @param T The result data type of get().
             */
            template<typename T>
            class LateConstReferenceDataSource
                : public DataSource<T>
            {
                // a reference to a value_t
                const typename DataSource<T>::value_t* mptr;
            public:

                typedef boost::intrusive_ptr<LateConstReferenceDataSource<T> > shared_ptr;

                LateConstReferenceDataSource(const typename DataSource<T>::value_t* ptr = 0)
                :mptr(ptr) {}

                void setPointer(const typename AssignableDataSource<T>::value_t* ptr ) {
                    mptr = ptr;
                }

                void const* getRawDataConst()
                {
                    return mptr;
                }

                typename DataSource<T>::result_t get() const
                {
                    return *mptr;
                }

                typename DataSource<T>::result_t value() const
                {
                    return *mptr;
                }

                typename DataSource<T>::const_reference_t rvalue() const
                {
                    return *mptr;
                }

                virtual LateConstReferenceDataSource<T>* clone() const {
                    return new LateConstReferenceDataSource<T>( mptr );
                }

                virtual LateConstReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>&  ) const {
                    return const_cast<LateConstReferenceDataSource<T>* >(this);
                }
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

            ~ActionAliasDataSource() { delete action; }

            bool evaluate() const {
                // since get() may return a copy, we override evaluate() to
                // call alias->get() with alias->evaluate().
                action->readArguments();
                bool r = action->execute();
                // alias may only be evaluated after action was executed.
                alias->evaluate();
                return r;
            }

            typename DataSource<T>::result_t get() const
            {
                action->readArguments();
                action->execute();
                return alias->get();
            }

            typename DataSource<T>::result_t value() const
            {
                return alias->value();
            }

            typename DataSource<T>::const_reference_t rvalue() const
            {
                return alias->rvalue();
            }

            virtual void reset() { action->reset(); alias->reset(); }

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

                ~ActionAliasAssignableDataSource() { delete action;}

                bool evaluate() const {
                    // since get() may return a copy, we override evaluate() to
                    // call alias->get() with alias->evaluate().
                    action->readArguments();
                    bool r = action->execute();
                    // alias may only be evaluated after action was executed.
                    alias->evaluate();
                    return r;
                }

                typename DataSource<T>::result_t get() const
                {
                    action->readArguments();
                    action->execute();
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

                virtual void reset() { action->reset(); alias->reset(); }

                virtual ActionAliasAssignableDataSource<T>* clone() const {
                    return new ActionAliasAssignableDataSource(action, alias.get());
                }
                virtual ActionAliasAssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                    return new ActionAliasAssignableDataSource( action->copy(alreadyCloned), alias->copy(alreadyCloned) );
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
    : public DataSource< typename remove_cr<typename function::result_type>::type >
  {
    typedef typename remove_cr<typename function::result_type>::type value_t;
    typedef typename remove_cr<typename function::first_argument_type>::type  first_arg_t;
    typedef typename remove_cr<typename function::second_argument_type>::type second_arg_t;
    typename DataSource<first_arg_t>::shared_ptr mdsa;
    typename DataSource<second_arg_t>::shared_ptr mdsb;
    function fun;
    mutable value_t mdata;
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
        return mdata = fun( a, b );
      }

    virtual value_t value() const
      {
        return mdata;
      }

    typename DataSource<value_t>::const_reference_t rvalue() const
    {
        return mdata;
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
   * A DataSource which returns the return value of a unary function.
   * The return value of get() and the input argument are infered from the
   * \a function signature type.
   */
  template <typename function>
  class UnaryDataSource
    : public DataSource<typename remove_cr<typename function::result_type>::type>
  {
    typedef typename remove_cr<typename function::result_type>::type value_t;
    typedef typename remove_cr<typename function::argument_type>::type arg_t;
    typename DataSource<arg_t>::shared_ptr mdsa;
    function fun;
    mutable value_t mdata;
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
        return mdata = fun( mdsa->get() );
      }

    virtual value_t value() const
      {
        return mdata;
      }

    typename DataSource<value_t>::const_reference_t rvalue() const
    {
        return mdata;
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
    : public DataSource<typename remove_cr<typename function::result_type>::type>
  {
      typedef typename remove_cr<typename function::result_type>::type value_t;
      typedef typename remove_cr<typename function::argument_type>::type  arg_t;
      mutable std::vector<arg_t> margs;
      std::vector<typename DataSource<arg_t>::shared_ptr > mdsargs;
      function fun;
      mutable value_t mdata;
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
          return mdata = fun( margs );
      }

      virtual value_t value() const
        {
          return mdata;
        }

      typename DataSource<value_t>::const_reference_t rvalue() const
      {
          return mdata;
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

/*
 * Extern template declarations for core data source types
 * (instantiated in DataSources.cpp)
 */
RTT_EXT_IMPL template class RTT::internal::ValueDataSource< bool >;
RTT_EXT_IMPL template class RTT::internal::ConstantDataSource< bool >;
RTT_EXT_IMPL template class RTT::internal::ReferenceDataSource< bool >;
RTT_EXT_IMPL template class RTT::internal::ValueDataSource< std::string >;
RTT_EXT_IMPL template class RTT::internal::ConstantDataSource< std::string >;
RTT_EXT_IMPL template class RTT::internal::ReferenceDataSource< std::string >;

#endif
