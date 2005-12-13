/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  DataSourceAdaptor.hpp 

                        DataSourceAdaptor.hpp -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef DATASOURCE_ADAPTOR_HPP
#define DATASOURCE_ADAPTOR_HPP

#include "DataSource.hpp"

namespace ORO_Execution
{
    namespace detail {
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
     *            |/        |/   (copy to stack)   (copy to heap)
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

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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

        virtual typename AssignableDataSource<To>::reference_t set() { return orig_->get(); }

        virtual void set(typename AssignableDataSource<To>::param_t v) { orig_->get() = v; }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual AssignableDataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual AssignableDataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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
        : public DataSource<TFrom>
    {
        typedef const TFrom& From;
        typedef const TFrom  To;
        typename DataSource<From>::shared_ptr orig_;

        DataSourceAdaptor( typename DataSource<From>::shared_ptr orig)
            : orig_(orig) {}
    
        virtual typename DataSource<To>::result_t  get() const { return orig_->get(); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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

        virtual typename AssignableDataSource<To>::reference_t set() { return orig_->get(); }

        virtual void set(typename AssignableDataSource<To>::param_t v) { return orig_->get() = v; }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual AssignableDataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual AssignableDataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new DataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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
     */
    template<class From, class To>
    struct AssignableDataSourceAdaptor
        : public DataSource<To>
    {
        typename AssignableDataSource<From>::shared_ptr orig_;

        AssignableDataSourceAdaptor( typename AssignableDataSource<From>::shared_ptr orig)
            : orig_(orig) {}

        virtual typename DataSource<To>::result_t get() const { return orig_->set(); }

        virtual void reset() { orig_->reset(); }

        virtual bool evaluate() const { return orig_->evaluate(); }

        virtual DataSource<To>* clone() const {
            return new AssignableDataSourceAdaptor( orig_->clone() );
        }

        virtual DataSource<To>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
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

        DataSource<Result>* operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            DataSource<Result>* t1 = dynamic_cast< DataSource<Result>*>( dsb.get() );
            if (t1)
                return t1;

            // const ref to value case
            DataSource<const Result&>* t2 = dynamic_cast<DataSource<const Result&>*>( dsb.get() );
            if ( t2 )
                return new detail::DataSourceAdaptor<const Result&, Result>( t2 );

            // ref to value case
            DataSource<Result&>* t3 = dynamic_cast<DataSource<Result&>*>( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<Result&, Result>( t3 );

            // const value to value case
            DataSource<const Result>* t4 = dynamic_cast<DataSource<const Result>*>( dsb.get() );
            if ( t4 )
                return new detail::DataSourceAdaptor<const Result, Result>( t4 );

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

        AssignableDataSource<Result>* operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            AssignableDataSource<Result>* t1 = dynamic_cast< AssignableDataSource<Result>*>( dsb.get() );
            if (t1)
                return t1;

            // ref to assignable value case
            DataSource<Result&>* t3 = dynamic_cast<DataSource<Result&>*>( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<Result&, Result>( t3 ); // will return AssignableDS !

            // complete type failure.
            return 0;
        }
        
    };

    /**
     * Try to adapt a DataSourceBase to an  AssignableDataSource< by reference >.
     * Needed to avoid reference-to-reference dynamic_cast.
     */
    template< class TResult >
    struct AdaptAssignableDataSource< TResult& >
    {
        typedef TResult& Result;

        AssignableDataSource<Result>* operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            AssignableDataSource<Result>* t1 = dynamic_cast< AssignableDataSource<Result>*>( dsb.get() );
            if (t1)
                return t1;

            // ref to assignable value case
            DataSource<Result>* t3 = dynamic_cast<DataSource<Result>*>( dsb.get() );
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

        DataSource<Result>* operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            DataSource<Result>* t1 = dynamic_cast< DataSource<Result>*>( dsb.get() );
            if (t1)
                return t1;

            // const ref to const value case
            DataSource<const TResult&>* t2 = dynamic_cast<DataSource<const TResult&>*>( dsb.get() );
            if ( t2 )
                return new detail::DataSourceAdaptor<const TResult&, Result>( t2 );

            // ref to const value case
            DataSource<TResult&>* t3 = dynamic_cast<DataSource<TResult&>*>( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<TResult&, Result>( t3 );

            // value to const value case
            DataSource<TResult>* t4 = dynamic_cast<DataSource<TResult>*>( dsb.get() );
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

        DataSource<Result>* operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            DataSource<Result>* t1 = dynamic_cast< DataSource<Result>*>( dsb.get() );
            if (t1)
                return t1;

            // assignable case
            AssignableDataSource<TResult>* t2 = dynamic_cast< AssignableDataSource<TResult>* >( dsb.get() );
            if (t2 && &(t2->set()) != 0 )
                return new detail::AssignableDataSourceAdaptor<TResult, TResult&>( t2 );
            
            // complete type failure.
            return 0;
        }
        
    };

    /**
     * Try to adapt a DataSourceBase to a DataSource< by const reference > 
     * Allows all conversions.
     */
    template<class TResult>
    struct AdaptDataSource<const TResult&>
    {
        DataSource<const TResult&>* operator()( DataSourceBase::shared_ptr dsb) const
        {
            // equal case
            DataSource<const TResult&>* t1 = dynamic_cast< DataSource<const TResult&>*>( dsb.get() );
            if (t1)
                return t1;

            // value to const ref case
            DataSource<TResult>* t2 = dynamic_cast<DataSource<TResult>*>( dsb.get() );
            if ( t2 )
                return new detail::DataSourceAdaptor<TResult, const TResult&>( t2 );

            // ref to const ref case
            DataSource<TResult&>* t3 = dynamic_cast<DataSource<TResult&>*>( dsb.get() );
            if ( t3 )
                return new detail::DataSourceAdaptor<TResult&, const TResult&>( t3 );

            // const value to const ref case
            DataSource<const TResult>* t4 = dynamic_cast<DataSource<const TResult>*>( dsb.get() );
            if ( t4 )
                return new detail::DataSourceAdaptor<const TResult, const TResult&>( t4 );

            // assignable case
            AssignableDataSource<TResult>* ta1 = dynamic_cast< AssignableDataSource<TResult>* >( dsb.get() );
            if (ta1 && &(ta1->set()) != 0 ) // check for null set()
                return new detail::AssignableDataSourceAdaptor<TResult, const TResult&>( ta1 );
            

            // complete type failure.
            return 0;
        }
        
    };

}

#endif
