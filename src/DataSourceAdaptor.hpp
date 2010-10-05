/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  DataSourceAdaptor.hpp

                        DataSourceAdaptor.hpp -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#include "DataSource.hpp"

#ifndef DATASOURCE_ADAPTOR_HPP
#define DATASOURCE_ADAPTOR_HPP

namespace RTT
{
    namespace detail {

        template<class From, class To>
        struct DataSourceAdaptor;

        template<class From, class To>
        struct AssignableDataSourceAdaptor;

    /**
     * Adapt parser DataSource storage type to user type.
     * Rationale : the Parser chooses internally how to pass
     * values from DataSource to DataSource. The user however might
     * choose to receive the result by value or by const reference.
     * The parser thus needs to apply adaptors to convert from value
     * to const reference and vice versa :
     *
     * @verbatim
     *           (1)       (2)        (3)              (4)
     * Parser:  value    constref   constref          value
     *            \/        \/   (copy to stack)   (copy to heap)
     *-----------------------------------------------------------
     * User  :  value    constref    value           constref
     *
     * (note : 2bis, 3bis and 4bis are cases for 'ref' instead of 'const ref')
     * @endverbatim
     *
     * If the internal format is constref and the user chooses value (3)
     * ( very unwise! ) we can make a copy and pass that to the users
     * method. If the internal format is value, and the user
     * chooses constref (4) we can not pass it
     * directly, but must copy it to a heap variable and pass
     * a constref of the heaped variable to the user.
     *
     * A user should always follow the parser's choice, since, not
     * doing so will not save a copy. In the worst case, a memory
     * allocation might follow in (3), when the parser chooses constref
     * for containers, for example.
     *
     * This implementation Adapts cases (1), (2) and (3). A
     * specialisation in this same file implements (4) and (4bis).
     *
     * Specialises for example DataSourceAdaptor<[const &] int, int>
     * DataSourceAdaptor<[const &] int, const int>,
     */
    template<class From, class To>
    struct DataSourceAdaptor
        : public DataSource<To>
    {
        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t  get() const { return orig_->get(); }

        virtual typename DataSource<To>::result_t  value() const { return orig_->value(); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                DataSourceAdaptor<From,To>* n = new DataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef DataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }
    };

#ifndef ORO_EMBEDDED

        /**
         * Adapt from non-const reference-type to value-type, yielding an AssignableDataSource,
         * since references are always assignable.
         * Specialises for example DataSourceAdaptor<int &, int> but not DataSourceAdaptor< const int&, const int > (see below),
         *  DataSourceAdaptor<int &, const int> or  DataSourceAdaptor< const int &, int>
         */
    template<class TFrom>
    struct DataSourceAdaptor<TFrom&, TFrom>
        : public AssignableDataSource<TFrom>
    {
        typedef TFrom& From;
        typedef TFrom  To;
        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t  get() const { return orig_->get(); }

        virtual typename DataSource<To>::result_t  value() const { return orig_->value(); }

        virtual typename AssignableDataSource<To>::const_reference_t rvalue() const { return orig_->value(); }

        virtual typename AssignableDataSource<To>::reference_t set() { return orig_->get(); }

        virtual void set(typename AssignableDataSource<To>::param_t v) { orig_->get() = v; }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual AssignableDataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual AssignableDataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                DataSourceAdaptor<From,To>* n = new DataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef DataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }
    };

        /**
         * Specialises DataSourceAdaptor< const int&, const int >. Needed because
         * otherwise the compiler would choose the above, more general case.
         */
    template<class TFrom>
    struct DataSourceAdaptor<const TFrom&, const TFrom>
        : public DataSource<const TFrom>
    {
        typedef const TFrom& From;
        typedef const TFrom  To;
        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t  get() const { return orig_->get(); }

        virtual typename DataSource<To>::result_t  value() const { return orig_->value(); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                DataSourceAdaptor<From,To>* n = new DataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef DataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }
    };

