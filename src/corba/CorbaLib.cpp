/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:22 CET 2008  CorbaLib.cpp 

                        CorbaLib.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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
 
 
#include "CorbaTemplateProtocol.hpp"
#include "os/StartStopManager.hpp"

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
                Corba::CorbaPort* cp = dynamic_cast<Corba::CorbaPort*>( data );
                return new Corba::CorbaDataObjectProxy<detail::UnknownType>("CorbaProxy", cp->getDataChannel());
            }

            virtual DataSourceBase* dataProxy( void* data ) const {
                log(Warning) << "Can not create a proxy for data connection." <<endlog();
                Corba::AssignableExpression_ptr ae = static_cast<Corba::AssignableExpression_ptr>(data);
                return new Corba::CorbaDataObjectProxy<detail::UnknownType>("CorbaProxy", ae );
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
                Corba::CorbaPort* cp = dynamic_cast<Corba::CorbaPort*>( data );
                return new Corba::CorbaBufferProxy<detail::UnknownType>( cp->getBufferChannel() );
            }

            virtual BufferBase* bufferProxy( void* data ) const {
                log(Warning) << "Can not create a proxy for buffer connection." <<endlog();
                Corba::BufferChannel_ptr buf = static_cast<Corba::BufferChannel_ptr>(data);
                return new Corba::CorbaBufferProxy<detail::UnknownType>( buf );
            }

            virtual void* bufferServer( BufferBase::shared_ptr source, void* arg) const 
            {
              // arg is POA !
              log(Warning) << "Can not create a useful server for an unknown data type." << endlog();
              RTT_Corba_BufferChannel_i<UnknownType>* cbuf = new RTT_Corba_BufferChannel_i<UnknownType>( source );
              // activate servant:
              return cbuf->_this();
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
                if ( name == "uint" )
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

