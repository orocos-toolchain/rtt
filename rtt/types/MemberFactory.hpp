/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 MemberFactory.hpp

                        MemberFactory.hpp -  description
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


#ifndef ORO_MEMBER_FACTORY_HPP
#define ORO_MEMBER_FACTORY_HPP

#include "rtt/base/DataSourceBase.hpp"
#include "rtt/rtt-config.h"
#include "rtt/internal/Reference.hpp"
#include <vector>
#include <string>

namespace RTT
{
    namespace types
    {

        class RTT_API MemberFactory
        {
        public:
        virtual ~MemberFactory() {};

        /**
         * @name Inspecting data structures.
         * Used to write a complex type to an external representation, like XML.
         * @{
         */

        /**
         * Tries to resize a data source in case it's a resizable sequence.
         * @return true if the resizing could be done, false otherwise.
         */
        virtual bool resize(base::DataSourceBase::shared_ptr arg, int size) const;

        /**
         * Returns the list of struct member names of this type.
         * In case this type is not a struct, returns an empty list.
         */
        virtual std::vector<std::string> getMemberNames() const;

        /**
         * Returns a member of a given data source struct identified by its name.
         * @param item The item of which to return a reference to a member
         * @param name The name of a member within \a item. Is a name of a member in case of a struct
         * or an index number in case of a sequence.
         * @return null if no such member exists, an assignable datasource referencing that member otherwise.
         */
        virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const;

        /**
         * Returns a member of a given data source identified by a data source \a id. This will be an
         * int (for indexing) or a string (for the member name).
         * @warning Currently this only works for a SequenceTypeInfo-like object. We don't
         * implement this for StructTypeInfo-like objects, since that would be a very expensive
         * implementation (memory wise).
         *
         * @param item The item of which to return a member
         * @param id   Or a string data source containing the name of a member if item is a struct,
         * Or an unsigned int data source containing the index of an element if item is a sequence
         */
        virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                           base::DataSourceBase::shared_ptr id) const;


        /**
         * Stores a reference to a member of a struct identified by its name.
         * This method does not allocate memory when \a item is assignable..
         * @param ref The reference object in which to store the reference.
         * @param item The item of which to return a reference to a member. It must be assignable, otherwise,
         * a reference to a copy of item will be returned. This copy \b will allocate memory.
         * @param name The name of a member within \a item.
         * @return false if no such member exists, true if ref got filled in otherwise.
         */
        virtual bool getMember(internal::Reference* ref, base::DataSourceBase::shared_ptr item, const std::string& name) const;
        /** @} */
        };

        typedef boost::shared_ptr<MemberFactory> MemberFactoryPtr;
    }
}
#endif
