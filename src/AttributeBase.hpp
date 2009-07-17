/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeBase.hpp

                        AttributeBase.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#ifndef ORO_ATTRIBUTE_BASE_HPP
#define ORO_ATTRIBUTE_BASE_HPP

#include "DataSourceBase.hpp"

namespace RTT
{

    /**
     * An attribute is a minimalistic, named placeholder for data.
     * It is a light-weight equivalent of a Property.
     */
    class RTT_API AttributeBase
    {
    protected:
        std::string mname;
    public:
        /**
         * Create a nameless AttributeBase.
         */
        AttributeBase();

        /**
         * Create an AttributeBase known by a name.
         *
         * @param name The name.
         */
        AttributeBase(const std::string& name);

        virtual ~AttributeBase();

        /**
         * Get the name of this instance.
         *
         * @return the name
         */
        const std::string& getName() const;

        /**
         * Returns true if the Attribute was correctly
         * initialised.
         */
        bool ready() const
        {
            return this->getDataSource();
        }

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
