/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  DataSourceProxy.hpp

                        DataSourceProxy.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_CORBA_DATASOURCE_PROXY_HPP
#define ORO_CORBA_DATASOURCE_PROXY_HPP

#include "../../internal/DataSource.hpp"
#include "../../Logger.hpp"
#include "../../base/ActionInterface.hpp"
#include "CorbaConversion.hpp"
#include "CorbaTypeTransporter.hpp"
#include "CorbaLib.hpp"
#include <cassert>

namespace RTT
{
    namespace corba
    {
        /**
         * Gets thrown by the constructor of DataSourceProxy or
         * ValueDataSourceProxy when invalid service or property/attribute
         * was given.
         */
        struct NonExistingDataSource
        {
        };
        /**
         * Mirrors a remote DataSource.
         */
        template<class T>
        class DataSourceProxy
            : public internal::DataSource<T>
        {
            corba::CService_var mserv;
            const std::string mname;
            bool misproperty;
            mutable typename internal::DataSource<T>::value_t last_value;
            CorbaTypeTransporter* ctp;

        public:
            /**
             * Creates a new DataSource proxy for an attribute or property.
             * @param s The service to use
             * @param name The name of attribute or property
             * @param isproperty set to true if name refers to a property, to false if it referes to an attribute.
             *
             * @throw NonExistingDataSource when name does not exist in s as a property (isproperty==true)
             * or attribute (isproperty==false).
             */
            DataSourceProxy( corba::CService_ptr s, const std::string& name, bool isproperty )
                : mserv( corba::CService::_duplicate( s ) ), mname(name), misproperty(isproperty)
            {
                assert( !CORBA::is_nil(s) );
                types::TypeTransporter* tp = this->getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
                ctp = dynamic_cast<corba::CorbaTypeTransporter*>(tp);
                assert( ctp ); // only call this from CorbaTempateTypeInfo.
#if 0
                if ( misproperty && ! mserv->hasProperty( name.c_str()))
                    throw NonExistingDataSource();
                if ( !misproperty && ! mserv->hasAttribute( name.c_str()))
                    throw NonExistingDataSource();
#endif
            }

            typename internal::DataSource<T>::result_t value() const {
                return last_value;
            }

            typename internal::AssignableDataSource<T>::const_reference_t rvalue() const {
                return last_value;
            }

            virtual typename internal::DataSource<T>::result_t get() const {
                CORBA::Any_var res;
                if ( misproperty ) {
                    res = mserv->getProperty( mname.c_str() );
                } else {
                    res = mserv->getAttribute( mname.c_str() );
                }
                internal::ReferenceDataSource<T> rds(last_value);
                rds.ref();
                if ( ctp->updateFromAny(&res.in(),&rds ) == false)
                    Logger::log() <<Logger::Error << "Could not update DataSourceProxy from remote value!"<<Logger::endl;
                return last_value;
            }

            virtual internal::DataSource<T>* clone() const {
                return new DataSourceProxy<T>( corba::CService::_duplicate( mserv.in() ), mname, misproperty );
            }

            virtual internal::DataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                alreadyCloned[this] = const_cast<DataSourceProxy<T>*>(this);
                return const_cast<DataSourceProxy<T>*>(this);
            }

            virtual std::string getType() const {
                // both should be equivalent, but we display the local type.
                return internal::DataSource<T>::GetType();
                //return std::string( mserv->getType() );
            }

        };

        /**
         * Mirrors a remote assignable value datasource.
         */
        template<class T>
        class ValueDataSourceProxy
            : public internal::AssignableDataSource<T>
        {
            typedef typename internal::AssignableDataSource<T>::value_t value_t;
            corba::CService_var mserv;
            const std::string mname;
            bool misproperty;
            typename internal::AssignableDataSource<value_t>::shared_ptr storage;
            CorbaTypeTransporter* ctp;
            //mutable typename internal::DataSource<T>::value_t last_value;

        public:
            ValueDataSourceProxy( corba::CService_ptr serv, const std::string& name, bool isproperty)
                : mserv( corba::CService::_duplicate(serv) ), mname(name), misproperty(isproperty)
            {
                storage = new internal::ValueDataSource<value_t>();
                assert( serv );
                types::TypeTransporter* tp = this->getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
                ctp = dynamic_cast<corba::CorbaTypeTransporter*>(tp);
                assert(ctp);
#if 0
                if ( misproperty && !mserv->hasProperty( name.c_str()) )
                    throw NonExistingDataSource();
                if ( !misproperty && ( !mserv->hasAttribute( name.c_str())  || !mserv->isAttributeAssignable( name.c_str()) ))
                    throw NonExistingDataSource();
                this->get(); // initialize such that value()/rvalue() return a sane value !
#endif
            }

            typename internal::DataSource<T>::result_t value() const {
                return storage->rvalue();
            }

            typename internal::AssignableDataSource<T>::const_reference_t rvalue() const {
                return storage->rvalue();
            }


            virtual typename internal::DataSource<T>::result_t get() const {
                CORBA::Any_var res;
                if ( misproperty ) {
                    res = mserv->getProperty( mname.c_str() );
                } else {
                    res = mserv->getAttribute( mname.c_str() );
                }
                internal::ReferenceDataSource<T> rds( storage->set() );
                rds.ref();
                if ( ctp->updateFromAny(&res.in(), &rds ) == false)
                    Logger::log() <<Logger::Error << "Could not update ValueDataSourceProxy from remote value!"<<Logger::endl;
                return storage->rvalue();
            }

            virtual void set( typename internal::AssignableDataSource<T>::param_t t ) {
                internal::ValueDataSource<T> vds(t);
                vds.ref();
                CORBA::Any_var toset = ctp->createAny(&vds);
                if ( misproperty ) {
                    mserv->setProperty( mname.c_str(), toset.in() );
                } else {
                    mserv->setAttribute( mname.c_str(), toset.in() );
                }
                storage->set( t );
            }

            virtual typename internal::AssignableDataSource<T>::reference_t set() {
                this->get();
                return storage->set();
            }

            virtual void updated()
            {
                this->set( storage->value() );
            }

            virtual internal::AssignableDataSource<T>* clone() const {
                return new ValueDataSourceProxy<T>( corba::CService::_duplicate( mserv.in() ), mname, misproperty );
            }

            virtual internal::AssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                alreadyCloned[this] = const_cast<ValueDataSourceProxy<T>*>(this);
                return const_cast<ValueDataSourceProxy<T>*>(this);
            }
        };

    }
}

#endif
