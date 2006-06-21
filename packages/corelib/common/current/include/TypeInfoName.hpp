#ifndef ORO_TYPEINFO_NAME_HPP
#define ORO_TYPEINFO_NAME_HPP

#include "Types.hpp"
#include "Logger.hpp"

namespace ORO_CoreLib
{
    /**
     * This helper class allows only type names to be added to Orocos.
     * @warning OR use this class OR use TemplateTypeInfo to describe
     * your type, not both.
     * @see TemplateTypeInfo for adding full type info to Orocos.
     */
    template<typename T>
    class TypeInfoName
        : public TypeInfo
    {
        const std::string tname;
    public:
        /**
         * The given \a T parameter is the type for reading DataSources.
         */
        typedef T UserType;

        /**
         * Setup Type Name Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         * 
         */
        TypeInfoName(std::string name) 
            : tname(name)
        {
            Logger::In in("TypeInfoName");
            // Install the type info object for T.
            if ( ORO_CoreLib::detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject != 0) {
                Logger::log() << Logger::Warning << "Overriding TypeInfo for '" 
                              << ORO_CoreLib::detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject->getTypeName()
                              << "'." << Logger::endl;
                delete ORO_CoreLib::detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject;
            }
            ORO_CoreLib::detail::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = this;
        }

        AttributeBase* buildConstant(DataSourceBase::shared_ptr dsb) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Constant of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        AttributeBase* buildVariable() const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Variable of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        AttributeBase* buildAttribute( DataSourceBase::shared_ptr in) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Attribute of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        AttributeBase* buildAlias( DataSourceBase::shared_ptr in ) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Alias of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual const std::string& getTypeName() const { return tname; }

        virtual PropertyBase* buildProperty(const std::string& name, const std::string& desc, DataSourceBase::shared_ptr source = 0) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Property of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual DataSourceBase::shared_ptr buildValue() const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Property of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual std::ostream& write( std::ostream& os, DataSourceBase::shared_ptr in ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not write type "<<tname<<"."<<Logger::endl;
            return os;
        }

        virtual bool decomposeType( DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not decompose type "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual bool composeType( DataSourceBase::shared_ptr source, DataSourceBase::shared_ptr result) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not compose type "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual CORBA::Any* createAny(DataSourceBase::shared_ptr source) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not create Any of type "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual bool update(const CORBA::Any& any, DataSourceBase::shared_ptr target) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not update type "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual DataSourceBase* buildCorbaProxy( Orocos::Expression* e ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not create Corba Proxy of type "<<tname<<"."<<Logger::endl;
            return 0;
        }
    };

}

#endif
