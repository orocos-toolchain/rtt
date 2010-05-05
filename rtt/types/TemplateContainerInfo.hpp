#ifndef ORO_TEMPLATE_CONTAINER_INFO_HPP
#define ORO_TEMPLATE_CONTAINER_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "type_discovery.hpp"
#include "../internal/DataSourceGenerator.hpp"

namespace RTT
{
    namespace types
    {
        char get_char_from_string(const std::string& s, int index)
        {
            if (index >= (int) (s.length()) || index < 0)
                return 0;
            return s[index];
        }

        template<class T>
        int get_capacity(T const& cont)
        {
            return cont.capacity();
        }

        template<class T>
        int get_size(T const& cont)
        {
            return cont.size();
        }

        template<class T>
        typename T::value_type& get_container_item(T & cont, int index)
        {
            if (index >= (int) (cont.size()) || index < 0)
                return NA<typename T::value_type&>::na();
            return cont[index];
        }

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
                    try {
                        return newFunctorDataSource(&get_size<T>, GenerateDataSource()(item.get()) );
                    } catch(...) {}
                }
                if ( name == "capacity" ) {
                    try {
                        return newFunctorDataSource(&get_capacity<T>, GenerateDataSource()(item.get()) );
                    } catch(...) {}
                }

                // contents of indx *can* change during program execution:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return newFunctorDataSource(&get_container_item<T>, GenerateDataSource()(item.get(), new ConstantDataSource<int>(indx)) );
                } catch(...) {}
                log(Error) << "TemplateContainerInfo: No such part (or invalid index): " << name << endlog();
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
                    if ( id_name->get() == "size" ) {
                        try {
                            return newFunctorDataSource(&get_size<T>, GenerateDataSource()(item.get()) );
                        } catch(...) {}
                    }
                    if ( id_name->get() == "capacity" ) {
                        try {
                            return newFunctorDataSource(&get_capacity<T>, GenerateDataSource()(item.get()) );
                        } catch(...) {}
                    }
                }

                if ( id_indx ) {
                    return newFunctorDataSource(&get_container_item<T>, GenerateDataSource()(item.get(), id_indx) );
                }
                log(Error) << "TemplateContainerInfo: No such part (or invalid index): " << id_name->get() << id_indx->get() << endlog();
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif
