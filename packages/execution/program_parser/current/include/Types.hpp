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
 
#ifndef ORO_TYPES_HPP
#define ORO_TYPES_HPP

#include <map>
#include <string>
#include <corelib/DataSource.hpp>

namespace ORO_Execution
{
  class TaskAttributeBase;

  /**
   * \file We need some information on types if we want to make
   * constants, variables or aliases of them, the classes in this file
   * provide that information.
   */

    /**
     * A class for representing a parser type, and which can build
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
        virtual TaskAttributeBase* buildConstant(int sizehint) const;
        virtual TaskAttributeBase* buildConstant() const = 0;
        /**
         * Build a modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual TaskAttributeBase* buildVariable(int sizehint) const;
        virtual TaskAttributeBase* buildVariable() const = 0;
        /**
         * build an alias with b as the value.  If b is of the wrong type,
         * 0 will be returned..
         */
        virtual TaskAttributeBase* buildAlias( ORO_CoreLib::DataSourceBase* b ) const = 0;
    };

    /**
     * This class contains all known types to the parser.
     */
    class TypeInfoRepository
    {
        TypeInfoRepository();
        ~TypeInfoRepository();
        typedef std::map<std::string, TypeInfo*> map_t;
        map_t data;
    public:
        static TypeInfoRepository& instance();
        /**
         * Retrieve a type with a given \a name.
         */
        TypeInfo* type( const std::string& name ) const;
    };
}

#endif
