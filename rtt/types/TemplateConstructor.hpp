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
                    return new internal::FusedFunctorDataSource<S>(ff, SequenceFactory()(args) );
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
