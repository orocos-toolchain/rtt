#include "CorbaTemplateProtocol.hpp"
#include <os/StartStopManager.hpp>

namespace RTT {
    namespace Corba {
        
        using namespace RTT::detail;

        /**
         * This protocol is used for all types which did not get a protocol.
         */
        class CorbaFallBackProtocol
            : public TypeTransporter
        {
            virtual void* createBlob(DataSourceBase::shared_ptr source) const 
            {
                Logger::In in("CorbaFallBackProtocol");
                log(Error) << "Failing Corba::Any creation of type "<< source->getTypeName()<<"." <<Logger::endl;
                return new CORBA::Any();
            }

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateBlob(const void* blob, DataSourceBase::shared_ptr target) const 
            {
                Logger::In in("CorbaFallBackProtocol");
                log(Debug) << "Failing conversion of type "<<target->getTypeName()<<"." <<Logger::endl;
                return false; 
            }

            /**
             * Create a DataSource which is a proxy for a remote server object.
             * Used to read/write remote attributes, properties and general data over a network.
             */
            virtual DataSourceBase* proxy(void* data ) const {
                DataSourceBase* result = 0;
                Corba::Expression_ptr e = static_cast<Corba::Expression_ptr>(data);
                
                // return a dumb proxy.
                result = ExpressionProxy::Create( e ).get();
                return result;
            }

            /**
             * Create a server for a DataSource, which can be picked up by a proxy.
             * Used to export local data to a network.
             */
            virtual void* server(DataSourceBase::shared_ptr source, bool assignable, void* arg) const 
            {
                // Return a dumb server, it will return empty any's using the methods above.
                PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
                if (assignable){
                    return static_cast<Expression_ptr>(Corba::ExpressionServer::CreateAssignableExpression( source, p ));
                } else {
                    return Corba::ExpressionServer::CreateExpression( source, p );
              }
            }

            /**
             * Create a server for a local method.
             * Used to export local methods to a network.
             */
            virtual void* method(DataSourceBase::shared_ptr source, MethodC* orig, void* arg) const {
                // Return a dumb method server.
                PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
                return Corba::ExpressionServer::CreateMethod( source, orig, p );
            }

            /**
             * Returns a new DataObject<T> object mirroring a remote data object server.
             * Used to setup a Corba Data Flow.
             */
            virtual DataSourceBase* dataProxy( PortInterface* data ) const {
                log(Warning) << "Can not create a proxy for data connection of remote port "<<data->getName() <<endlog();
                return 0; // ConnectionFactory checks for this, you should too !
            }

            virtual void* dataServer( DataSourceBase::shared_ptr source, void* arg) const {
                return this->server(source, true, arg );
            }

            /**
             * Returns a new BufferInterface<T> object mirroring a remote buffer object server.
             * Used to setup a Corba Data Flow.
             */
            virtual BufferBase* bufferProxy( PortInterface* data ) const {
                log(Warning) << "Can not create a proxy for buffer connection of remote port "<<data->getName() <<endlog();
                return 0; // ConnectionFactory checks for this, you should too !
            }

            virtual void* bufferServer( BufferBase::shared_ptr source, void* arg) const 
            {
              // arg is POA !
              log(Warning) << "Can not create a useful server for an unknown data type." << endlog();
              RTT_Corba_BufferChannel_i<UnknownType>* cbuf = new RTT_Corba_BufferChannel_i<UnknownType>( source );
              // activate servant:
              CORBA::Object_var ret = cbuf->_this();
              return ret._retn();
                
            }

            /**
             * Narrows a remote data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual DataSourceBase* narrowDataSource(DataSourceBase* dsb)
            {
                log(Warning) << "Corba: Do not know how to narrow to " << dsb->getTypeName() << endlog();
                return 0;
            }

            /**
             * Narrows a remote assignable data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual DataSourceBase* narrowAssignableDataSource(DataSourceBase* dsb)
            {
                log(Warning) << "Corba: Do not know how to narrow to " << dsb->getTypeName() << endlog();
                return 0;
            }

        };

        struct CorbaLibRegistrator
            : public TransportRegistrator
        {
            bool registerTransport(std::string name, TypeInfo* ti)
            {
                assert( name == ti->getTypeName() );
                if ( name == "int" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<int>() );
                if ( name == "double" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<double>() );
                if ( name == "string" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<std::string>() );
                if ( name == "float" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<float>() );
                if ( name == "unsigned int" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<unsigned int>() );
                //if ( name == "long" )
                //    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<long>() );
                if ( name == "PropertyBag" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<PropertyBag>() );
                if ( name == "bool" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<bool>() );
                if ( name == "array" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol< std::vector<double> >() );
                return false;
            }

            std::string getTransportName() const {
                return "CORBA";
            }

        } CorbaLibRegistrator;

        /**
         * This struct has the sole purpose of invoking
         * the Import function.
         */
        int loadCorbaLib()
        {
            log(Info) << "Loading CorbaLib in RTT type system." <<endlog();
            TypeInfoRepository::Instance()->registerTransport( &CorbaLibRegistrator );
            // register fallback.
            DataSourceTypeInfo<UnknownType>::getTypeInfo()->addProtocol( ORO_CORBA_PROTOCOL_ID, new CorbaFallBackProtocol() );
            return 0;
        }

        OS::InitFunction CorbaLoader( &loadCorbaLib );
    }

    
}

