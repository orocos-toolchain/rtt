/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Types.cxx

                        Types.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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
#include "Types.hpp"

#include "rtt-config.h"

#include <MultiVector.hpp>
#include <Logger.hpp>
#include "Attribute.hpp"
#include "DataSourceAdaptor.hpp"
#include "TypeTransporter.hpp"
#include "TransportPlugin.hpp"
#include <TypeStream.hpp>
#include <mystd.hpp>

#ifdef OROCFG_CORELIB_REALTIME_TOOLKIT_IMPORT
#include <RealTimeToolkit.hpp>
#include <Toolkit.hpp>
#endif

#include <functional>
#include <sstream>

#ifdef OROCFG_CORELIB_REALTIME_TOOLKIT_IMPORT
#include <os/StartStopManager.hpp>
namespace RTT {
    namespace {
        using namespace detail;

        /**
         * This struct has the sole purpose of invoking
         * the Import function.
         */
        int loadRTT()
        {
            Toolkit::Import( RealTimeToolkit );
            return true;
        }

        OS::InitFunction RTTLoader( &loadRTT );
    }
}
#endif


namespace RTT
{
    using namespace std;
    using namespace detail;

    AttributeBase* TypeInfo::buildVariable(std::string name, int ) const {
        return this->buildVariable(name);
    }

    AttributeBase* TypeInfo::buildConstant(std::string name,DataSourceBase::shared_ptr dsb, int ) const {
        return this->buildConstant(name, dsb );
    }


    TypeInfo::~TypeInfo()
    {
        // cleanup constructors
        Constructors::iterator i= constructors.begin();
        while (i != constructors.end() ) {
            delete (*i);
            ++i;
        }
    }

    DataSourceBase::shared_ptr TypeInfo::construct(const std::vector<DataSourceBase::shared_ptr>& args) const
    {

        DataSourceBase::shared_ptr ds;
        if ( args.empty() ) {
            AttributeBase* ab = this->buildVariable("constructor");
            ds = ab->getDataSource();
            delete ab;
            return ds;
        }

        Constructors::const_iterator i= constructors.begin();
        while (i != constructors.end() ) {
            ds = (*i)->build( args );
            if ( ds )
                return ds;
            ++i;
        }
        return ds;
    }

    void TypeInfo::addConstructor(TypeBuilder* tb) {
        constructors.push_back(tb);
    }

    DataSourceBase::shared_ptr TypeInfo::convert(DataSourceBase::shared_ptr arg) const
    {
        DataSourceBase::shared_ptr ds;
        Constructors::const_iterator i= constructors.begin();
        //log(Info) << getTypeName() << ": trying to convert from " << arg->getTypeName()<<endlog();
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

    string TypeInfo::toString( DataSourceBase::shared_ptr in ) const
    {
#ifdef OS_HAVE_STREAMS
        stringstream result;
        this->write( result, in );
        return result.str();
#else
        return string("(") + in->getTypeInfo()->getTypeName() + ")";
#endif
    }

    bool TypeInfo::fromString( const std::string& value, DataSourceBase::shared_ptr out ) const
    {
        stringstream result(value);
        return this->read( result, out );
    }

    bool TypeInfo::addProtocol(int protocol_id, TypeTransporter* tt)
    {
        transporters.resize(protocol_id + 1);
        if ( transporters[protocol_id] ) {
            log(Error) << "A protocol with id "<<protocol_id<<" was already added for type "<< getTypeName()<<endlog();
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
        if ( protocol_id > int(transporters.size()) || transporters[protocol_id] == 0) {
            if ( DataSourceTypeInfo<UnknownType>::getTypeInfo() != this )
                return DataSourceTypeInfo<UnknownType>::getTypeInfo()->getProtocol( protocol_id );
            else {
                log(Warning) << "A protocol with id "<<protocol_id<<" did not register a fall-back handler for unknown types!"<<endlog();
                return 0; // That transport did not register a fall-back !
            }
        }
        return transporters[protocol_id];
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

    namespace {
        boost::shared_ptr<TypeInfoRepository> typerepos;
    }

    TypeBuilder::~TypeBuilder() {}

    TypeInfoRepository::TypeInfoRepository()
    {
    }

    TypeInfoRepository::shared_ptr types() { return TypeInfoRepository::Instance(); }

    boost::shared_ptr<TypeInfoRepository> TypeInfoRepository::Instance()
    {
        if ( typerepos )
            return typerepos;
        typerepos.reset( new TypeInfoRepository() );

        return typerepos;
    }

    TypeInfo* TypeInfoRepository::type( const std::string& name ) const
    {
        map_t::const_iterator i = data.find( name );
        if ( i == data.end() )
            return 0;
        else return i->second;
    }

    TypeInfoRepository::~TypeInfoRepository()
    {
        map_t::const_iterator i = data.begin();
        for( ; i != data.end(); ++i )
            delete i->second;
    }


	TypeInfo* TypeInfoRepository::getTypeById(std::string type_id_name) const {
		// Ask each type for its type id name.
    	map_t::const_iterator i = data.begin();
		for (; i != data.end(); ++i){
			if (i->second->getTypeIdName() == type_id_name)
				return i->second;
		}
		return 0;
    }
	
    bool TypeInfoRepository::addType(TypeInfo* t)
    {
        std::string tname = t->getTypeName();
        if ( data.count( tname ) != 0 ) {
            Logger::log() << Logger::Warning << "Attempt to register Type '"<<tname <<"' twice to the Orocos Type System."<<Logger::endl;
        }
        data[ tname ] = t;
        Logger::log() << Logger::Debug << "Registered Type '"<<tname <<"' to the Orocos Type System."<<Logger::endl;
        for(Transports::iterator it = transports.begin(); it != transports.end(); ++it)
            if ( (*it)->registerTransport( tname, t) )
                log(Info) << "Registered new '"<< (*it)->getTransportName()<<"' transport for " << tname <<endlog();
        return true;
    }

    std::vector<std::string> TypeInfoRepository::getTypes() const
    {
        return keys( data );
    }

    void TypeInfoRepository::registerTransport( TransportPlugin* tr )
    {
        transports.reserve( transports.size() + 1 );
        transports.push_back( tr );
        // infor transport of existing types.
        map_t::const_iterator i = data.begin();
        for( ; i != data.end(); ++i )
            if ( tr->registerTransport( i->first , i->second ) )
                log(Info) << "Registered new '"<< tr->getTransportName()<<"' transport for " << i->first <<endlog();
    }

    void TypeInfoRepository::logTypeInfo() const
    {
        // dump the names of all known types
        Logger::log() << Logger::Debug << "Types known to the Orocos Type System."<<Logger::endl;
        for(map_t::const_iterator it = data.begin(); it != data.end(); ++it)
        {
            std::vector<int>    transports;
            transports = it->second->getTransportNames();
            Logger::log() << Logger::Debug << "-- " << it->first 
                          << " (" << (*it).second->getTypeName() << ") protocols [";
            for (std::vector<int>::const_iterator   iter=transports.begin();
                 iter != transports.end();
                 ++iter) 
            {
                Logger::log() << *iter;
            }
            Logger::log() << "]" << Logger::endl;
        }
        // dump the names of all known transports
        Logger::log() << Logger::Debug << "Transports known to the Orocos Type System."<<Logger::endl;
        for(Transports::const_iterator it = transports.begin(); it != transports.end(); ++it)
        {
            Logger::log() << Logger::Debug << "-- " << (*it)->getTransportName() << Logger::endl;
        }
    }


}
