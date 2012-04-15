/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  MatrixTypeInfo.hpp

                        MatrixTypeInfo.hpp -  description
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


#ifndef ORO_MATRIX_TYPE_INFO_HPP
#define ORO_MATRIX_TYPE_INFO_HPP

#include "types/TemplateTypeInfo.hpp"
#include "types/MemberFactory.hpp"
#include "types/type_discovery.hpp"
#include "internal/DataSourceGenerator.hpp"
#include <boost/lexical_cast.hpp>

namespace RTT
{
    namespace types
    {
        /**
         * Returns the number of rows of a matrix container which has the rows() member function.
         * @param cont A const ref to a matrix container.
         * @return its number of rows.
         */
        template<class T>
        int get_rows(T const& cont)
        {
            return cont.rows();
        }

        /**
         * Returns the number of columns of a matrix container which has the columns() member function.
         * @param cont A const ref to a matrix container.
         * @return its number of columns.
         */
        template<class T>
        int get_columns(T const& cont)
        {
            return cont.columns();
        }

        /**
         * Returns a reference to one item in a matrix container.
         * @param cont The container to access
         * @param index_row The row index of the item to reference
         * @param index_column The column index of the item to reference
         * @return A reference to item \a index
         */
        template<class T>
        typename T::reference get_container_item(T & cont, int index_row, int index_column)
        {
            if (index_row >= (int) (cont.rows()) || index_row < 0 || index_column >= (int) (cont.columns()) || index_column < 0)
                return internal::NA<typename T::reference>::na();
            return cont[index_row,index_column];
        }


        /**
         * Template for data types that are C++ STL Sequences with operator[,], rows() and columns() methods.
         *
         * This class requires that your STL Sequence is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace.
         *
         * The matrix is always square !
         */
        template<typename T, bool has_ostream = false>
        class MatrixTypeInfo: public TemplateTypeInfo<T, has_ostream>, public MemberFactory
        {
        public:
            MatrixTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            bool installTypeInfoObject(TypeInfo* ti) {
                // aquire a shared reference to the this object
                boost::shared_ptr< MatrixTypeInfo<T> > mthis = boost::dynamic_pointer_cast<MatrixTypeInfo<T> >( this->getSharedPtr() );
                // Allow base to install first
                TemplateTypeInfo<T,has_ostream>::installTypeInfoObject(ti);
                // Install the factories for primitive types
                ti->setMemberFactory( mthis );
                
                // Don't delete us, we're memory-managed.
                return false;
            }

            base::AttributeBase* buildVariable(std::string name,int size) const
            {
                // if a sizehint is given
                T t_init(size_rows, size_columns, typename T::value_type() );

                log(Debug) << "Building variable '"<<name <<"' of type " << this->getTypeName() <<" and number of rows "<< size << " and number of columns " << size <<Logger::endl;
                return new Attribute<T>( name, new internal::UnboundDataSource<internal::ValueDataSource<T> >( t_init ) );
            }

            bool resize(base::DataSourceBase::shared_ptr arg, int size_rows, int size_columns) const
            {
                if (arg->isAssignable()) {
                    typename internal::AssignableDataSource<T>::shared_ptr asarg = internal::AssignableDataSource<T>::narrow( arg.get() );
                    asarg->set().resize( size_rows, size_columns );
                    asarg->updated();
                    return true;
                }
                return false;
            }

            /**
             * Specialize to resize \a result given the size of \a source.
             */
            virtual bool composeTypeImpl(const PropertyBag& bag, typename internal::AssignableDataSource<T>::reference_t result) const
            {
                if ( bag.getType() == "Matrix" ) {
                    unsigned int rows = bag.size();
                    unsigned int cols = 0;
                    // Get values
                    for (unsigned int i = 1; i <= rows ; i++) {
                        std::stringstream out;
                        out << i;
                        Property<PropertyBag>* row_bag =  bag.getProperty<PropertyBag>(out.str());
                        if(row_bag==NULL){
                            log(Error)<<"Could not read row "<<i<<endlog();
                            return false;
                        }
                        Property<RowVector> row_p(row_bag->getName(),row_bag->getDescription());
                        if(!(row_p.getDataSource()->composeType(row_bag->getDataSource()))){
                            log(Error)<<"Could not decompose row "<<i<<endlog();
                            return false;
                        }
                        if(row_p.ready()){
                            if(i==1){
                                cols = row_p.get().size();
                                result.resize(rows,cols);
                            } else
                                if(row_p.get().size()!=cols){
                                    log(Error)<<"Row "<<i+1<<" size does not match matrix columns"<<endlog();
                                    return false;
                                }
                            for ( unsigned int j=1; j <= row_p.get().size() ; j++){
                                result(i,j)=row_p.get()(j);
                            }
                        }else{
                            log(Error)<<"Property of Row "<<i<<"was not ready for use"<<endlog();
                            return false;
                        }
                    }
                }else {
                    log(Error) << "Composing Property< Matrix > :"
                               << " type mismatch, got type '"<< bag.getType()
                               << "', expected type "<<"Matrix."<<endlog();
                    return false;
                }
                return true;
            }
/*
                // take into account sequences:
                base::PropertyBase* sz = source.find("Rows");
                if (!sz)
                    sz = source.find("rows");
                if (sz)
                {
                    internal::DataSource<int>::shared_ptr sz_ds = internal::DataSource<int>::narrow(sz->getDataSource().get());
                    if (sz_ds)
                        result.resize( sz_ds->get(), sz_ds->get()  );
                }
                else
                {
                    // no size found, inform parent of number of elements to come:
                    result.resize( source.size() );
                }
                return TemplateTypeInfo<T,has_ostream>::composeTypeImpl(source,result);
            }
*/

/*
            virtual std::vector<std::string> getMemberNames() const {
                // only discover the parts of this struct:
                std::vector<std::string> result;
                result.push_back("size");
                result.push_back("capacity");
                return result;
            }
*/

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                // the only thing we do is to check for an integer in name, otherwise, assume a part (size/capacity) is accessed:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return getMember( item, new internal::ConstantDataSource<int>(indx));
                } catch(...) {}

                return getMember( item, new internal::ConstantDataSource<std::string>(name) );
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                typename internal::AssignableDataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( item );
                if ( !data ) {
                    if ( !item->isAssignable() )
                        log(Error) << "Can't return reference to members of type "<< this->getTypeName() <<" since given object is not assignable." <<endlog();
                    else
                        log(Error) << "Consistency error: TypeInfo of type "<< this->getTypeName() <<" can't handle types of type "<< item->getType() <<endlog();
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
                    log(Error) << "MatrixTypeInfo: No such part : " << id_name->get() << endlog();
                }
                if (id_indx) {
                    log(Error) << "MatrixTypeInfo: Invalid index : " << id_indx->get() <<":"<< id_indx->getTypeName() << endlog();
                }
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif
