/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  TypeInfoName.hpp

                        TypeInfoName.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_TYPEINFO_NAME_HPP
#define ORO_TYPEINFO_NAME_HPP

#include "Types.hpp"
#include "../internal/DataSourceTypeInfo.hpp"
#include "../Logger.hpp"
#include "../rtt-config.h"

namespace RTT
{ namespace types {

    /**
     * Empty implementation of TypeInfo interface.
     */
    class RTT_API EmptyTypeInfo
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

        base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr dsb) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Constant of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        base::AttributeBase* buildVariable(std::string name) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Variable of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        base::AttributeBase* buildAttribute(std::string name, base::DataSourceBase::shared_ptr in) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Attribute of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr in ) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Alias of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* act, base::DataSourceBase::shared_ptr in ) const
        {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build ActionAlias of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual const std::string& getTypeName() const { return tname; }

        virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build Property of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual base::DataSourceBase::shared_ptr buildValue() const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build internal::ValueDataSource of "<<tname<<"."<<Logger::endl;
            return 0;
        }

        virtual base::DataSourceBase::shared_ptr buildReference(void*) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not build internal::ReferenceDataSource of "<<tname<<"."<<Logger::endl;
            return 0;
        }


        base::DataSourceBase::shared_ptr construct(const std::vector<base::DataSourceBase::shared_ptr>& ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not construct value of "<<tname<<"."<<Logger::endl;
            return base::DataSourceBase::shared_ptr();
        }

        virtual base::DataSourceBase::shared_ptr getAssignable(base::DataSourceBase::shared_ptr arg) const {
            return base::DataSourceBase::shared_ptr();
        }

        virtual std::ostream& write( std::ostream& os, base::DataSourceBase::shared_ptr in ) const {
            Logger::In loc("TypeInfoName");
#ifdef OS_HAVE_STREAMS
                std::string output = std::string("(")+ in->getTypeName() +")";
                os << output;
#endif
            return os;
        }

        virtual std::istream& read( std::istream& is, base::DataSourceBase::shared_ptr out ) const {
            Logger::In loc("TypeInfoName");
            return is;
        }

        virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not decompose "<<tname<<"."<<Logger::endl;
            return false;
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            Logger::In loc("TypeInfoName");
            Logger::log() << Logger::Error << "Can not compose "<<tname<<"."<<Logger::endl;
            return false;
        }

		virtual std::string getTypeIdName() const { return ""; }

        virtual base::InputPortInterface* inputPort(std::string const& name) const
        { return 0; }
        virtual base::OutputPortInterface* outputPort(std::string const& name) const
        { return 0; }
        virtual base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy) const
        { return 0; }
        virtual base::ChannelElementBase* buildChannelOutput(base::InputPortInterface& port) const
        { return 0; }
        virtual base::ChannelElementBase* buildChannelInput(base::OutputPortInterface& port) const
        { return 0; }

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
            if ( internal::DataSourceTypeInfo<T>::TypeInfoObject != 0) {
                Logger::log() << Logger::Warning << "Overriding TypeInfo for '"
                              << internal::DataSourceTypeInfo<T>::TypeInfoObject->getTypeName()
                              << "'." << Logger::endl;
            }
            internal::DataSourceTypeInfo<T>::TypeInfoObject = this;
        }
    };

}}

#endif
