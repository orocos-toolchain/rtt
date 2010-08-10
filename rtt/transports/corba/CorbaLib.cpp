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
#include "../../types/TransportPlugin.hpp"
#include "../../types/TypekitPlugin.hpp"

using namespace std;
using namespace RTT::detail;

namespace RTT {
    namespace corba {

        /**
         * This protocol is used for all types which did not get a protocol.
         * Specifically, if the type is UnknownType.
         */
        class CorbaFallBackProtocol
            : public CorbaTypeTransporter
        {
            bool warn;
        public:
            CorbaFallBackProtocol(bool do_warn = true) : warn(do_warn) {}
            virtual CORBA::Any* createAny(DataSourceBase::shared_ptr source) const
            {
                if (warn) {
                    Logger::In in("CorbaFallBackProtocol");
                    log(Error) << "Could not send data of type '"<< source->getTypeName()<<"' : data type not known to CORBA Transport." <<Logger::endl;
                }
                return new CORBA::Any();
            }

            virtual bool updateAny( base::DataSourceBase::shared_ptr source, CORBA::Any& any) const
            {
                if (warn) {
                    Logger::In in("CorbaFallBackProtocol");
                    log(Error) << "Could not send data of type '"<< source->getTypeName()<<"' : data type not known to CORBA Transport." <<Logger::endl;
                }
                return false;
            }

            virtual base::DataSourceBase::shared_ptr createDataSource(const CORBA::Any* any) const
            {
                return base::DataSourceBase::shared_ptr();
            }

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateFromAny(const CORBA::Any* blob, DataSourceBase::shared_ptr target) const
            {
                if (warn) {
                    Logger::In in("CorbaFallBackProtocol");
                    log(Error) << "Could not update type '"<<target->getTypeName()<<"' with received data : data type not known to CORBA Transport." <<Logger::endl;
                }
                return false;
            }

            virtual ChannelElementBase* createStream(base::PortInterface* port, const ConnPolicy& policy, bool is_sender) const {
                Logger::In in("CorbaFallBackProtocol");
                log(Error) << "Could create Stream for port '"<<port->getName()<<"' : data type not known to CORBA Transport." <<Logger::endl;
                return 0;
            }

            virtual base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy) const { return 0; }

            virtual CRemoteChannelElement_i* createChannelElement_i(DataFlowInterface*, ::PortableServer::POA* poa, bool) const {
                Logger::In in("CorbaFallBackProtocol");
                log(Error) << "Could create Channel : data type not known to CORBA Transport." <<Logger::endl;
                return 0;

            }

            virtual base::ChannelElementBase* buildChannelOutput(base::InputPortInterface& port,
                ConnPolicy const& policy) const {
                Logger::In in("CorbaFallBackProtocol");
                log(Error) << "Could create outputHalf for port "<<port.getName()<<": data type not known to CORBA Transport." <<Logger::endl;
                return 0;
            }

            virtual base::ChannelElementBase* buildChannelInput(base::OutputPortInterface& port,
                ConnPolicy const& policy) const {
                Logger::In in("CorbaFallBackProtocol");
                log(Error) << "Could create outputHalf for port "<<port.getName()<<": data type not known to CORBA Transport." <<Logger::endl;
                return 0;
            }
          virtual base::DataSourceBase::shared_ptr createPropertyDataSource(CService_ptr serv, const std::string& vname) {
              CORBA::String_var tname = serv->getPropertyTypeName( CORBA::string_dup(vname.c_str()));
              log(Warning) << "Corba: Remote property '"<< vname << "' has unknown type " << tname.in()  << endlog();
              return base::DataSourceBase::shared_ptr( );
          }

          virtual base::DataSourceBase::shared_ptr createAttributeDataSource(CService_ptr serv, const std::string& vname) {
              CORBA::String_var tname = serv->getAttributeTypeName( CORBA::string_dup( vname.c_str()));
              log(Warning) << "Corba: Remote attribute '"<< vname << "' has unknown type " << tname.in()  << endlog();
              return base::DataSourceBase::shared_ptr( );
          }
        };

        struct CorbaLibPlugin: public TransportPlugin
        {
            bool registerTransport(std::string name, TypeInfo* ti)
            {
                assert( name == ti->getTypeName() );
                if ( name == "unknown_t") // register fallback also.
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaFallBackProtocol());
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
//                if ( name == "PropertyBag" )
//                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<PropertyBag>() );
                if ( name == "bool" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<bool>() );
                if ( name == "array" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol< std::vector<double> >() );
                if ( name == "void" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaFallBackProtocol(false)); // warn=false
                if ( name == "ConnPolicy")
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<ConnPolicy>() );
                return false;
            }

            std::string getTransportName() const {
                return "CORBA";
            }

            std::string getTypekitName() const {
                return "rtt-types";
            }

            std::string getName() const {
                return "rtt-corba-types";
            }

        };
    }
}

ORO_TYPEKIT_PLUGIN( RTT::corba::CorbaLibPlugin )
