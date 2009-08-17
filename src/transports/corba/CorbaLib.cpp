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


#include "corba.h"
#ifdef CORBA_IS_TAO
#include "corba.h"
#include <tao/PortableServer/PortableServer.h>
#else
#include <omniORB4/CORBA.h>
#include <omniORB4/poa.h>
#endif

#include "CorbaTemplateProtocol.hpp"
#include "../../plugin/TransportPlugin.hpp"
#include "../../types/Toolkit.hpp"
#include "../../os/StartStopManager.hpp"

namespace RTT {
    using namespace detail;
    namespace corba {

        using namespace RTT::detail;

        /**
         * This protocol is used for all types which did not get a protocol.
         */
        class CorbaFallBackProtocol
            : public TypeTransporter
        {
            bool warn;
        public:
            CorbaFallBackProtocol(bool do_warn = true) : warn(do_warn) {}
            virtual void* createBlob(DataSourceBase::shared_ptr source) const
            {
                if (warn) {
                    Logger::In in("CorbaFallBackProtocol");
                    log(Error) << "Could not send data of type '"<< source->getTypeName()<<"' : data type not known to CORBA Transport." <<Logger::endl;
                }
                return new CORBA::Any();
            }

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateBlob(const void* blob, DataSourceBase::shared_ptr target) const
            {
                if (warn) {
                    Logger::In in("CorbaFallBackProtocol");
                    log(Error) << "Could not update type '"<<target->getTypeName()<<"' with received data : data type not known to CORBA Transport." <<Logger::endl;
                }
                return false;
            }

            /**
             * Create a DataSource which is a proxy for a remote server object.
             * Used to read/write remote attributes, properties and general data over a network.
             */
            virtual DataSourceBase* proxy(void* data ) const {
                DataSourceBase* result = 0;
                corba::Expression_ptr e = static_cast<corba::Expression_ptr>(data);

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
                    return static_cast<Expression_ptr>(corba::ExpressionServer::CreateAssignableExpression( source, p ));
                } else {
                    return corba::ExpressionServer::CreateExpression( source, p );
              }
            }

            /**
             * Create a server for a local method.
             * Used to export local methods to a network.
             */
            virtual void* method(DataSourceBase::shared_ptr source, MethodC* orig, void* arg) const {
                // Return a dumb method server.
                PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
                return corba::ExpressionServer::CreateMethod( source, orig, p );
            }

            /**
             * Narrows a remote data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual DataSourceBase* narrowDataSource(DataSourceBase* dsb)
            {
                if (warn) {
                    log(Warning) << "Corba: Do not know how to narrow to " << dsb->getTypeName() << endlog();
                }
                return 0;
            }

            /**
             * Narrows a remote assignable data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual DataSourceBase* narrowAssignableDataSource(DataSourceBase* dsb)
            {
                if (warn) {
                    log(Warning) << "Corba: Do not know how to narrow to " << dsb->getTypeName() << endlog();
                }
                return 0;
            }

        };

        struct CorbaLibPlugin
            : public TransportPlugin
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
                if ( name == "void" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaFallBackProtocol(false)); // warn=false
                return false;
            }

            std::string getTransportName() const {
                return "CORBA";
            }

            std::string getName() const {
                return "CorbaRealtime";
            }

        } CorbaLibPlugin;

        /**
         * This struct has the sole purpose of invoking
         * the Import function.
         */
        int loadCorbaLib()
        {
            RTT::Toolkit::Import(CorbaLibPlugin);
            // register fallback also.
            DataSourceTypeInfo<UnknownType>::getTypeInfo()->addProtocol( ORO_CORBA_PROTOCOL_ID, new CorbaFallBackProtocol() );
            return 0;
        }

        os::InitFunction CorbaLoader( &loadCorbaLib );
    }


}

