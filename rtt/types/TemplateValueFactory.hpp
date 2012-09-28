#ifndef RTT_TEMPLATE_TYPE_FACTORY
#define RTT_TEMPLATE_TYPE_FACTORY

#include "ValueFactory.hpp"
#include "../Property.hpp"
#include "../Attribute.hpp"
#include "../Logger.hpp"
#include "../rtt-config.h"

namespace RTT
{
    namespace types {

        template<class T>
        class TemplateValueFactory 
            : public ValueFactory
        {
        public:
            typedef T DataType;
            base::AttributeBase* buildConstant(std::string name, base::DataSourceBase::shared_ptr dsb) const
            {
                typename internal::DataSource<DataType>::shared_ptr res =
                    boost::dynamic_pointer_cast< internal::DataSource<DataType> >( internal::DataSourceTypeInfo<DataType>::getTypeInfo()->convert(dsb));
                if ( res ) {
                    res->get();
                    //Logger::log() << Logger::Info << "Building "<<tname<<" Constant '"<<name<<"' with value "<< dsb->getTypeInfo()->toString(dsb) <<Logger::endl;
                    return new Constant<DataType>( name, res->rvalue() );
                }
                else
                    return 0;
            }

            base::AttributeBase* buildVariable(std::string name) const
            {
                // A variable starts its life as unbounded.
                //Logger::log() << Logger::Debug << "Building variable '"<<name <<"' of type " << tname <<Logger::endl;
                return new Attribute<T>( name, new internal::UnboundDataSource<internal::ValueDataSource<T> >() );
            }

            base::AttributeBase* buildAttribute( std::string name, base::DataSourceBase::shared_ptr in) const
            {
                typename internal::AssignableDataSource<DataType>::shared_ptr ds;
                if ( !in )
                    ds = new internal::ValueDataSource<DataType>();
                else
                    ds = internal::AssignableDataSource<DataType>::narrow( in.get() );
                if (!ds)
                    return 0;
                // An attribute is always bounded.
                //Logger::log() << Logger::Debug << "Building Attribute '"<< name <<"' of type " << tname <<Logger::endl;
                return new Attribute<DataType>( name, ds.get() );
            }

            base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr in ) const
            {
                typename internal::DataSource<T>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<T> >( internal::DataSourceTypeInfo<T>::getTypeInfo()->convert(in) );
                if ( ! ds )
                    return 0;
                return new Alias( name, ds );
            }

            base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* action, base::DataSourceBase::shared_ptr in) const
            {
                typename internal::AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( in ); // no type conversion is done.
                if ( ads )
                    return new internal::ActionAliasAssignableDataSource<T>(action, ads.get());

                typename internal::DataSource<T>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<T> >( in ); // no type conversion is done.
                if ( ! ds )
                    return 0;
                return new internal::ActionAliasDataSource<T>(action, ds.get());
            }

            virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const {
                if (source) {
                    typename internal::AssignableDataSource<DataType>::shared_ptr ad
                        = boost::dynamic_pointer_cast< internal::AssignableDataSource<DataType> >( source );
                    if (ad)
                        return new Property<DataType>(name, desc, ad );
                    else {
                        //log(Error) <<"Failed to build 'Property<"<< this->tname <<"> "<<name<<"' from given DataSourceBase. Returning default."<<endlog();
                    }
                }
                return new Property<DataType>(name, desc, DataType());
            }

            virtual base::DataSourceBase::shared_ptr buildValue() const {
                return new internal::ValueDataSource<DataType>();
            }
            virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const {
                return new internal::ReferenceDataSource<DataType>(*static_cast<DataType*>(ptr));
            }

        };

    }
}

#endif
