#ifndef ORO_TEMPLATE_BOOSTARRAY_INFO_HPP
#define ORO_TEMPLATE_BOOSTARRAY_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "../internal/ArrayPartDataSource.hpp"
#include "type_discovery.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>

namespace RTT
{
    namespace types
    {
        /**
         * Template for data types that are of type boost::array<U,int>
         *
         * @param T A boost::array<U> wrapper, where U is a data type.
         */
        template<typename T, bool has_ostream = false>
        class BoostArrayTypeInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            BoostArrayTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            virtual std::vector<std::string> getPartNames() const {
                // only discover the parts of this struct:
                std::vector<std::string> result;
                result.push_back("size");
                result.push_back("capacity");
                return result;
            }

            virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                typename internal::AssignableDataSource<T>::shared_ptr data = internal::AdaptAssignableDataSource<T>()( item );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // size and capacity can not change during program execution:
                if (name == "size" || name == "capacity") {
                    return new ValueDataSource<unsigned int>( T::static_size );
                }

                // contents of indx can change during program execution:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return new ArrayPartDataSource<typename T::value_type>( *data->set().c_array(), new ConstantDataSource<unsigned int>(indx), item );
                } catch(...) {}
                log(Error) << "BoostArrayTypeInfo: No such part (or invalid index): " << name << endlog();
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                typename internal::AssignableDataSource<T>::shared_ptr data = internal::AdaptAssignableDataSource<T>()( item );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // discover if user gave us a part name or index:
                typename DataSource<unsigned int>::shared_ptr id_indx = DataSource<unsigned int>::narrow( id.get() );
                typename DataSource<string>::shared_ptr id_name = DataSource<string>::narrow( id.get() );
                if ( id_name ) {
                    // size and capacity can not change during program execution:
                    if (id_name->get() == "size" || id_name->get() == "capacity") {
                        return new ValueDataSource<unsigned int>( T::static_size );
                    }
                }

                if ( id_indx ) {
                    return new ArrayPartDataSource<typename T::value_type>( *data->set().c_array(), id_indx, item );
                }
                log(Error) << "BoostArrayTypeInfo: No such part (or invalid index): " << id_name->get() << id_indx->get() << endlog();
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif
