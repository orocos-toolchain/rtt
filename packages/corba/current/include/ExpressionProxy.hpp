#ifndef ORO_CORBA_EXPRESSIONPROXY_HPP
#define ORO_CORBA_EXPRESSIONPROXY_HPP

#include <corelib/DataSource.hpp>
#include "ExecutionC.h"
#include "CORBAExpression.hpp"
#include "corelib/Logger.hpp"
#include <corelib/DataSources.hpp>

namespace ORO_Corba
{

    /**
     * This class manages the access of remote Expression Corba Servers.
     */
    class ExpressionProxy
        : public ORO_CoreLib::DataSourceBase
    {
    protected:
        static std::map<Orocos::Expression_ptr, ExpressionProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ExpressionProxy( ::Orocos::Expression_ptr t );

        Orocos::Expression_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Expression Object.
         * @param expr The Object to connect to.
         * @return A new or previously created CORBA proxy for \a expr.
         */
        static ExpressionProxy* Create(::Orocos::Expression_ptr expr);

        /**
         * Create a DataSource connection to an existing Expression Object.
         * @param T the type of data the Expression supposedly has.
         * @param expr The Object to connect to.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        static ORO_CoreLib::DataSource<T>* CreateDataSource(::Orocos::Expression_ptr expr) {
            using ORO_CoreLib::Logger;
            CORBA::Any_var any = expr->value();
            typename ORO_CoreLib::DataSource<T>::value_t target = typename ORO_CoreLib::DataSource<T>::value_t();
            if ( ORO_CoreLib::AnyConversion<T>::update( any.in(), target ) ) {
                Logger::log() <<Logger::Debug<< "Found valid conversion from server "<< expr->getType()
                              <<" to local "<< ORO_CoreLib::DataSource<T>::GetType()<<Logger::endl;
                return new CORBAExpression<T>( expr );
            } 
            return 0; // not convertible.
        }

        /**
         * Create a ConstantDataSource representing a read-only Any.
         * @param T the type of data the Any supposedly has.
         * @param any The any.
         * @return 0 if the Any is not convertible to T.
         */
        template<class T>
        static ORO_CoreLib::DataSource<T>* CreateConstant( const CORBA::Any& any) {
            // C++ language forces partial T specialisation using classes, not possible
            // with functions:
            return CreateConstantHelper<T>::Create( any );
        }

        /**
         * Create an Orocos AssignableDataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @param expr The Object to connect to.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        static ORO_CoreLib::AssignableDataSource<T>* CreateAssignableDataSource( ::Orocos::Expression_ptr expr) {
            using ORO_CoreLib::Logger;
            Orocos::AssignableExpression_var ret = Orocos::AssignableExpression::_narrow( expr );
            if ( ret ) {
                CORBA::Any_var any = ret->value();
                typename ORO_CoreLib::DataSource<T>::value_t target;
                if ( ORO_CoreLib::AnyConversion<T>::update( any.in(), target ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid assignment conversion from server "<< ret->getType()
                                  <<" to local "<< ORO_CoreLib::DataSource<T>::GetType()<<Logger::endl;
                    return new CORBAAssignableExpression<T>( ret._retn() );
                }
            }
            return 0; // not convertible.
        }
            
        /**
         * Create an Orocos DataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        ORO_CoreLib::DataSource<T>* createDataSource() const {
            return CreateDataSource<T>( mdata );
        }
            
        /**
         * Create an Orocos DataSource<void> proxy.
         * @return A new DataSource.
         */
        ORO_CoreLib::DataSource<void>* createDataSource() const {
            return new CORBAExpression<void>( mdata.in() );
        }
            
        /**
         * Create an Orocos AssignableDataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        ORO_CoreLib::AssignableDataSource<T>* createAssignableDataSource() const {
            return CreateAssignableDataSource<T>( mdata.in() );
        }
            
        /**
         * Get the Corba Object reference of the Expression.
         * This object universally identifies the remote Expression Object
         * and can be used to tell other (remote) objects where to find it.
         */
        //virtual Orocos::Expression_ptr createExpression() const;

        virtual bool hasServer() const
        {
            return true;
        }

        virtual bool evaluate() const {
            return mdata->evaluate();
        }

        virtual ORO_CoreLib::DataSourceBase* clone() const {
            return new ExpressionProxy( mdata.in() );
        }

        virtual ORO_CoreLib::DataSourceBase* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<ExpressionProxy*>(this);
            return alreadyCloned[this];
        }

        virtual std::string getType() const { return std::string( mdata->getType() ); }

        virtual std::string getTypeName() const { return std::string( mdata->getType() ); }

        virtual CORBA::Any* createAny() const {
            return mdata->value();
        }

        virtual CORBA::Any* getAny() const {
            return mdata->get();
        }

        virtual Orocos::Expression_ptr server() { return Orocos::Expression::_duplicate(mdata.in()); }

        virtual Orocos::Expression_ptr server() const { return Orocos::Expression::_duplicate(mdata.in()); }

        virtual Orocos::Method_ptr method() {
            return Orocos::Method::_narrow( mdata.in() );
        }

    private:
        template<class T>
        struct CreateConstantHelper
        {
            static ORO_CoreLib::DataSource<T>* Create(const CORBA::Any& any) {
                using ORO_CoreLib::Logger;
                typename ORO_CoreLib::DataSource<T>::value_t target = typename ORO_CoreLib::DataSource<T>::value_t();
                if ( ORO_CoreLib::AnyConversion<T>::update( any, target ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid conversion from CORBA::Any "
                                  <<" to local constant "<< ORO_CoreLib::DataSource<T>::GetType()<<Logger::endl;
                    return new ORO_CoreLib::ConstantDataSource<T>( target );
                } 
                return 0; // not convertible.
            }
        };

        template<class T>
        struct CreateConstantHelper<T&>
        {
            static ORO_CoreLib::DataSource<T&>* Create(const CORBA::Any& any) {
                return 0; // not convertible.
            }
        };

        template<class T>
        struct CreateConstantHelper<const T&>
        {
            static ORO_CoreLib::DataSource<const T&>* Create(const CORBA::Any& any) {
                using ORO_CoreLib::Logger;
                typename ORO_CoreLib::DataSource<T>::value_t target = typename ORO_CoreLib::DataSource<T>::value_t();
                if ( ORO_CoreLib::AnyConversion<const T&>::update( any, target ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid conversion from CORBA::Any "
                                  <<" to local constant "<< ORO_CoreLib::DataSource<const T&>::GetType()<<Logger::endl;
                    return new ORO_CoreLib::ConstantDataSource<const T&>( target );
                } 
                return 0; // not convertible.
            }
        };

    };

}

#endif
