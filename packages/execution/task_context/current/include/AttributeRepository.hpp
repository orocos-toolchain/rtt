/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeRepository.hpp 

                        AttributeRepository.hpp -  description
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
 
 
#ifndef ATTRIBUTEREPOSITORY_HPP
#define ATTRIBUTEREPOSITORY_HPP

#include <memory>
#include <map>
#include "TaskAttribute.hpp"

namespace ORO_Execution
{
    class TaskAttributeBase;
    class DataSourceBase;

    /**
     * @brief A class for keeping track of values/variables.
     *
     * It stores types in a map,
     * so that they can be referenced to later on.
     */
  struct AttributeRepository
  {
    typedef std::map<std::string, TaskAttributeBase*> map_t;
    map_t values;

    AttributeRepository();
    ~AttributeRepository();
      /**
       * Erases the whole repository.
       */
    void clear();

    bool isDefined( const std::string& name ) const;

      template<class T>
      void addConstant( const std::string& name, T value )
      {
          setValue( name, new TaskConstant<T>( value ) );
      }

      template<class T>
      void addAttribute( const std::string& name, T value )
      {
          setValue( name, new TaskAttribute<T>( value ));
      }

      /**
       * The most common way to add or remove a variable to the repository.
       */
    void setValue( const std::string& name, TaskAttributeBase* pc );
    void removeValue( const std::string& name );
    /**
     * Get the value with name name.  If no such value exists, this
     * returns 0.
     */
    TaskAttributeBase* getValue( const std::string& name );

      /**
       * Return a new copy of this repository with the copy operation semantics.
       * @see CommandInterface
       */
      AttributeRepository* copy( std::map<const DataSourceBase*, DataSourceBase*>& repl ) const;

      /**
       * Return a list of all attributes.
       */
      std::vector<std::string> attributes() const;
//       const AttributeRepository& operator=(const AttributeRepository& orig) {
//           this->values = orig.values;
//       }
          
  };
}

#endif
