/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  Types.hpp 

                        Types.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
#ifndef ORO_CORELIB_TYPES_HPP
#define ORO_CORELIB_TYPES_HPP

#include <map>
#include <string>
#include "DataSourceBase.hpp"
#include <boost/shared_ptr.hpp>
#include "AttributeBase.hpp"

namespace CORBA{
    class Any;
}

namespace Orocos {
    class Expression;
}

namespace ORO_CoreLib
{
    class PropertyBase;
    class PropertyBag;
    class AttributeBase;

    /**
     * \file We need some information on types if we want to make
     * properties, variables or corba types of them, the classes in this file
     * provide that information.
     */

    /**
     * A class for representing a user type, and which can build
     * instances of that type.
     */
    class TypeInfo
    {
    public:
        virtual ~TypeInfo();
        /**
         * Build a non modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual AttributeBase* buildConstant(ORO_CoreLib::DataSourceBase::shared_ptr, int sizehint) const;
        virtual AttributeBase* buildConstant(ORO_CoreLib::DataSourceBase::shared_ptr) const = 0;
        /**
         * Build a modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual AttributeBase* buildVariable(int sizehint) const;
        virtual AttributeBase* buildVariable() const = 0;
        /**
         * build an alias with b as the value.  If b is of the wrong type,
         * 0 will be returned..
         */
        virtual AttributeBase* buildAlias( ORO_CoreLib::DataSourceBase::shared_ptr b ) const = 0;

        /**
         * Build a Property of this type.
         */
        virtual PropertyBase* buildProperty(const std::string& name, const std::string& desc, DataSourceBase::shared_ptr source = 0) const = 0;

        /**
         * Build an Attribute of this type.
         */
        virtual AttributeBase* buildAttribute( DataSourceBase::shared_ptr source = 0 ) const = 0;

        /**
         * Build a ValueDataSource of this type.
         */
        virtual DataSourceBase::shared_ptr buildValue() const = 0;

        /**
         * Output this datasource as a human readable string.
         * The default just writes the type name in parentheses to \a os.
         */
        virtual std::ostream& write(std::ostream& os, DataSourceBase::shared_ptr in ) const = 0;

        /**
         * Usability function which converts data to a string.
         */
        virtual std::string toString( DataSourceBase::shared_ptr in ) const;

        /**
         * Decompose a structure as basic components into a PropertyBag.
         * @retval true decomposition resulted in new types added to targetbag.
         * @retval false nothing was added to targetbag.
         */
        virtual bool decomposeType( DataSourceBase::shared_ptr source, ORO_CoreLib::PropertyBag& targetbag ) const = 0;

        /**
         * Compose a structure from a PropertyBase containing its basic components.
         * The default behavior tries to assign \a source to \a target. If this does
         * not work, because source and target have different type, this function returns false.
         */
        virtual bool composeType( DataSourceBase::shared_ptr source, DataSourceBase::shared_ptr target) const = 0;

        /**
         * Create an Any object which contains the value of \a source.
         */
        virtual CORBA::Any* createAny(DataSourceBase::shared_ptr source) const = 0;

        /**
         * Update \a target with the contents of \a any.
         */
        virtual bool update(const CORBA::Any& any, DataSourceBase::shared_ptr target) const = 0;

        /**
         * Create a DataSource which is a proxy for a Corba Expression object.
         */
        virtual DataSourceBase* buildCorbaProxy( Orocos::Expression* e ) const = 0;

        /**
         * Return unique the type name.
         */
        virtual const std::string& getTypeName() const = 0;
    };

    /**
     * This class contains all known types to Orocos.
     * @see TemplateTypeInfo to add your own classes to Orocos.
     */
    class TypeInfoRepository
    {
        TypeInfoRepository();
        typedef std::map<std::string, TypeInfo*> map_t;
        map_t data;
    public:
        ~TypeInfoRepository();
        typedef boost::shared_ptr<TypeInfoRepository> shared_ptr;
        static shared_ptr Instance();
        /**
         * Retrieve a type with a given \a name.
         */
        TypeInfo* type( const std::string& name ) const;

        /**
         * Add a type to the Orocos type repository.
         */
        bool addType( TypeInfo* );
    };
}

#endif
