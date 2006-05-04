#ifndef ORO_CORBAEXPRESSION_HPP
#define ORO_CORBAEXPRESSION_HPP

#include <corelib/DataSource.hpp>
#include <corelib/Logger.hpp>
#include "CorbaConversion.hpp"


namespace ORO_Corba
{
    using ORO_CoreLib::Logger;
    template<class T>
    class CORBAExpression
        : public ORO_CoreLib::DataSource<T>
    {
        Orocos::Expression_var mexpr;
        mutable typename ORO_CoreLib::DataSource<T>::value_t last_value;
    public:
        CORBAExpression( Orocos::Expression_ptr expr )
            : mexpr( Orocos::Expression::_duplicate( expr ) ), last_value()
        {}

        Orocos::Expression_ptr server()
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        Orocos::Expression_ptr server() const
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        typename ORO_CoreLib::DataSource<T>::result_t value() const {
            return last_value;
        }

        virtual typename ORO_CoreLib::DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            if (ORO_CoreLib::AnyConversion<T>::update( res.in(), last_value ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAExpression to remote value!"<<Logger::endl;
            return last_value;
        }

        virtual ORO_CoreLib::DataSource<T>* clone() const {
            return new CORBAExpression<T>( Orocos::Expression::_duplicate( mexpr.in() ) );
        } 

        virtual ORO_CoreLib::DataSource<T>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<T>*>(this);
            return const_cast<CORBAExpression<T>*>(this);
        }

        virtual std::string getType() const { return  ORO_CoreLib::DataSource<T>::GetType() +"::"+std::string( mexpr->getType() ); }

        virtual bool hasServer() const
        {
            return true;
        }
    };

    /**
     * Specialisation for void DataSources.
     */
    template<>
    class CORBAExpression<void>
        : public ORO_CoreLib::DataSource<void>
    {
        Orocos::Expression_var mexpr;
    public:
        CORBAExpression( Orocos::Expression_ptr expr )
            : mexpr( Orocos::Expression::_duplicate( expr ) )
        {}

        Orocos::Expression_ptr server()
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        Orocos::Expression_ptr server() const
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        void value() const {
        }

        virtual void get() const {
            // need var in order not to leak any...
            CORBA::Any_var res = mexpr->get();
        }

        virtual ORO_CoreLib::DataSource<void>* clone() const {
            return new CORBAExpression<void>( Orocos::Expression::_duplicate( mexpr.in() ) );
        } 

        virtual ORO_CoreLib::DataSource<void>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<void>*>(this);
            return const_cast<CORBAExpression<void>*>(this);
        }

        virtual std::string getType() const { return std::string( mexpr->getType() ) + '/' + ORO_CoreLib::DataSource<void>::GetType(); }

        virtual bool hasServer() const
        {
            return true;
        }
    };

    template<class T>
    class CORBAAssignableExpression
        : public ORO_CoreLib::AssignableDataSource<T>
    {
        Orocos::AssignableExpression_var mexpr;
        mutable typename ORO_CoreLib::DataSource<T>::value_t last_value;
    public:
        CORBAAssignableExpression( Orocos::AssignableExpression_ptr expr )
            : mexpr( Orocos::AssignableExpression::_duplicate(expr) ), last_value()
        {}

        using ORO_CoreLib::AssignableDataSource<T>::server;
        
        Orocos::Expression_ptr server()
        {
            return Orocos::AssignableExpression::_duplicate( mexpr );
        }

        typename ORO_CoreLib::DataSource<T>::result_t value() const {
            return last_value;
        }

        virtual typename ORO_CoreLib::DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            if (ORO_CoreLib::AnyConversion<T>::update( res.in(), last_value ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAAssignableExpression to remote value!"<<Logger::endl;
            return last_value;
        }

        virtual void set( typename ORO_CoreLib::AssignableDataSource<T>::param_t t ) {
            CORBA::Any_var toset = ORO_CoreLib::AnyConversion<T>::createAny(t);
            mexpr->set( toset.in() );
        }

        virtual typename ORO_CoreLib::AssignableDataSource<T>::reference_t set() {
            return last_value;
        }

        virtual void updated()
        {
            CORBA::Any_var toset = ORO_CoreLib::AnyConversion<T>::createAny(last_value);
            mexpr->set( toset.in() );
        }

        using ORO_CoreLib::AssignableDataSource<T>::update;
        virtual bool update(const CORBA::Any& any) {
            return mexpr->set( any );
        }


        virtual ORO_CoreLib::AssignableDataSource<T>* clone() const {
            return new CORBAAssignableExpression<T>( Orocos::AssignableExpression::_duplicate( mexpr.in() ) );
        } 

        virtual ORO_CoreLib::AssignableDataSource<T>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAAssignableExpression<T>*>(this);
            return const_cast<CORBAAssignableExpression<T>*>(this);
        }

        virtual std::string getType() const { return  ORO_CoreLib::DataSource<T>::GetType() +"::"+std::string( mexpr->getType() ); }

        virtual bool hasServer() const
        {
            return true;
        }
    };

}

#endif
