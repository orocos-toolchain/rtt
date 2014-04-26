/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SequenceTypeInfo.hpp

                        SequenceTypeInfo.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_SEQUENCE_TYPE_INFO_BASE_HPP
#define ORO_SEQUENCE_TYPE_INFO_BASE_HPP

#include "SequenceConstructor.hpp"
#include "TemplateConstructor.hpp"
#include "PropertyComposition.hpp"
#include "VectorTemplateComposition.hpp"
#include "PropertyDecomposition.hpp"
#include "../internal/FusedFunctorDataSource.hpp"
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
         * Returns a copy to one item in an STL container.
         * @note vector<bool> is not supported, since it's not an STL container.
         * @param cont The container to access
         * @param index The item to extract from the sequence
         * @return A copy of item \a index
         */
        template<class T>
        typename T::value_type get_container_item_copy(const T & cont, int index)
        {
            if (index >= (int) (cont.size()) || index < 0)
                return internal::NA<typename T::value_type>::na();
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
        bool get_container_item_copy(const std::vector<bool> & cont, int index);

        /**
         * Template for data types that are C++ STL Sequences with operator[], size() and capacity() methods.
         *
         * This class requires that your STL Sequence is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace.
         */
        template<typename T>
        class SequenceTypeInfoBase
        {
        public:
            SequenceTypeInfoBase()
            {
            }

            bool installTypeInfoObject(TypeInfo* ti) {
                ti->addConstructor( new SequenceBuilder<T>() );
                ti->addConstructor( newConstructor( sequence_ctor<T>() ) );
                ti->addConstructor( newConstructor( sequence_ctor2<T>() ) );
                // Don't delete us, we're memory-managed.
                return false;
            }

            ~SequenceTypeInfoBase() {}

            base::AttributeBase* buildVariable(std::string name,int size) const
            {
                // if a sizehint is given
                T t_init(size, typename T::value_type() );

                return new Attribute<T>( name, new internal::UnboundDataSource<internal::ValueDataSource<T> >( t_init ) );
            }

            bool resize(base::DataSourceBase::shared_ptr arg, int size) const
            {
                if (arg->isAssignable()) {
                    typename internal::AssignableDataSource<T>::shared_ptr asarg = internal::AssignableDataSource<T>::narrow( arg.get() );
                    asarg->set().resize( size );
                    asarg->updated();
                    return true;
                }
                return false;
            }

            /**
             * Specialize to resize \a result given the size of \a source.
             */
            bool composeType( base::DataSourceBase::shared_ptr dssource, base::DataSourceBase::shared_ptr dsresult) const {
                const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (dssource.get() );
                if ( !pb )
                    return false;
                typename internal::AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( dsresult );
                if ( !ads )
                    return false;

                PropertyBag const& source = pb->rvalue();
                typename internal::AssignableDataSource<T>::reference_t result = ads->set();

                // recurse into items of this sequence:
                PropertyBag target( source.getType() );
                // we compose each item in this sequence and then update result with target's result.
                // 1. each child is composed into target (this is a recursive thing using composeType() on each child)
                // 2. we decompose result one-level deep and 
                // 3. 'refresh' it with the composed children of step 1.
                if ( composePropertyBag(source, target) && composeTemplateProperty(target, result ) ){
                    ads->updated();
                    Logger::log() <<Logger::Debug<<"Successfuly composed Sequence from "<< source.getType() <<Logger::endl;
                    return true;
                } else 
                    Logger::log() <<Logger::Debug<<"Failed to composed Sequence from "<< source.getType() <<Logger::endl;

                return false;
            }

            /**
             * Use getMember() for decomposition...
             */
            base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
            {
                return base::DataSourceBase::shared_ptr();
            }


            std::vector<std::string> getMemberNames() const {
                // only discover the parts of this struct:
                std::vector<std::string> result;
                result.push_back("size");
                result.push_back("capacity");
                return result;
            }

            base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                // the only thing we do is to check for an integer in name, otherwise, assume a part (size/capacity) is accessed:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return getMember( item, new internal::ConstantDataSource<int>(indx));
                } catch(...) {}

                return getMember( item, new internal::ConstantDataSource<std::string>(name) );
            }

            base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                // discover if user gave us a part name or index:
                typename internal::DataSource<int>::shared_ptr id_indx = internal::DataSource<int>::narrow( internal::DataSourceTypeInfo<int>::getTypeInfo()->convert(id).get() );
                typename internal::DataSource<std::string>::shared_ptr id_name = internal::DataSource<std::string>::narrow( id.get() );
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
                        if ( item->isAssignable() )
                                return internal::newFunctorDataSource(&get_container_item<T>, internal::GenerateDataSource()(item.get(), id_indx.get() ) );
                            else
                                return internal::newFunctorDataSource(&get_container_item_copy<T>, internal::GenerateDataSource()(item.get(), id_indx.get() ) );
                    } catch(...) {}
                }
                if (id_name) {
                    log(Error) << "SequenceTypeInfo: No such member : " << id_name->get() << endlog();
                }
                if (id_indx) {
                    log(Error) << "SequenceTypeInfo: Invalid index : " << id_indx->get() <<":"<< id_indx->getTypeName() << endlog();
                }
                if ( !id_name && ! id_indx)
                    log(Error) << "SequenceTypeInfo: Not a member or index : " << id <<":"<< id->getTypeName() << endlog();
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif
