/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  TypeInfoName.hpp 

                        TypeInfoName.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_TYPEINFO_NAME_HPP
#define ORO_TYPEINFO_NAME_HPP

#include "Types.hpp"
#include "DataSourceTypeInfo.hpp"
#include "Logger.hpp"
#include <pkgconf/system.h>

namespace ORO_CoreLib
{
    /**
     * Empty implementation of TypeInfo interface.
     */
    class EmptyTypeInfo
        : public TypeInfo
    {
        const std::string tname;
    public:
        EmptyTypeInfo(std::string name) 
            : tname(name)
        {
        }

        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

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
#if HAVE_STREAMS
                std::string output = std::string("(")+ in->getTypeName() +")";
                os << output;
#endif
            return os;
        }

        virtual bool decomposeType( DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not decompose "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual bool composeType( DataSourceBase::shared_ptr source, DataSourceBase::shared_ptr result) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not compose "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual CORBA::Any* createAny(DataSourceBase::shared_ptr source) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not create Any of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual bool update(const CORBA::Any& any, DataSourceBase::shared_ptr target) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not update type "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual DataSourceBase* buildCorbaProxy( Orocos::Expression* e ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not create Corba Proxy of "<<tname<<"."<<Logger::endl;
            return 0;
        }
    };

    /**
     * This helper class allows only type names to be added to Orocos.
     * @warning OR use this class OR use TemplateTypeInfo to describe
     * your type, not both.
     * @see TemplateTypeInfo for adding full type info to Orocos.
     */
    template<typename T>
    struct TypeInfoName 
        : public EmptyTypeInfo
    {
        /**
         * Setup Type Name Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         * 
         */
        TypeInfoName(std::string name) 
            : EmptyTypeInfo(name) 
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
    };

}

#endif