        /**
         * Adapt from non-const reference-type to non-const reference-type, yielding an AssignableDataSource,
         * since references are always assignable.
         * Specialises for example DataSourceAdaptor<int &, int&> (Rare case).
         */
    template<class TFrom>
    struct DataSourceAdaptor<TFrom&, TFrom&>
        : public AssignableDataSource<TFrom&>
    {
        typedef TFrom& From;
        typedef TFrom& To;
        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t  get() const { return orig_->get(); }

        virtual typename DataSource<To>::result_t  value() const { return orig_->value(); }

        virtual typename AssignableDataSource<To>::const_reference_t rvalue() const { return orig_->value(); }

        virtual typename AssignableDataSource<To>::reference_t set() { return orig_->get(); }

        virtual void set(typename AssignableDataSource<To>::param_t v) { orig_->get() = v; }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual AssignableDataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual AssignableDataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                DataSourceAdaptor<From,To>* n = new DataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef DataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }
    };

#endif

    /**
     * DataSourceAdaptor specialisation to not return a const reference to a stack
     * based variable ( case (4) ).
     * Specialises for example DataSourceAdaptor<int, const int&>
     */
    template<class TFrom>
    struct DataSourceAdaptor<TFrom,const TFrom&>
        : public DataSource<const TFrom&>
    {
        typedef const TFrom& To;
        typedef TFrom  From;

        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        mutable From copy_;  //! Store the result value.

        virtual typename DataSource<To>::result_t get() const { copy_ = orig_->get(); return copy_; }

        virtual typename DataSource<To>::result_t value() const { copy_ = orig_->value(); return copy_; }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                DataSourceAdaptor<From,To>* n = new DataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef DataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }

    };

