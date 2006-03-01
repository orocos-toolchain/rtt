/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  MapDataSourceFactory.cxx 

                        MapDataSourceFactory.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#include "execution/MapDataSourceFactory.hpp"
#include "execution/mystd.hpp"

using namespace ORO_std;
using namespace ORO_CoreLib;

namespace ORO_Execution
{

       MapDataSourceFactory::MapDataSourceFactory( const map_t& map, const std::string& d )
           : mmap( map ), descr(d)
        {
        }

        MapDataSourceFactory::~MapDataSourceFactory()
        {
        }

        std::vector<std::string> MapDataSourceFactory::getNames() const
        {
            return ORO_std::keys( mmap );
        }

        bool MapDataSourceFactory::hasMember( const std::string& s ) const
        {
            return mmap.find( s ) != mmap.end();
        }

    int MapDataSourceFactory::getArity( const std::string& ) const
    {
        return 0;
    }

        std::vector<ArgumentDescription> MapDataSourceFactory::getArgumentList( const std::string& method ) const
        {
            // we don't support arguments here...
            return std::vector<ArgumentDescription>();
        }

        PropertyBag MapDataSourceFactory::getArgumentSpec( const std::string& method ) const
        {
            return PropertyBag();
        }

        DataSourceBase* MapDataSourceFactory::create( const std::string& name, const PropertyBag& args ) const
        {
            if ( !args.empty() )
                throw wrong_number_of_args_exception( 0, args.getProperties().size() );
            map_t::const_iterator reti = mmap.find( name );
            if ( reti == mmap.end() )
                throw name_not_found_exception();
            else
                return reti->second.get();
        }

        DataSourceBase* MapDataSourceFactory::create( const std::string& name, const std::vector<DataSourceBase*>& args ) const
        {
            if ( !args.empty() )
                throw wrong_number_of_args_exception( 0, args.size() );
            map_t::const_iterator reti = mmap.find( name );
            if ( reti == mmap.end() )
                throw name_not_found_exception();
            else
                return reti->second.get();
        }

        DataSourceBase* MapDataSourceFactory::create( const std::string& name, const std::vector<DataSourceBase::shared_ptr>& args ) const
        {
            if ( !args.empty() )
                throw wrong_number_of_args_exception( 0, args.size() );
            map_t::const_iterator reti = mmap.find( name );
            if ( reti == mmap.end() )
                throw name_not_found_exception();
            else
                return reti->second.get();
        }

        std::string MapDataSourceFactory::getDescription( const std::string& source ) const
        {
            return descr;
        }

    std::string MapDataSourceFactory::getResultType( const std::string& source ) const {
        map_t::const_iterator reti = mmap.find( source );
        if ( reti == mmap.end() )
            throw name_not_found_exception();
        else
            return reti->second->getType();
    }        

}
