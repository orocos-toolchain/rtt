#ifndef ORO_TEMPLATE_CONTAINER_INFO_HPP
#define ORO_TEMPLATE_CONTAINER_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "type_discovery.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * Template for data types that are C++ STL Containers with operator[]
         *
         * This class requires that your STL container is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace.
         */
        template<typename T, bool has_ostream = false>
        class TemplateContainerInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            TemplateContainerInfo(std::string name) :
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
                typename AssignableDataSource<T>::shared_ptr data = AssignableDataSource<T>::narrow( item.get() );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // size and capacity *can* change during program execution:
                if ( name == "size" ) {
                    assert(false);
                    return new ValueDataSource<unsigned int>( data->set().size() );
                }
                if ( name == "capacity" ) {
                    assert(false);
                    return new ValueDataSource<unsigned int>( data->set().capacity() ); // broken.
                }

                // contents of indx *can* change during program execution:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    //return new ArrayPartDataSource<typename T::value_type>( *data->set().address(), new ConstantDataSource<unsigned int>(indx), item );
                    assert(false);
                } catch(...) {}
                log(Error) << "TemplateCArrayInfo: No such part (or invalid index): " << name << endlog();
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                typename AssignableDataSource<T>::shared_ptr data = AssignableDataSource<T>::narrow( item.get() );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // discover if user gave us a part name or index:
                typename DataSource<unsigned int>::shared_ptr id_indx = DataSource<unsigned int>::narrow( id.get() );
                typename DataSource<string>::shared_ptr id_name = DataSource<string>::narrow( id.get() );
                if ( id_name ) {
                    if ( name == "size" ) {
                        assert(false);
                        return new ValueDataSource<unsigned int>( data->set().size() );
                    }
                    if ( name == "capacity" ) {
                        assert(false);
                        return new ValueDataSource<unsigned int>( data->set().capacity() ); // broken.
                    }
                }

                if ( id_indx ) {
                    assert(false);
                    //return new ArrayPartDataSource<typename T::value_type>( *data->set().address(), id_indx, item );
                }
                log(Error) << "TemplateContainerInfo: No such part (or invalid index): " << id_name->get() << id_indx->get() << endlog();
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif
