#ifndef ORO_TEMPLATE_STRUCT_INFO_HPP
#define ORO_TEMPLATE_STRUCT_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "type_discovery.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * Template for data types that are structs.
         *
         * This class requires that your struct is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace.
         */
        template<typename T, bool has_ostream = false>
        class TemplateStructInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            using TemplateTypeInfo<T, has_ostream>::buildConstant;
            using TemplateTypeInfo<T, has_ostream>::buildVariable;

            TemplateStructInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            virtual std::vector<std::string> getPartNames() const {
                // only discover the parts of this struct:
                type_discovery in;
                T t; // boost can't work without a value.
                in.discover( t );
                return in.mnames;
            }

            virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                typename AssignableDataSource<T>::shared_ptr data = AssignableDataSource<T>::narrow( item.get() );
                if ( !data ) {
                    log(Error) << "No such part: " << name << endlog();
                    return base::DataSourceBase::shared_ptr();
                }
                type_discovery in( item );
                in.discover( data->set() );
                log(Debug) << "Returning part: " << name << endlog();
                return in.getPart(name);
            }
        };
    }
}

#endif
