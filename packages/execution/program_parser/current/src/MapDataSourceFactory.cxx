#include "execution/MapDataSourceFactory.hpp"


namespace ORO_Execution
{

       MapDataSourceFactory::MapDataSourceFactory( const map_t& map )
            : mmap( map )
        {
        }

        MapDataSourceFactory::~MapDataSourceFactory()
        {
        }

        std::vector<std::string> MapDataSourceFactory::getNames() const
        {
            return mystd::keys( mmap );
        }

        bool MapDataSourceFactory::hasMember( const std::string& s ) const
        {
            return mmap.find( s ) != mmap.end();
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
}
