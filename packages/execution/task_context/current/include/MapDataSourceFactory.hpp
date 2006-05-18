/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  MapDataSourceFactory.hpp 

                        MapDataSourceFactory.hpp -  description
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
 
 
#ifndef MAPDATASOURCEFACTORY_HPP
#define MAPDATASOURCEFACTORY_HPP

#include <map>
#include "DataSourceFactoryInterface.hpp"
#include "DataSource.hpp"
#include "corelib/PropertyBag.hpp"

namespace ORO_Execution
{
    /**
     * A utility DataSourceFactory that implements the
     * DataSourceFactoryInterface using a given map of strings to
     * DataSources.
     */
    class MapDataSourceFactory
        : public DataSourceFactoryInterface
    {
    public:
        typedef std::map<std::string, ORO_CoreLib::DataSourceBase::shared_ptr> map_t;
        typedef map_t Map;
        /**
         * Create a DataSourceFactory which creates DataSources 
         * according to the \a map.
         */
        MapDataSourceFactory( const map_t& map, const std::string& description = std::string() );
        ~MapDataSourceFactory();

        std::vector<std::string> getNames() const;
        bool hasMember( const std::string& s ) const;
        std::vector<ArgumentDescription> getArgumentList( const std::string& method ) const;
        int getArity(const std::string& name) const;
        ORO_CoreLib::PropertyBag getArgumentSpec( const std::string& method ) const;
        ORO_CoreLib::DataSourceBase* create( const std::string& name, const ORO_CoreLib::PropertyBag& args ) const;
        ORO_CoreLib::DataSourceBase* create( const std::string& name, const std::vector<ORO_CoreLib::DataSourceBase*>& args ) const;
        ORO_CoreLib::DataSourceBase* create( const std::string& name, const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args ) const;
        std::string getDescription( const std::string& source ) const;
        std::string getResultType( const std::string& source ) const;
    private:
        map_t mmap;
        std::string descr;
    };

 
}

#endif
