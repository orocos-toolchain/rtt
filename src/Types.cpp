/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Types.cxx 

                        Types.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "Types.hpp"

#include <rtt-config.h>
#include <corelib.h>

#include <MultiVector.hpp>
#include <Logger.hpp>
#include "Attribute.hpp"
#include "DataSourceAdaptor.hpp"
#include "TemplateTypeInfo.hpp"
#include "TypeInfoName.hpp"
#include <TypeStream.hpp>
#include <mystd.hpp>

#ifdef OROCFG_CORELIB_REALTIME_TOOLKIT_IMPORT
#include <RealTimeToolkit.hpp>
#endif

#include <functional>
#include <sstream>

using namespace std;
using namespace RTT::detail;

#ifdef OROCFG_CORELIB_REALTIME_TOOLKIT_IMPORT
#include <os/StartStopManager.hpp>
namespace RTT {
    namespace {

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
        Logger::log() << Logger::Debug << "Constructor of " << getTypeName() <<Logger::endl;
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

    string TypeInfo::toString( DataSourceBase::shared_ptr in ) const
    {
        stringstream result;
        this->write( result, in );
        return result.str();
    }

    namespace {
        boost::shared_ptr<TypeInfoRepository> typerepos;
    }

    TypeBuilder::~TypeBuilder() {}

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


    bool TypeInfoRepository::addType(TypeInfo* t)
    {
        if ( data.count( t->getTypeName() ) != 0 ) {
            Logger::log() << Logger::Warning << "Attempt to register Type '"<<t->getTypeName() <<"' twice to the Orocos Type System."<<Logger::endl;
        }
        data[ t->getTypeName() ] = t;
        Logger::log() << Logger::Debug << "Registered Type '"<<t->getTypeName() <<"' to the Orocos Type System."<<Logger::endl;
        return true;
    }

    std::vector<std::string> TypeInfoRepository::getTypes() const
    {
        return keys( data );
    }

}
