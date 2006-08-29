/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  ExpressionProxy.hpp 

                        ExpressionProxy.hpp -  description
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
 
 
#ifndef ORO_CORBA_EXPRESSIONPROXY_HPP
#define ORO_CORBA_EXPRESSIONPROXY_HPP

#include "../DataSource.hpp"
#include "OperationsC.h"
#include "CORBAExpression.hpp"
#include "../Logger.hpp"
#include "../DataSources.hpp"

namespace RTT
{namespace Corba
{

    /**
     * This class manages the access of remote Expression Corba Servers.
     */
    class ExpressionProxy
        : public DataSourceBase
    {
    protected:
        static std::map<Corba::Expression_ptr, ExpressionProxy*> proxies;
        static std::map<Corba::Expression_ptr, DataSourceBase*> dproxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ExpressionProxy( ::RTT::Corba::Expression_ptr t );

        Corba::Expression_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Expression Object.
         * @param expr The Object to connect to.
         * @return A new or previously created CORBA proxy for \a expr.
         */
        static ExpressionProxy* Create(::RTT::Corba::Expression_ptr expr);

        /**
         * Factory method: create a DataSource to an existing Expression Object.
         * @param expr The Object to connect to.
         * @return A new or previously created DataSource for \a expr.
         */
        static DataSourceBase* CreateDataSource(::RTT::Corba::Expression_ptr expr);


        /**
         * Create a DataSource connection to an existing Expression Object.
         * @param T the type of data the Expression supposedly has.
         * @param expr The Object to connect to.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        static DataSource<T>* NarrowDataSource(::RTT::Corba::Expression_ptr expr) {
            
            CORBA::Any_var any = expr->value();
            typename DataSource<T>::value_t target = typename DataSource<T>::value_t();
            if ( AnyConversion<T>::update( any.in(), target ) ) {
                Logger::log() <<Logger::Debug<< "Found valid conversion from server "<< expr->getType()
                              <<" to local "<< DataSource<T>::GetType()<<Logger::endl;
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
        static DataSource<T>* NarrowConstant( const CORBA::Any& any) {
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
        static AssignableDataSource<T>* NarrowAssignableDataSource( ::RTT::Corba::Expression_ptr expr) {
            
            Corba::AssignableExpression_var ret = Corba::AssignableExpression::_narrow( expr );
            if ( ret ) {
                CORBA::Any_var any = ret->value();
                typename DataSource<T>::value_t target = typename DataSource<T>::value_t();
                if ( AnyConversion<T>::update( any.in(), target ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid assignment conversion from server "<< ret->getType()
                                  <<" to local "<< DataSource<T>::GetType()<<Logger::endl;
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
        DataSource<T>* narrowDataSource() const {
            return NarrowDataSource<T>( mdata );
        }
            
        /**
         * Create an Orocos DataSource<void> proxy.
         * @return A new DataSource.
         */
        DataSource<void>* narrowDataSource() const {
            return new CORBAExpression<void>( mdata.in() );
        }
            
        /**
         * Create an Orocos AssignableDataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        AssignableDataSource<T>* narrowAssignableDataSource() const {
            return NarrowAssignableDataSource<T>( mdata.in() );
        }
            
        /**
         * Get the Corba Object reference of the Expression.
         * This object universally identifies the remote Expression Object
         * and can be used to tell other (remote) objects where to find it.
         */
        //virtual Corba::Expression_ptr createExpression() const;

        virtual bool hasServer() const
        {
            return true;
        }

        virtual bool evaluate() const {
            return mdata->evaluate();
        }

        virtual DataSourceBase* clone() const {
            return new ExpressionProxy( mdata.in() );
        }

        virtual DataSourceBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<ExpressionProxy*>(this);
            return alreadyCloned[this];
        }

        virtual std::string getType() const { return std::string( mdata->getType() ); }

        virtual const TypeInfo* getTypeInfo() const { return detail::DataSourceTypeInfo<detail::UnknownType>::getTypeInfo(); }

        virtual std::string getTypeName() const { return std::string( mdata->getTypeName() ); }

        virtual CORBA::Any* createAny() const {
            return mdata->value();
        }

        virtual CORBA::Any* getAny() const {
            return mdata->get();
        }

        virtual Corba::Expression_ptr server(PortableServer::POA_ptr) { return Corba::Expression::_duplicate(mdata.in()); }

        virtual Corba::Expression_ptr server(PortableServer::POA_ptr) const { return Corba::Expression::_duplicate(mdata.in()); }

        virtual Corba::Method_ptr method(PortableServer::POA_ptr) {
            return Corba::Method::_narrow( mdata.in() );
        }

    private:
        template<class T>
        struct CreateConstantHelper
        {
            static DataSource<T>* Create(const CORBA::Any& any) {
                
                typename DataSource<T>::value_t target = typename DataSource<T>::value_t();
                if ( AnyConversion<T>::update( any, target ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid conversion from CORBA::Any "
                                  <<" to local constant "<< DataSource<T>::GetType()<<Logger::endl;
                    return new ConstantDataSource<T>( target );
                } 
                return 0; // not convertible.
            }
        };

        template<class T>
        struct CreateConstantHelper<T&>
        {
            static DataSource<T&>* Create(const CORBA::Any& any) {
                return 0; // not convertible.
            }
        };

        template<class T>
        struct CreateConstantHelper<const T&>
        {
            static DataSource<const T&>* Create(const CORBA::Any& any) {
                
                typename DataSource<T>::value_t target = typename DataSource<T>::value_t();
                if ( AnyConversion<const T&>::update( any, target ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid conversion from CORBA::Any "
                                  <<" to local constant "<< DataSource<const T&>::GetType()<<Logger::endl;
                    return new ConstantDataSource<const T&>( target );
                } 
                return 0; // not convertible.
            }
        };

    };

}}

#endif
