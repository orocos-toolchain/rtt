#ifndef MAPDATASOURCEFACTORY_HPP
#define MAPDATASOURCEFACTORY_HPP

#include <map>
#include "DataSourceFactoryInterface.hpp"

namespace ORO_Execution
{
        // a utility DataSourceFactory that implements the
        // DataSourceFactoryInterface using a given map of strings to
        // DataSources...
        class MapDataSourceFactory
            : public DataSourceFactoryInterface
        {
            typedef std::map<std::string, DataSourceBase::shared_ptr> map_t;
            map_t mmap;
        public:
            MapDataSourceFactory( const map_t& map );
            ~MapDataSourceFactory();

            std::vector<std::string> getNames() const;
            bool hasMember( const std::string& s ) const;
            std::vector<ArgumentDescription> getArgumentList( const std::string& method ) const;
            PropertyBag getArgumentSpec( const std::string& method ) const;
            DataSourceBase* create( const std::string& name, const PropertyBag& args ) const;
            DataSourceBase* create( const std::string& name, const std::vector<DataSourceBase*>& args ) const;
            // we don't know how to define this here -> relegating to
            // the subclass..
            virtual std::string getDescription( const std::string& source ) const = 0;
        };

 
}

#endif