#ifndef ORO_EMBEDDED

    /**
     * DataSourceAdaptor specialisation to not return a reference to a stack
     * based variable ( case (4bis) ).
     * Specialises for example
     * DataSourceAdaptor<const int, const int&>
     */
    template<class TFrom>
    struct DataSourceAdaptor<const TFrom, const TFrom&>
        : public DataSource<const TFrom&>
    {
        typedef const TFrom& To;
        typedef const TFrom From;

        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        mutable TFrom copy_;  //! Store the result value.

        virtual typename DataSource<To>::result_t get() const { copy_ = orig_->get(); return copy_; }

        virtual typename DataSource<To>::result_t value() const { copy_ = orig_->value(); return copy_; }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                DataSourceAdaptor<From,To>* n = new DataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef DataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }

    };

    /**
     * AssignableDataSourceAdaptor allows a conversion from an AssignableDataSource<T>
     * to DataSource<[const] T [&]> which will return the result of the AssignableDataSource<T>::set()
     * method in its get() (thus a reference to a heaped value).
     *
     * This class always makes a copy of the data during get() or value(). Use updated() if you
     * modified this copy (for example because this class returns a reference to it).
     */
    template<class From, class To>
    struct AssignableDataSourceAdaptor
        : public DataSource<To>
    {
        typename AssignableDataSource<From>::shared_ptr orig_;
        mutable typename DataSource<To>::value_t mcache;

        AssignableDataSourceAdaptor( typename AssignableDataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t get() const { mcache = orig_->get(); return mcache; }

        virtual typename DataSource<To>::result_t value() const { mcache = orig_->value(); return mcache; }

        virtual void updated() { orig_->set( mcache ); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new AssignableDataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                AssignableDataSourceAdaptor<From,To>* n = new AssignableDataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef AssignableDataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }

    };
#endif
    /**
     * AssignableDataSourceAdaptor allows a conversion from an AssignableDataSource<T>
     * to DataSource<T> which will return the result of the AssignableDataSource<T>::rvalue()
     * method in its get() (thus a reference to a heaped value).
     */
    template<class From>
    struct AssignableDataSourceAdaptor<From, const From& >
        : public DataSource<const From&>
    {
        typedef const From& To;
        typename AssignableDataSource<From>::shared_ptr orig_;

        AssignableDataSourceAdaptor( typename AssignableDataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t get() const { return orig_->rvalue(); }

        virtual typename DataSource<To>::result_t value() const { return orig_->rvalue(); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new AssignableDataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                AssignableDataSourceAdaptor<From,To>* n = new AssignableDataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef AssignableDataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }

    };

    /**
     * AssignableDataSourceAdaptor allows a conversion from an AssignableDataSource<const& T>
     * to AssignableDataSource<T>. Properties work with the latter.
     */
    template<class To>
    struct AssignableDataSourceAdaptor<To const&, To>
        : public AssignableDataSource<To>
    {
        typedef To const& From;
        typename AssignableDataSource<From>::shared_ptr orig_;

        AssignableDataSourceAdaptor( typename AssignableDataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t get() const { return orig_->get(); }

        virtual typename DataSource<To>::result_t value() const { return orig_->value(); }

        virtual typename AssignableDataSource<To>::const_reference_t rvalue() const { return orig_->rvalue(); }

        virtual typename AssignableDataSource<To>::reference_t set() { return orig_->set(); }

        virtual void set( typename AssignableDataSource<To>::param_t v) { orig_->set(v); }

        virtual void updated() { orig_->updated(); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual bool updatePart( DataSourceBase* part, DataSourceBase* other ) { return orig_->updatePart( part, other); }

        virtual CommandInterface* updatePartCommand( DataSourceBase* part, DataSourceBase* other) { return orig_->updatePartCommand(part, other ); }

        virtual AssignableDataSource<To>* clone() const {
            return new AssignableDataSourceAdaptor( orig_->clone() );
        }

        virtual AssignableDataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            std::map<const DataSourceBase*,  DataSourceBase*>::iterator i = alreadyCloned.find( this );
            if ( i == alreadyCloned.end() ) {
                AssignableDataSourceAdaptor<From,To>* n = new AssignableDataSourceAdaptor<From,To>( orig_->copy( alreadyCloned) );
                alreadyCloned[this] = n;
                return n;
            }
            typedef AssignableDataSourceAdaptor<From,To> CastType;
            assert( dynamic_cast< CastType* >( i->second ) == static_cast< CastType* >( i->second ) );
            return static_cast< CastType* >( i->second );
        }

    };

    }

    /**
     * Try to adapt a DataSourceBase to a DataSource< by value >.
     */
    template< class TResult >
    struct AdaptDataSource
    {
        typedef TResult Result;

        typename DataSource<Result>::shared_ptr operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            typename DataSource<Result>::shared_ptr t1 = DataSource<Result>::narrow( dsb.get() );
            if (t1)
                return t1;

            // const ref to value case
            typename DataSource<const Result&>::shared_ptr t2 = DataSource<const Result&>::narrow( dsb.get() );
            if ( t2 )
                return new detail::DataSourceAdaptor<const Result&, Result>( t2 );

#ifndef ORO_EMBEDDED
            // ref to value case
            typename DataSource<Result&>::shared_ptr t3 = DataSource<Result&>::narrow( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<Result&, Result>( t3 );

            // const value to value case
            typename DataSource<const Result>::shared_ptr t4 = DataSource<const Result>::narrow( dsb.get() );
            if ( t4 )
                return new detail::DataSourceAdaptor<const Result, Result>( t4 );
#endif

            // complete type failure.
            return 0;
        }

    };

    /**
     * Try to adapt a DataSourceBase to an  AssignableDataSource< by value >.
     */
    template< class TResult >
    struct AdaptAssignableDataSource
    {
        typedef TResult Result;

        typename AssignableDataSource<Result>::shared_ptr operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            typename AssignableDataSource<Result>::shared_ptr t1 = AssignableDataSource<Result>::narrow( dsb.get() );
            if (t1)
                return t1;

#if 0
            // Does this case exist ?
            // Assignable const ref case
            typename AssignableDataSource<const Result&>::shared_ptr t2 = AssignableDataSource<const Result&>::narrow( dsb.get() );
            if ( t2 )
                return new detail::AssignableDataSourceAdaptor<const Result&, Result>( t2 ); // will return AssignableDS !
#endif

#ifndef ORO_EMBEDDED
            // ref to assignable value case
            typename DataSource<Result&>::shared_ptr t3 = DataSource<Result&>::narrow( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<Result&, Result>( t3 ); // will return AssignableDS !
#endif
            // complete type failure.
            return 0;
        }

    };

#ifndef ORO_EMBEDDED
    /**
     * Try to adapt a DataSourceBase to an  AssignableDataSource< by reference >.
     * Needed to avoid reference-to-reference dynamic_cast.
     */
    template< class TResult >
    struct AdaptAssignableDataSource< TResult& >
    {
        typedef TResult& Result;

        typename AssignableDataSource<Result>::shared_ptr operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            typename AssignableDataSource<Result>::shared_ptr t1 = AssignableDataSource<Result>::narrow( dsb.get() );
            if (t1)
                return t1;

            // ref to assignable value case
            typename DataSource<Result>::shared_ptr t3 = DataSource<Result>::narrow( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<Result, Result>( t3 ); // will return AssignableDS !

            // complete type failure.
            return 0;
        }

    };

    /**
     * Try to adapt a DataSourceBase to a DataSource< by const value >.
     */
    template< class TResult >
    struct AdaptDataSource< const TResult >
    {
        typedef const TResult Result;

        typename DataSource<Result>::shared_ptr operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            typename DataSource<Result>::shared_ptr t1 = DataSource<Result>::narrow( dsb.get() );
            if (t1)
                return t1;

            // const ref to const value case
            typename DataSource<const TResult&>::shared_ptr t2 = DataSource<const TResult&>::narrow( dsb.get() );
            if ( t2 )
                return new detail::DataSourceAdaptor<const TResult&, Result>( t2 );

            // ref to const value case
            typename DataSource<TResult&>::shared_ptr t3 = DataSource<TResult&>::narrow( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<TResult&, Result>( t3 );

            // value to const value case
            typename DataSource<TResult>::shared_ptr t4 = DataSource<TResult>::narrow( dsb.get() );
            if ( t4 )
                return new detail::DataSourceAdaptor<TResult, Result>( t4 );

            // complete type failure.
            return 0;
        }

    };

    /**
     * Try to adapt a DataSourceBase to a DataSource< by ref >.
     * Only allow one 'conversion' : ref to ref. All the others are
     * illegal in C++ too.
     */
    template< class TResult >
    struct AdaptDataSource< TResult& >
    {
        typedef TResult& Result;

        typename DataSource<Result>::shared_ptr operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            typename DataSource<Result>::shared_ptr t1 = DataSource<Result>::narrow( dsb.get() );
            if (t1)
                return t1;

            // assignable case
            typename AssignableDataSource<TResult>::shared_ptr t2 = AssignableDataSource<TResult>::narrow( dsb.get() );
            if (t2 && &(t2->set()) != 0 )
                return new detail::AssignableDataSourceAdaptor<TResult, TResult&>( t2 );

            // complete type failure.
            return 0;
        }

    };

#endif

    /**
     * Try to adapt a DataSourceBase to a DataSource< by const reference >
     * Allows all conversions. This one is used for assignments of class types.
     */
    template<class TResult>
    struct AdaptDataSource<const TResult&>
    {
        typename DataSource<const TResult&>::shared_ptr operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            typename DataSource<const TResult&>::shared_ptr t1 = DataSource<const TResult&>::narrow( dsb.get() );
            if (t1)
                return t1;

            // assignable case: this is a more efficient implementation than the one below (t2)
            // does not involve a copy.
            typename AssignableDataSource<TResult>::shared_ptr ta1 =  AssignableDataSource<TResult>::narrow( dsb.get() );
            if (ta1 && &(ta1->set()) != 0 ) // check for null set()
                return new detail::AssignableDataSourceAdaptor<TResult, const TResult&>( ta1 );

            // value to const ref case
            // makes a copy !
            typename DataSource<TResult>::shared_ptr t2 = DataSource<TResult>::narrow( dsb.get() );
            if ( t2 )
                return new detail::DataSourceAdaptor<TResult, const TResult&>( t2 );

#ifndef ORO_EMBEDDED
            // ref to const ref case
            typename DataSource<TResult&>::shared_ptr t3 = DataSource<TResult&>::narrow( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<TResult&, const TResult&>( t3 );

            // const value to const ref case
            typename DataSource<const TResult>::shared_ptr t4 = DataSource<const TResult>::narrow( dsb.get() );
            if ( t4 )
                return new detail::DataSourceAdaptor<const TResult, const TResult&>( t4 );

#endif

            // complete type failure.
            return 0;
        }

    };
}

#endif
