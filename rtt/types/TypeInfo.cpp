/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  TypeInfo.cpp

                        TypeInfo.cpp -  description
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


#include "TypeInfo.hpp"
#include "TypeConstructor.hpp"
#include "../internal/DataSourceTypeInfo.hpp"
#include "../internal/ConnFactory.hpp"
#include "TypeTransporter.hpp"

#include "rtt-config.h"

#include "../Logger.hpp"
#include "../base/AttributeBase.hpp"

namespace RTT
{
    using namespace std;
    using namespace detail;
    using namespace internal;

    TypeInfo::~TypeInfo()
    {
        // cleanup transporters
        for (Transporters::iterator i = transporters.begin(); i != transporters.end(); ++i)
            delete *i;

        // cleanup constructors
        for (Constructors::iterator i= constructors.begin(); i != constructors.end(); ++i)
            delete (*i);
    }

    std::vector<std::string> TypeInfo::getTypeNames() const
    {
        return mtypenames;
    }

    void TypeInfo::addAlias(const std::string& alias) {
        // only alias new names:
        if ( !alias.empty() && find(mtypenames.begin(), mtypenames.end(), alias) == mtypenames.end() )
            mtypenames.push_back(alias);
    }

    bool TypeInfo::isType(const std::string& name) {
        return (find(mtypenames.begin(), mtypenames.end(), name) != mtypenames.end() );
    }

    base::AttributeBase* TypeInfo::buildVariable(std::string name, int hint) const
    {
        return mdsf ? mdsf->buildVariable(name, hint) : 0;
    }

    DataSourceBase::shared_ptr TypeInfo::construct(const std::vector<DataSourceBase::shared_ptr>& args) const
    {

        DataSourceBase::shared_ptr ds;
        // build default constructible:
        if ( args.empty() ) {
            AttributeBase* ab = this->buildVariable("constructor");
            ds = ab->getDataSource();
            delete ab;
            return ds;
        }

        // return same type if equal:
        if ( args.size() == 1 && args.front()->getTypeInfo() == this )
            return args.front();

        Constructors::const_iterator i= constructors.begin();
        while (i != constructors.end() ) {
            ds = (*i)->build( args );
            if ( ds )
                return ds;
            ++i;
        }
        // returns empty data source to indicate not constructible
        return ds;
    }

    void TypeInfo::addConstructor(TypeConstructor* tb) {
        constructors.push_back(tb);
    }

    DataSourceBase::shared_ptr TypeInfo::convert(DataSourceBase::shared_ptr arg) const
    {
        if ( arg->getTypeInfo() == this )
            return arg;

        //log(Info) << getTypeName() << ": trying to convert from " << arg->getTypeName()<<endlog();
        DataSourceBase::shared_ptr ds;
        Constructors::const_iterator i= constructors.begin();
        while (i != constructors.end() ) {
            ds = (*i)->convert( arg );
            if ( ds ) {
                return ds;
            }
            ++i;
        }
        // if no conversion happend, return arg again.
        return arg;
    }

    bool TypeInfo::addProtocol(int protocol_id, TypeTransporter* tt)
    {
        if (transporters.size() < static_cast<size_t>(protocol_id + 1))
            transporters.resize(protocol_id + 1);
        if ( transporters[protocol_id] ) {
            log(Debug) << "A protocol with id "<<protocol_id<<" was already added for type "<< getTypeName()<<endlog();
            delete tt;
            return false;
        }
        transporters[protocol_id] = tt;
        return true;
    }

    TypeTransporter* TypeInfo::getProtocol(int protocol_id) const
    {
        // if the protocol is unknown to this type, return the protocol of the 'unknown type'
        // type, which is a fallback such that we won't have to return zero, but can
        // gracefully fall-back.
        // In order to not endlessly recurse, we check if we aren't the UnknownType !
        if ( protocol_id + 1 > int(transporters.size()) || transporters[protocol_id] == 0) {
            if ( DataSourceTypeInfo<UnknownType>::getTypeInfo() != this )
                return DataSourceTypeInfo<UnknownType>::getTypeInfo()->getProtocol( protocol_id );
            else {
                log(Warning) << "The protocol with id "<<protocol_id<<" did not register a fall-back handler for unknown types!"<<endlog();
                log(Warning) << "  triggered by: "<< getTypeName() << " which does not have a transport."<<endlog();
                return 0; // That transport did not register a fall-back !
            }
        }
        return transporters[protocol_id];
    }

    bool TypeInfo::hasProtocol(int protocol_id) const
    {
        // if the protocol is unknown to this type, return the protocol of the 'unknown type'
        // type, which is a fallback such that we won't have to return zero, but can
        // gracefully fall-back.
        // In order to not endlessly recurse, we check if we aren't the UnknownType !
        if ( protocol_id + 1 > int(transporters.size()) || transporters[protocol_id] == 0) {
            return false;
        }
        return true;
    }

    std::vector<int> TypeInfo::getTransportNames() const
    {
        std::vector<int>    ret;
        for (size_t i=0; i<transporters.size(); ++i)
        {
            // dump only protocols with an actual transporter
            // NB the transporter does not have a name, so you have to manually
            // match the protocol number to an actual transport
            if (0 != transporters[i])
            {
                ret.push_back(i);
            }
        }
        return ret;
    }


    base::InputPortInterface* TypeInfo::inputPort(std::string const& name) const
    {
        return mconnf ? mconnf->inputPort(name) : 0; 
    }

    base::OutputPortInterface* TypeInfo::outputPort(std::string const& name) const
    {
        return mconnf ? mconnf->outputPort(name) : 0; 
    }

    base::ChannelElementBase::shared_ptr TypeInfo::buildDataStorage(ConnPolicy const& policy) const
    {
        return mconnf ? mconnf->buildDataStorage(policy) : base::ChannelElementBase::shared_ptr();
    }

    base::ChannelElementBase::shared_ptr TypeInfo::buildChannelOutput(base::InputPortInterface& port, ConnPolicy const& policy) const
    {
        return mconnf ? mconnf->buildChannelOutput(port, policy) : base::ChannelElementBase::shared_ptr();
    }

    base::ChannelElementBase::shared_ptr TypeInfo::buildChannelInput(base::OutputPortInterface& port, ConnPolicy const& policy) const
    {
        return mconnf ? mconnf->buildChannelInput(port, policy) : base::ChannelElementBase::shared_ptr();
    }

    internal::SharedConnectionBase::shared_ptr TypeInfo::buildSharedConnection(base::OutputPortInterface *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy) const
    {
        return mconnf ? mconnf->buildSharedConnection(output_port, input_port, policy) : internal::SharedConnectionBase::shared_ptr();
    }
}
