/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  TemplateConstructor.hpp

                        TemplateConstructor.hpp -  description
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


#ifndef ORO_TEMPLATE_CONSTRUCTOR_HPP
#define ORO_TEMPLATE_CONSTRUCTOR_HPP

#include "Types.hpp"
#include "../internal/FusedFunctorDataSource.hpp"
#include "../internal/CreateSequence.hpp"

#include <boost/type_traits/function_traits.hpp>
#include <boost/function.hpp>

#include "../rtt-config.h"

namespace RTT
{
    namespace types {
        /**
         * The constructor classes allow to define type constructors
         * or type conversions (convert type B from type A).
         * @see TypeInfo::addConstructor()
         * @param S The function Signature, should at least have one argument
         * and must return non-void.
         */
        template<class S>
        struct TemplateConstructor
            : public TypeBuilder
        {
            typedef typename boost::function_traits<S>::result_type result_type;
            typedef typename boost::function_traits<S>::arg1_type arg1_type;
            typedef internal::create_sequence<typename boost::function_types::parameter_types<S>::type> SequenceFactory;

            boost::function<S> ff;
            bool automatic;

            template<class FInit>
            TemplateConstructor( FInit f, bool autom)
                : ff(f), automatic(autom)
            {}

            virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                // number of arguments must be exact.
                if ( args.size() != boost::function_traits<S>::arity )
                    return base::DataSourceBase::shared_ptr();
                try {
                    return new internal::FusedFunctorDataSource<S>(ff, SequenceFactory::sources(args.begin()) );
                } catch(...) // wrong argument types
                {}
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr convert(base::DataSourceBase::shared_ptr arg) const {
                if ( boost::function_traits<S>::arity != 1) {
                    return base::DataSourceBase::shared_ptr();
                } else {
                    // The compiler should optimise this out...
                    // these checks are necessary because produce(args) calls convert, which could lead to endless loops.
                    // detect same type converion.
                    if ( arg->getTypeInfo() == internal::DataSourceTypeInfo<result_type>::getTypeInfo() ) {
                        return arg;
                    }
                    // detect invalid type conversion.
                    if ( arg->getTypeInfo() != internal::DataSourceTypeInfo<arg1_type>::getTypeInfo() ) {
                        return base::DataSourceBase::shared_ptr();
                    }
                    // from now on, it should always succeed.
                    std::vector<base::DataSourceBase::shared_ptr> args;
                    args.push_back(arg);
                    base::DataSourceBase::shared_ptr ret = this->build(args);
                    assert( ret );
                    if (!automatic)
                        log(Warning) << "Conversion from " << arg->getTypeName() << " to " << ret->getTypeName() <<endlog();
                    return ret;
                }
            }
        };
    /**
     * Create a new Constructor.
     *
     * @param foo A pointer to the 'C' function which creates an object.
     * @param automatic Set to true to allow automatic conversion (without warning) to this type.
     * @return a Constructor object suitable for the type system.
     */
    template<class Function>
    TypeBuilder* newConstructor( Function* foo, bool automatic = false ) {
        return new detail::TemplateConstructor<Function>(foo, automatic);
    }

    /**
     * Create a new Constructor.
     *
     * @param obj A function object which has operator().
     * @param automatic Set to true to allow automatic conversion (without warning) to this type.
     *
     * @return a Constructor object suitable for the type system.
     */
    template<class Object>
    TypeBuilder* newConstructor( Object obj, bool automatic = false) {
        return new detail::TemplateConstructor<typename Object::Signature>(obj, automatic);
    }
}}

#endif
