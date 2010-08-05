#ifndef ORO_STRUCT_TYPE_INFO_HPP
#define ORO_STRUCT_TYPE_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "type_discovery.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * Type Information for data types that are structs, ie all user data types fall in this category.
         *
         * Use this class to register your data type to the Orocos type system.
         *
         * This class requires that your struct is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace. If no such function exists, you can
         * fall back to StdTypeInfo or even TemplateTypeInfo.
         */
        template<typename T, bool has_ostream = false>
        class StructTypeInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            StructTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            virtual std::vector<std::string> getMemberNames() const {
                // only discover the parts of this struct:
                type_discovery in;
                T t; // boost can't work without a value.
                in.discover( t );
                return in.mnames;
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                typename internal::AssignableDataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( item );
                if ( !data ) {
                    log(Error) << "Could not get reference to value of 'Struct' data source of type " + item->getTypeName() << " because it's not an AssignableDataSource."<<endlog();
                    return base::DataSourceBase::shared_ptr();
                }
                type_discovery in( item );
                in.discover( data->set() );
                log(Debug) << "Returning part: " << name << endlog();
                return in.getMember(name);
            }
        };
    }
}

#endif
