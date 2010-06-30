/***************************************************************************
  tag: Tinne De Laet 2007  VectorTemplateComposition.hpp
       2007 Ruben Smits
                        VectorTemplateComposition.hpp -  description

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

#ifndef VECTOR_TEMPLATE_COMPOSITION_HPP
#define VECTOR_TEMPLATE_COMPOSITION_HPP

#include "../Property.hpp"
#include "../PropertyBag.hpp"
#include "SequenceTypeInfo.hpp"
#include "Types.hpp"
#include "../Logger.hpp"
#include "../internal/DataSources.hpp"
#include <ostream>
#include <sstream>
#include <vector>

namespace RTT
{ namespace types {
    template <typename T, bool has_ostream>
    struct StdVectorTemplateTypeInfo
        : public SequenceTypeInfo<std::vector<T>, has_ostream >
    {
        StdVectorTemplateTypeInfo<T,has_ostream>( std::string name )
            : SequenceTypeInfo<std::vector<T>, has_ostream >(name)
        {
        };

    };

    template<typename T>
    std::ostream& operator << (std::ostream& os, const std::vector<T>& vec)
    {
        os<<'[';
        for(unsigned int i=0;i<vec.size();i++){
            if(i>0)
                os<<',';
            os<<vec[i]<<' ';
        }

        return os << ']';
    };

    template<typename T>
    std::istream& operator >> (std::istream& is,std::vector<T>& vec)
    {
        return is;
    };

    template<typename T>
    struct stdvector_ctor
        : public std::unary_function<int, const std::vector<T>&>
    {
        typedef const std::vector<T>& (Signature)( int );
        mutable boost::shared_ptr< std::vector<T> > ptr;
        stdvector_ctor()
            : ptr( new std::vector<T>() ) {}
        const std::vector<T>& operator()( int size ) const
        {
            ptr->resize( size );
            return *(ptr);
        }
    };

    /**
     * See internal::NArityDataSource which requires a function object like
     * this one.
     */
    template<typename T>
    struct stdvector_varargs_ctor
    {
        typedef const std::vector<T>& result_type;
        typedef T argument_type;
        result_type operator()( const std::vector<T>& args ) const
        {
            return args;
        }
    };

    /**
     * Constructs an array with \a n elements, which are given upon
     * construction time.
     */
     template<typename T>
     struct StdVectorBuilder
         : public TypeBuilder
     {
         virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
             if (args.size() == 0 )
                 return base::DataSourceBase::shared_ptr();
             typename internal::NArityDataSource<stdvector_varargs_ctor<T> >::shared_ptr vds = new internal::NArityDataSource<stdvector_varargs_ctor<T> >();
             for(unsigned int i=0; i != args.size(); ++i) {
                 typename internal::DataSource<T>::shared_ptr dsd = boost::dynamic_pointer_cast< DataSource<T> >( args[i] );
                 if (dsd)
                     vds->add( dsd );
                 else
                     return base::DataSourceBase::shared_ptr();
             }
             return vds;
         }
     };

    template<typename T>
    struct stdvector_ctor2
        : public std::binary_function<int, T, const std::vector<T>&>
    {
        typedef const std::vector<T>& (Signature)( int, T );
        mutable boost::shared_ptr< std::vector<T> > ptr;
        stdvector_ctor2()
            : ptr( new std::vector<T>() ) {}
        const std::vector<T>& operator()( int size, T value ) const
        {
            ptr->resize( size );
            ptr->assign( size, value );
            return *(ptr);
        }
    };
}};
#endif

