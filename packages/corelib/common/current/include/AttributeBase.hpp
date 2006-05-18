/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeBase.hpp 

                        AttributeBase.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef ORO_ATTRIBUTE_BASE_HPP
#define ORO_ATTRIBUTE_BASE_HPP

#include "DataSourceBase.hpp"

namespace ORO_CoreLib
{

    /**
     * An attribute is a minimalistic placeholder for data.
     * It is a light-weight equivalent of a Property.
     */
    class AttributeBase
    {
    public:
        virtual ~AttributeBase();

        /**
         * Return a DataSource which contains the same contents.
         */
        virtual DataSourceBase::shared_ptr getDataSource() const = 0;

        /**
         * Returns a clone of this AttributeBase.
         */
        virtual AttributeBase* clone() const = 0;

        /**
         * Returns a copy of this AttributeBase.  Uses the given
         * replacements to copy held DataSources.
         * @param instantiate Set to true to get a copy which will return itself
         * on any future copy request.
         */
        virtual AttributeBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate ) = 0;
    };
}
#endif
