/***************************************************************************
  tag: Peter Soetens Fri Oct 1 14:48:38 2010 +0200 SequenceConstructor.hpp

                        SequenceConstructor.hpp -  description
                           -------------------
    begin                : Fri Oct 1 2010
    copyright            : (C) 2010 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef SEQUENCECONSTRUCTOR_HPP_
#define SEQUENCECONSTRUCTOR_HPP_

#include "TypeConstructor.hpp"
#include "../internal/DataSources.hpp"
#include <vector>

namespace RTT
{
    namespace types
    {
        /**
         * Sequence constructor which takes the number of elements in the sequence
         */
        template<class T>
        struct sequence_ctor: public std::unary_function<int, const T& >
        {
            typedef const T& ( Signature)(int);
            mutable boost::shared_ptr<T> ptr;
            sequence_ctor() :
                ptr(new T())
            {
            }
            const T& operator()(int size) const
            {
                ptr->resize(size);
                return *(ptr);
            }
        };

        /**
         * See NArityDataSource which requires a function object like
         * this one.
         */
        template<class T>
        struct sequence_varargs_ctor
        {
            typedef const std::vector<T>& result_type;
            typedef T argument_type;
            result_type operator()(const std::vector<T>& args) const
            {
                return args;
            }
        };

        /**
         * Helper DataSource for constructing sequences with a variable number of
         * parameters.
         */
        template<class T>
        struct sequence_constructor_datasource {
            typedef internal::NArityDataSource<sequence_varargs_ctor<T> > type;
        };

        /**
         * Constructs an sequence with \a n elements, which are given upon
         * construction time.
         */
        template<class T>
        struct SequenceBuilder: public TypeConstructor
        {
            typedef typename T::value_type value_type;
            virtual base::DataSourceBase::shared_ptr build(const std::vector<
                    base::DataSourceBase::shared_ptr>& args) const
            {
                if (args.size() == 0)
                    return base::DataSourceBase::shared_ptr();
                typename sequence_constructor_datasource<value_type>::type::shared_ptr vds = new typename sequence_constructor_datasource<value_type>::type();
                for (unsigned int i = 0; i != args.size(); ++i)
                {
                    typename internal::DataSource<value_type>::shared_ptr dsd =
                            boost::dynamic_pointer_cast<internal::DataSource<value_type> >(
                                    args[i]);
                    if (dsd)
                        vds->add(dsd);
                    else
                        return base::DataSourceBase::shared_ptr();
                }
                return vds;
            }

        };

        /**
         * Constructs a sequence from the number of elements and a prototype
         * element for these elements.
         * Usage: sequence_ctor2<std::vector<Foo> >()
         */
        template<class T>
        struct sequence_ctor2: public std::binary_function<int, typename T::value_type,
                const T&>
        {
            typedef const T& ( Signature)(int, typename T::value_type);
            mutable boost::shared_ptr<T> ptr;
            sequence_ctor2() :
                ptr(new T() )
            {
            }
            const T& operator()(int size, typename T::value_type value) const
            {
                ptr->resize(size);
                ptr->assign(size, value);
                return *(ptr);
            }
        };

    }
}

#endif /* SEQUENCECONSTRUCTOR_HPP_ */
