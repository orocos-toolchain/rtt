#ifndef ORO_SEQUENCE_TYPE_INFO_HPP
#define ORO_SEQUENCE_TYPE_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "type_discovery.hpp"
#include "../internal/DataSourceGenerator.hpp"
#include <boost/lexical_cast.hpp>

namespace RTT
{
    namespace types
    {
        /**
         * Returns the capacity of an STL container which has the capacity() member function.
         * @param cont A const ref to an STL container.
         * @return its capacity.
         */
        template<class T>
        int get_capacity(T const& cont)
        {
            return cont.capacity();
        }

        /**
         * Returns the size of an STL container which has the size() member function.
         * @param cont A const ref to an STL container.
         * @return its size.
         */
        template<class T>
        int get_size(T const& cont)
        {
            return cont.size();
        }

        /**
         * Returns a reference to one item in an STL container.
         * @note vector<bool> is not supported, since it's not an STL container.
         * @param cont The container to access
         * @param index The item to reference
         * @return A reference to item \a index
         */
        template<class T>
        typename T::reference get_container_item(T & cont, int index)
        {
            if (index >= (int) (cont.size()) || index < 0)
                return internal::NA<typename T::reference>::na();
            return cont[index];
        }

        /**
         * Specialisation for vector<bool>, we don't return references to bits aka std::_Bit_reference.
         * vector<bool> is an outlier and should not be used. Use vector<int>
         * or vector<char> instead.
         * @param cont Is a vector<bool>
         * @param index The item to read.
         * @return A copy of the value at position \a index. \b not a reference !
         */
        bool get_container_item(std::vector<bool> & cont, int index);

        /**
         * Template for data types that are C++ STL Sequences with operator[], size() and capacity() methods.
         *
         * This class requires that your STL Sequence is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace.
         */
        template<typename T, bool has_ostream = false>
        class SequenceTypeInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            SequenceTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            base::AttributeBase* buildVariable(std::string name,int size) const
            {
                // if a sizehint is given
                T t_init(size, typename T::value_type() );

                log(Debug) << "Building variable '"<<name <<"' of type " << this->getTypeName() <<" and size "<< size <<Logger::endl;
                return new Attribute<T>( name, new internal::UnboundDataSource<internal::ValueDataSource<T> >( t_init ) );
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

                // size and capacity *can* change during program execution:
                if ( name == "size" ) {
                    try {
                        return internal::newFunctorDataSource(&get_size<T>, internal::GenerateDataSource()(item.get()) );
                    } catch(...) {}
                }
                if ( name == "capacity" ) {
                    try {
                        return internal::newFunctorDataSource(&get_capacity<T>, internal::GenerateDataSource()(item.get()) );
                    } catch(...) {}
                }

                // contents of indx *can* change during program execution:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return internal::newFunctorDataSource(&get_container_item<T>, internal::GenerateDataSource()(item.get(), new internal::ConstantDataSource<int>(indx)) );
                } catch(...) {}
                log(Error) << "TemplateContainerInfo: No such part (or invalid index): " << name << endlog();
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                typename internal::AssignableDataSource<T>::shared_ptr data = internal::AdaptAssignableDataSource<T>()( item );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // discover if user gave us a part name or index:
                typename internal::DataSource<int>::shared_ptr id_indx = internal::DataSource<int>::narrow( id.get() );
                typename internal::DataSource<string>::shared_ptr id_name = internal::DataSource<string>::narrow( id.get() );
                if ( id_name ) {
                    if ( id_name->get() == "size" ) {
                        try {
                            return internal::newFunctorDataSource(&get_size<T>, internal::GenerateDataSource()(item.get()) );
                        } catch(...) {}
                    }
                    if ( id_name->get() == "capacity" ) {
                        try {
                            return internal::newFunctorDataSource(&get_capacity<T>, internal::GenerateDataSource()(item.get()) );
                        } catch(...) {}
                    }
                }

                if ( id_indx ) {
                    try {
                        return internal::newFunctorDataSource(&get_container_item<T>, internal::GenerateDataSource()(item.get(), id_indx.get() ) );
                    } catch(...) {}
                }
                if (id_name) {
                    log(Error) << "TemplateContainerInfo: No such part : " << id_name->get() << endlog();
                }
                if (id_indx) {
                    log(Error) << "TemplateContainerInfo: Invalid index : " << id_indx->get() <<":"<< id_indx->getTypeName() << endlog();
                }
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif