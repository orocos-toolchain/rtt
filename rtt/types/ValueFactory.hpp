/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 ValueFactory.hpp

                        ValueFactory.hpp -  description
                           -------------------
    begin                : Fri Mar 16 2012
    copyright            : (C) 2012 Peter Soetens
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


#ifndef RTT_VALUE_TYPE_FACTORY
#define RTT_VALUE_TYPE_FACTORY

#include "../base/DataSourceBase.hpp"
#include "../base/AttributeBase.hpp"
#include "../base/PropertyBase.hpp"
#include "../rtt-config.h"
#include <vector>
#include <string>

namespace RTT
{
    namespace types {

        /**
         * Creates all value objects of a certain type.
         */
        class RTT_API ValueFactory
        {
        public:
            virtual ~ValueFactory() {}
        /**
         * @name Type building/factory functions
         * Used to create objects that hold data of a certain type.
         * @{
         */
        /**
         * Build a non modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr, int sizehint) const;
        virtual base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr) const = 0;
        /**
         * Build a modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual base::AttributeBase* buildVariable(std::string name,int sizehint) const;
        virtual base::AttributeBase* buildVariable(std::string name) const = 0;

        /**
         * build an alias with b as the value.  If b is of the wrong type,
         * 0 will be returned..
         */
        virtual base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr b ) const = 0;

        /**
         * Build a Property of this type.
         */
        virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const = 0;

        /**
         * Build an Attribute of this type.
         */
        virtual base::AttributeBase* buildAttribute(std::string name, base::DataSourceBase::shared_ptr source = 0 ) const = 0;

        /**
         * Build a internal::ValueDataSource of this type.
         */
        virtual base::DataSourceBase::shared_ptr buildValue() const = 0;

        /** Build a internal::ReferenceDataSource of this type, pointing to the given
         * pointer
         */
        virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const = 0;

        /**
         * Returns a DataSource that first executes an action and returns the result of another data source.
         * If \a source is an AssignableDataSource, an AssignableDataSource is returned of the same type, otherwise,
         * a plain DataSource is returned.
         */
        virtual base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* action, base::DataSourceBase::shared_ptr source) const = 0;
        /** @} */

        };

        typedef boost::shared_ptr<ValueFactory> ValueFactoryPtr;
    }
}

#endif
