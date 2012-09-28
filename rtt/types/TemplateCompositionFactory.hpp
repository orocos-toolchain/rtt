#ifndef RTT_TEMPLATE_COMPOSITION_FACTORY_HPP
#define RTT_TEMPLATE_COMPOSITION_FACTORY_HPP

#include "CompositionFactory.hpp"
#include "../Property.hpp"
#include "../PropertyBag.hpp"

namespace RTT {
    namespace types {
        template<typename T>
        class TemplateCompositionFactory
            : public CompositionFactory
        {
        public:

        /**
         * The given \a T parameter is the type for reading DataSources.
         */
        typedef T UserType;

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (source.get() );
            if ( !pb )
                return false;
            typename internal::AssignableDataSource<UserType>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<UserType> >( result );
            if ( !ads )
                return false;

            // last fall-back: use user supplied function:
            if ( composeTypeImpl( pb->rvalue(), ads->set() ) )
                ads->updated();
            else {
                Logger::log() <<Logger::Debug<<"Failed to compose from "<< source->getTypeName() <<Logger::endl;
                return false;
            }
            Logger::log() <<Logger::Debug<<"Successfuly composed type from "<< source->getTypeName() <<Logger::endl;
            return true;
        }

        /**
         * This default implementation sets up a PropertyBag which is passed
         * to decomposeTypeImpl(). It is advised to implement that function and
         * to leave this function as-is, unless you don't want to return a
         * PropertyBag, but another data type.
         */
        virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
        {
            // Extract typed values
            typename internal::DataSource<UserType>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<UserType> >( source );
            if ( !ds )
                return base::DataSourceBase::shared_ptr(); // happens in the case of 'unknown type'
            Property<PropertyBag> targetbag_p("targetbag");
            if (decomposeTypeImpl( ds->rvalue(), targetbag_p.value() ))
                return targetbag_p.getDataSource();
            return base::DataSourceBase::shared_ptr();
        }

        /**
         * User, implement this function in case you want to control reading the XML data format.
         */
        virtual bool composeTypeImpl(const PropertyBag& source,  typename internal::AssignableDataSource<T>::reference_t result) const {
            return false;
        }

        /**
         * User, implement this function in case you want to control writing the XML data format.
         * Add the structural elements of source to targetbag.
         */
        virtual bool decomposeTypeImpl( typename internal::AssignableDataSource<T>::const_reference_t source, PropertyBag& targetbag ) const {
            return false;
        }

    };
    }
}

#endif
