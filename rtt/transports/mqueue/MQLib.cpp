/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  MQLib.cpp

                        MQLib.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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



#include "MQLib.hpp"
#include "MQTemplateProtocol.hpp"
#include "MQSerializationProtocol.hpp"
#include "../../types/TransportPlugin.hpp"
#include "../../types/TypekitPlugin.hpp"
#include <boost/serialization/vector.hpp>

using namespace std;
using namespace RTT::detail;

namespace RTT {
    namespace mqueue {
        bool MQLibPlugin::registerTransport(std::string name, TypeInfo* ti)
        {
            if ( name == "int" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<int>() );
            if ( name == "double" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<double>() );
            //if ( name == "string" )
            //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<std::string>() );
            if ( name == "float" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<float>() );
            if ( name == "uint" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<unsigned int>() );
            if ( name == "char" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<char>() );
            if ( name == "llong" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<long long>() );
            if ( name == "ullong" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<unsigned long long>() );
            //if ( name == "long" )
            //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<long>() );
            //if ( name == "PropertyBag" )
            //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<PropertyBag>() );
            if ( name == "bool" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<bool>() );
            if ( name == "array" )
                return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQSerializationProtocol< std::vector<double> >() );
            //if ( name == "void" )
            //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQFallBackProtocol(false)); // warn=false
            return false;
        }

        std::string MQLibPlugin::getTransportName() const {
            return "mqueue";
        }

        std::string MQLibPlugin::getTypekitName() const {
            return "rtt-types";
        }
        std::string MQLibPlugin::getName() const {
            return "rtt-mqueue-transport";
        }
    }
}

ORO_TYPEKIT_PLUGIN( RTT::mqueue::MQLibPlugin )
