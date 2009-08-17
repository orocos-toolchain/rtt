/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  ExpressionProxy.hpp

                        ExpressionProxy.hpp -  description
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


#ifndef ORO_CORBA_EXPRESSIONPROXY_HPP
#define ORO_CORBA_EXPRESSIONPROXY_HPP

#include "../rtt-config.h"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/PortableServer/PortableServer.h>
#else
#include <omniORB4/poa.h>
#endif
#include "OperationsC.h"
#include "../../internal/DataSource.hpp"
#include "CORBAExpression.hpp"
#include "../../Logger.hpp"
#include "../../internal/DataSources.hpp"

namespace RTT
{namespace corba
{

    /**
     * This class manages the access of remote Expression Corba Servers.
     */
    class RTT_CORBA_API ExpressionProxy
        : public base::DataSourceBase
    {
    public:
        typedef boost::intrusive_ptr<ExpressionProxy> shared_ptr;

        /**
         * Factory method: create a CORBA connection to an existing Expression Object.
         * @param expr The Object to connect to.
         * @return A new or previously created CORBA proxy for \a expr.
         */
        static ExpressionProxy::shared_ptr Create(::RTT::corba::Expression_ptr expr);

        /**
         * Factory method: create a internal::DataSource to an existing Expression Object.
         * @param expr The Object to connect to.
         * @return A new or previously created internal::DataSource for \a expr.
         */
        static base::DataSourceBase::shared_ptr CreateDataSource(::RTT::corba::Expression_ptr expr);


        /**
         * Create a internal::DataSource connection to an existing Expression Object.
         * @param T the type of data the Expression supposedly has.
         * @param expr The Object to connect to.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        static internal::DataSource<T>* NarrowDataSource(::RTT::corba::Expression_ptr expr) {

            CORBA::Any_var any = expr->value();
            typename internal::DataSource<T>::value_t target = typename internal::DataSource<T>::value_t();
            internal::ReferenceDataSource<T> rds( target );
            rds.ref();
            if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &any.in() ) ) {
                Logger::log() <<Logger::Debug<< "Found valid conversion from server "<< expr->getType()
                              <<" to local "<< internal::DataSource<T>::GetType()<<Logger::endl;
                return new CORBAExpression<T>( expr );
            }
            return 0; // not convertible.
        }

        /**
         * Create a internal::ConstantDataSource representing a read-only Any.
         * @param T the type of data the Any supposedly has.
         * @param any The any.
         * @return 0 if the Any is not convertible to T.
         */
        template<class T>
        static internal::DataSource<T>* NarrowConstant( const CORBA::Any& any) {
            // C++ language forces partial T specialisation using classes, not possible
            // with functions:
            return CreateConstantHelper<T>::Create( any );
        }

        /**
         * Create an Orocos internal::AssignableDataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @param expr The Object to connect to.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        static internal::AssignableDataSource<T>* NarrowAssignableDataSource( ::RTT::corba::Expression_ptr expr) {

            corba::AssignableExpression_var ret = corba::AssignableExpression::_narrow( expr );
            if ( !CORBA::is_nil(ret) ) {
                CORBA::Any_var any = ret->value();
                typename internal::DataSource<T>::value_t target = typename internal::DataSource<T>::value_t();
                internal::ReferenceDataSource<T> rds( target );
                rds.ref();
                if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &any.in() ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid assignment conversion from server "<< ret->getType()
                                  <<" to local "<< internal::DataSource<T>::GetType()<<Logger::endl;
                    return new CORBAAssignableExpression<T>( ret._retn() );
                }
            }
            return 0; // not convertible.
        }

        /**
         * Create an Orocos internal::DataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        internal::DataSource<T>* narrowDataSource() const {
            return NarrowDataSource<T>( mdata );
        }

        /**
         * Create an Orocos internal::DataSource<void> proxy.
         * @return A new DataSource.
         */
        internal::DataSource<void>* narrowDataSource() const {
            return new CORBAExpression<void>( mdata.in() );
        }

        /**
         * Create an Orocos internal::AssignableDataSource<T> proxy.
         * @param T the type of data the Expression supposedly has.
         * @return 0 if the Expression is not convertible to T.
         */
        template<class T>
        internal::AssignableDataSource<T>* narrowAssignableDataSource() const {
            return NarrowAssignableDataSource<T>( mdata.in() );
        }

        /**
         * Get the Corba Object reference of the Expression.
         * This object universally identifies the remote Expression Object
         * and can be used to tell other (remote) objects where to find it.
         */
        //virtual corba::Expression_ptr createExpression() const;

        virtual int serverProtocol() const
        {
            return ORO_CORBA_PROTOCOL_ID;
        }

        virtual bool evaluate() const {
            return mdata->evaluate();
        }

        virtual base::DataSourceBase* clone() const {
            return new ExpressionProxy( mdata.in() );
        }

        virtual base::DataSourceBase* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<ExpressionProxy*>(this);
            return alreadyCloned[this];
        }

        virtual std::string getType() const { return std::string( mdata->getType() ); }

        virtual const types::TypeInfo* getTypeInfo() const { return internal::DataSourceTypeInfo<internal::UnknownType>::getTypeInfo(); }

        virtual std::string getTypeName() const { return std::string( mdata->getTypeName() ); }

        virtual void* createBlob(int p) {
            if (p == ORO_CORBA_PROTOCOL_ID)
                return mdata->value();
            return 0;
        }

        virtual void* getBlob(int p) {
            if (p == ORO_CORBA_PROTOCOL_ID)
                return mdata->get();
            return 0;
        }

        virtual void* server(int p, void* arg) {
            if (p == ORO_CORBA_PROTOCOL_ID)
                return corba::Expression::_duplicate(mdata.in());
            return 0;
        }

        virtual void* server(int p, void* arg) const {
            if (p == ORO_CORBA_PROTOCOL_ID)
                return corba::Expression::_duplicate(mdata.in());
            return 0;
        }

        virtual void* method(int p, internal::MethodC* , void* arg) {
            if (p == ORO_CORBA_PROTOCOL_ID)
                return corba::Method::_narrow( mdata.in() );
            return 0;
        }

    private:
        template<class T>
        struct CreateConstantHelper
        {
            static internal::DataSource<T>* Create(const CORBA::Any& any) {

                typename internal::DataSource<T>::value_t target = typename internal::DataSource<T>::value_t();
                internal::ReferenceDataSource<T> rds( target );
                rds.ref();
                if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &any ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid conversion from CORBA::Any "
                                  <<" to local constant "<< internal::DataSource<T>::GetType()<<Logger::endl;
                    return new internal::ConstantDataSource<T>( target );
                }
                return 0; // not convertible.
            }
        };

        template<class T>
        struct CreateConstantHelper<T&>
        {
            static internal::DataSource<T&>* Create(const CORBA::Any& any) {
                return 0; // not convertible.
            }
        };

        template<class T>
        struct CreateConstantHelper<const T&>
        {
            static internal::DataSource<const T&>* Create(const CORBA::Any& any) {

                typename internal::DataSource<T>::value_t target = typename internal::DataSource<T>::value_t();
                internal::ReferenceDataSource<T> rds( target );
                rds.ref();
                if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &any ) ) {
                    Logger::log() <<Logger::Debug<< "Found valid conversion from CORBA::Any "
                                  <<" to local constant "<< internal::DataSource<const T&>::GetType()<<Logger::endl;
                    return new internal::ConstantDataSource<const T&>( target );
                }
                return 0; // not convertible.
            }
        };

    protected:
        typedef std::map<corba::Expression_ptr, ExpressionProxy::shared_ptr> EMap;
        typedef std::map<corba::Expression_ptr, base::DataSourceBase::shared_ptr> DMap;

        static EMap proxies;
        static DMap dproxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ExpressionProxy( ::RTT::corba::Expression_ptr t );

        corba::Expression_var mdata;

    };

}}

#endif
