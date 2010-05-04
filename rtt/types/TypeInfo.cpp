#include "TypeInfo.hpp"
#include "TypeBuilder.hpp"
#include "../internal/DataSourceTypeInfo.hpp"
#include "TypeTransporter.hpp"

#include "rtt-config.h"

#include "../Logger.hpp"
#include "../base/AttributeBase.hpp"

#ifdef OS_HAVE_STREAMS
#include <sstream>
#endif

namespace RTT
{
    using namespace std;
    using namespace detail;
    using namespace internal;

    AttributeBase* TypeInfo::buildVariable(std::string name, int ) const {
        return this->buildVariable(name);
    }

    AttributeBase* TypeInfo::buildConstant(std::string name,DataSourceBase::shared_ptr dsb, int ) const {
        return this->buildConstant(name, dsb );
    }


    TypeInfo::~TypeInfo()
    {
        // cleanup transporters
        for (Transporters::iterator i = transporters.begin(); i != transporters.end(); ++i)
            delete *i;

        // cleanup constructors
        for (Constructors::iterator i= constructors.begin(); i != constructors.end(); ++i)
            delete (*i);
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
        if ( arg->getTypeInfo() == this )
            return arg;
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
        if (transporters.size() < static_cast<size_t>(protocol_id + 1))
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
        if ( protocol_id + 1 > int(transporters.size()) || transporters[protocol_id] == 0) {
            if ( DataSourceTypeInfo<UnknownType>::getTypeInfo() != this )
                return DataSourceTypeInfo<UnknownType>::getTypeInfo()->getProtocol( protocol_id );
            else {
                log(Warning) << "A protocol with id "<<protocol_id<<" did not register a fall-back handler for unknown types!"<<endlog();
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

    vector<string> TypeInfo::getPartNames() const
    {
        return vector<string>();
    }

    DataSourceBase::shared_ptr TypeInfo::getPart(DataSourceBase::shared_ptr item, const std::string& part_name) const
    {
        /** ** Strong typed data **
         *
         * for( set i = 0; i <=10; set i = i + 1) {
         *      value[i] = i; // sequence index, runtime structure
         *      value.i  = i; // part name, browse static structure
         * }
         * set frame.pos = vector(a,b,c);     // getPart("pos")
         * set frame.pos[3] = vector(a,b,c);  // getPart("pos")->getPart(3)
         * set frame[3].pos = vector(a,b,c);  // getPart(3)->getPart("pos")
         * set frame[i].pos = vector(a,b,c);  // getPart( $i )->getPart("pos")
         * set frame["tool"].pos = vector(a,b,c); // getPart("tool") xx
         * set frame[arg].pos = vector(a,b,c);// getPart( arg )->getPart("pos")
         */
        log(Debug) <<"No parts registered for "<< getTypeName() <<endlog();
        if ( part_name.empty() )
            return item;
        else
            return DataSourceBase::shared_ptr();
    }

    DataSourceBase::shared_ptr TypeInfo::getPart(DataSourceBase::shared_ptr item, DataSourceBase::shared_ptr id) const
    {
        /** ** Strong typed data **
         *
         * for( set i = 0; i <=10; set i = i + 1) {
         *      value[i] = i; // sequence index, runtime structure
         *      value.i  = i; // part name, browse static structure
         * }
         * set frame.pos = vector(a,b,c);     // getPart("pos")
         * set frame.pos[3] = vector(a,b,c);  // getPart("pos")->getPart(3)
         * set frame[3].pos = vector(a,b,c);  // getPart(3)->getPart("pos")
         * set frame[i].pos = vector(a,b,c);  // getPart( $i )->getPart("pos")
         * set frame["tool"].pos = vector(a,b,c); // getPart("tool") xx
         * set frame[arg].pos = vector(a,b,c);// getPart( arg )->getPart("pos")
         */
        log(Debug) <<"No parts registered for "<< getTypeName() <<endlog();
        return DataSourceBase::shared_ptr();
    }
}
