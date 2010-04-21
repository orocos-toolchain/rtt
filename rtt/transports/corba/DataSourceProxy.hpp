#ifndef ORO_CORBA_DATASOURCE_PROXY_HPP
#define ORO_CORBA_DATASOURCE_PROXY_HPP

#include "../../internal/DataSource.hpp"
#include "../../scripting/CommandBinary.hpp"
#include "../../Logger.hpp"
#include "../../base/ActionInterface.hpp"
#include "CorbaConversion.hpp"
#include "CorbaTypeTransporter.hpp"
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
            corba::CServiceProvider_var mserv;
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
            DataSourceProxy( corba::CServiceProvider_ptr s, const std::string& name, bool isproperty )
                : mserv( corba::CServiceProvider::_duplicate( s ) ), mname(name), misproperty(isproperty)
            {
                assert( !CORBA::is_nil(s) );
                types::TypeTransporter* tp = this->getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
                ctp = dynamic_cast<corba::CorbaTypeTransporter*>(tp);
                assert( ctp ); // only call this from CorbaTempateTypeInfo.
                if ( misproperty && ! mserv->hasProperty( CORBA::string_dup(name.c_str())))
                    throw NonExistingDataSource();
                if ( !misproperty && ! mserv->hasAttribute( CORBA::string_dup(name.c_str())))
                    throw NonExistingDataSource();
            }

            typename internal::DataSource<T>::result_t value() const {
                return last_value;
            }

            virtual typename internal::DataSource<T>::result_t get() const {
                CORBA::Any_var res;
                if ( misproperty ) {
                    res = mserv->getProperty( CORBA::string_dup( mname.c_str() ) );
                } else {
                    res = mserv->getAttribute( CORBA::string_dup( mname.c_str() ) );
                }
                internal::ReferenceDataSource<T> rds(last_value);
                rds.ref();
                if ( ctp->updateFromAny(&res.in(),&rds ) == false)
                    Logger::log() <<Logger::Error << "Could not update DataSourceProxy from remote value!"<<Logger::endl;
                return last_value;
            }

            virtual internal::DataSource<T>* clone() const {
                return new DataSourceProxy<T>( corba::CServiceProvider::_duplicate( mserv.in() ), mname, misproperty );
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

            virtual int serverProtocol() const
            {
                return ORO_CORBA_PROTOCOL_ID;
            }
        };

        /**
         * Mirrors a remote assignable value datasource.
         * @todo Replace ValueDataSource when type info
         * decomposition is implemented.
         */
        template<class T>
        class ValueDataSourceProxy
            : public internal::AssignableDataSource<T>
        {
            typedef typename internal::AssignableDataSource<T>::value_t value_t;
            corba::CServiceProvider_var mserv;
            const std::string mname;
            bool misproperty;
            typename internal::AssignableDataSource<value_t>::shared_ptr storage;
            CorbaTypeTransporter* ctp;
            //mutable typename internal::DataSource<T>::value_t last_value;

            /**
             * We need this class such that we can re-use the container
             * template type infos. Once type info decomposition is standardized,
             * this can all go and be replaced by use of last_value.
             */
            struct  UpdatedCommand : public ::RTT::base::ActionInterface
            {
                base::DataSourceBase::shared_ptr mds;
                UpdatedCommand( base::DataSourceBase* ds )
                    :mds(ds)
                {}

                bool execute() {
                    mds->updated();
                    return true;
                }

                void readArguments() {}

                ::RTT::base::ActionInterface* clone() const {
                    return new UpdatedCommand(mds.get());
                }

                ::RTT::base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                    return new UpdatedCommand(mds->copy(alreadyCloned));
                }
            };

        public:
            ValueDataSourceProxy( corba::CServiceProvider_ptr serv, const std::string& name, bool isproperty)
                : mserv( corba::CServiceProvider::_duplicate(serv) ), mname(name), misproperty(isproperty)
            {
                storage = types::BuildType<value_t>::Value();
                assert( serv );
                types::TypeTransporter* tp = this->getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
                ctp = dynamic_cast<corba::CorbaTypeTransporter*>(tp);
                assert(ctp);
                if ( misproperty && !mserv->hasProperty( CORBA::string_dup(name.c_str())) )
                    throw NonExistingDataSource();
                if ( !misproperty && ( !mserv->hasAttribute( CORBA::string_dup(name.c_str()))  || !mserv->isAttributeAssignable( CORBA::string_dup(name.c_str())) ))
                    throw NonExistingDataSource();
                this->get(); // initialize such that value()/rvalue() return a sane value !
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
                    res = mserv->getProperty( CORBA::string_dup( mname.c_str() ) );
                } else {
                    res = mserv->getAttribute( CORBA::string_dup( mname.c_str() ) );
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
                bool res;
                if ( misproperty ) {
                    res = mserv->setProperty( CORBA::string_dup( mname.c_str() ), toset.in() );
                } else {
                    res = mserv->setAttribute( CORBA::string_dup( mname.c_str() ), toset.in() );
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

            using internal::AssignableDataSource<T>::update;

            ::RTT::base::ActionInterface* updateAction( base::DataSourceBase* other)
            {
                ::RTT::base::ActionInterface* ci = storage->updateAction(other);
                if (ci)
                    return new scripting::CommandBinary( ci, new UpdatedCommand( this ) );
                return 0;
            }

            virtual ::RTT::base::ActionInterface* updatePartCommand(base::DataSourceBase* index, base::DataSourceBase* rhs )
            {
                //::RTT::base::ActionInterface* ci = storage->updatePartCommand(index, rhs);
                assert(false);
                //if (ci)
                //    return new scripting::CommandBinary( ci, new UpdatedCommand( this ) );
                return 0;
            }

            virtual internal::AssignableDataSource<T>* clone() const {
                return new ValueDataSourceProxy<T>( corba::CServiceProvider::_duplicate( mserv.in() ), mname, misproperty );
            }

            virtual internal::AssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                alreadyCloned[this] = const_cast<ValueDataSourceProxy<T>*>(this);
                return const_cast<ValueDataSourceProxy<T>*>(this);
            }

            virtual int serverProtocol() const
            {
                return ORO_CORBA_PROTOCOL_ID;
            }
        };

    }
}

#endif
