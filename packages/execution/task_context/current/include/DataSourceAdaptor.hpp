#ifndef DATASOURCE_ADAPTOR_HPP
#define DATASOURCE_ADAPTOR_HPP

#include "DataSource.hpp"

namespace ORO_Execution
{

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
     * specialisation in this same file implements (4).
     */
    template<class From, class To>
    struct DataSourceAdaptor
        : public DataSource<To>
    {
        DataSource<From>* orig_;

        DataSourceAdaptor( DataSource<From>* orig)
            : orig_(orig) {}
    
        virtual To get() { return orig_->get(); }

        virtual void reset() { orig_->reset(); }

        virtual void evaluate() { orig_->evaluate(); }

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
            assert( dynamic_cast< DataSourceAdaptor<From,To>* >( i->second ) );
            return static_cast< DataSourceAdaptor<From,To>* >( i->second );
        }
    };

    /**
     * DataSourceAdaptor specialisation to not return a reference to a stack
     * based variable ( case (4) ).
     */
    template<class From>
    struct DataSourceAdaptor<From,const From&>
        : public DataSource<const From&>
    {
        typedef From& To;

        DataSource<From>* orig_;

        DataSourceAdaptor( DataSource<From>* orig)
            : orig_(orig) {}

        From copy;

        virtual To get() { copy = orig_->get(); return copy; }

        virtual void reset() { orig_->reset(); }

        virtual void evaluate() { orig_->evaluate(); }

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
            assert( dynamic_cast< DataSourceAdaptor<From,To>* >( i->second ) );
            return static_cast< DataSourceAdaptor<From,To>* >( i->second );
        }

    };

    /**
     * Try to adapt a DataSourceBase to a DataSource< by value >
     */
    template< class Result >
    struct AdaptDataSource
    {
        DataSource<Result>* operator()( DataSourceBase* dsb) const
        {
            DataSource<Result>* t1 = dynamic_cast< DataSource<Result>*>( dsb );
            if (t1)
                return t1;
            DataSource<const Result&>* t2 = dynamic_cast<DataSource<const Result&>*>( dsb );
            if ( t2 )
                return new DataSourceAdaptor<const Result&, Result>( t2 );
            return 0;
        }
        
    };

    /**
     * Try to adapt a DataSourceBase to a DataSource< by const reference > 
     */
    template<class Result>
    struct AdaptDataSource<const Result&>
    {
        DataSource<const Result&>* operator()( DataSourceBase* dsb) const
        {
            DataSource<const Result&>* t1 = dynamic_cast< DataSource<const Result&>*>( dsb );
            if (t1)
                return t1;
            DataSource<Result>* t2 = dynamic_cast<DataSource<Result>*>( dsb );
            if ( t2 )
                return new DataSourceAdaptor<Result, const Result&>( t2 );
            return 0;
        }
        
    };

}

#endif
