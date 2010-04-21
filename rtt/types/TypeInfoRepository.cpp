#include "TypeInfoRepository.hpp"

#include "rtt-config.h"

#include "../Logger.hpp"
#include "TypeTransporter.hpp"
#include "../plugin/TransportPlugin.hpp"
#include "../internal/mystd.hpp"

namespace RTT
{
    using namespace std;
    using namespace detail;

    namespace {
        boost::shared_ptr<TypeInfoRepository> typerepos;
    }

    TypeInfoRepository::TypeInfoRepository()
    {
    }

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
