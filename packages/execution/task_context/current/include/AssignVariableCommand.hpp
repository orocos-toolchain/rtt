/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  AssignVariableCommand.hpp

                        AssignVariableCommand.hpp -  description
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

#ifndef ASSIGNVARIABLECOMMAND_HPP
#define ASSIGNVARIABLECOMMAND_HPP

#include <corelib/CommandInterface.hpp>
#include <execution/DataSource.hpp>
#include <execution/mystd.hpp>

namespace ORO_Execution
{
  using ORO_CoreLib::CommandInterface;

    namespace detail {

  /**
   * This is a command that will assign the value of an expression to
   * a variable at runtime.  You pass it the AssignableDataSource that
   * you want to assign to, and the DataSource that you want to assign
   * on construction, and it will take care of the assignment.  Note
   * that both DataSource's need to be of the same type, and this
   * class needs that type as its template parameter..
   * @param T Target type
   * @param S Source type
   */
  template<typename T, typename S = T>
  class AssignVariableCommand
    : public CommandInterface
  {
      typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<S>::shared_ptr RHSSource;
      RHSSource rhs;
  public:
      AssignVariableCommand( LHSSource l, RHSSource r )
          : lhs( l ), rhs( r )
      {
      }

      void readArguments() {
          rhs->evaluate();
      }

      bool execute()
      {
        lhs->set( rhs->value() );
        return true;
      }

      virtual CommandInterface* clone() const
      {
          return new AssignVariableCommand( lhs.get(), rhs.get() );
      }

      virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new AssignVariableCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
      }
  };

    /**
     * Assign the contents of one variable-size container to another.
     * This class checks for capacity and fails execution if not sufficient.
   * @param T Target type
   * @param S Source type
     */
  template<typename T, typename S = T>
  class AssignContainerCommand
    : public CommandInterface
  {
      typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<S>::shared_ptr RHSSource;
      RHSSource rhs;
  public:
      AssignContainerCommand( LHSSource l, RHSSource r )
          : lhs( l ), rhs( r )
      {
      }

      void readArguments() {
          rhs->evaluate();
      }

      bool execute()
      {
          S result = rhs->value();
          if ( lhs->get().capacity() < result.size() )
              return false;
          lhs->set( result );
          return true;
      }

      virtual CommandInterface* clone() const
      {
          return new AssignContainerCommand( lhs.get(), rhs.get() );
      }

      virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new AssignContainerCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
      }
  };

#if 0
    /**
     * Assign the contents of one string to another, while maintaining capacity of the original.
     * This class checks for capacity and fails execution if not sufficient.
     * @param T Target type
     * @param S Source type
     */
  template<typename T, typename S = T>
  class AssignStringCommand
    : public CommandInterface
  {
      typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<S>::shared_ptr RHSSource;
      RHSSource rhs;
  public:
      AssignStringCommand( LHSSource l, RHSSource r )
          : lhs( l ), rhs( r )
      {
      }

      void readArguments() {
          rhs->evaluate();
      }

      bool execute()
      {
          if ( lhs->get().capacity() < rhs->value().size() || &(lhs->set()) == 0)
              return false;
          lhs->set() = rhs->value().c_str(); // std::string specific ! Does not allocate if S is a (const) std::string&
          return true;
      }

      virtual CommandInterface* clone() const
      {
          return new AssignStringCommand( lhs.get(), rhs.get() );
      }

      virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new AssignStringCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
      }
  };
#endif

  template<typename T, typename Index, typename SetType, typename Pred>
  class AssignIndexCommand
    : public CommandInterface
  {
      typedef typename DataSource<Index>::shared_ptr IndexSource;
      IndexSource i;
      typedef typename AssignableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<SetType>::shared_ptr RHSSource;
      RHSSource rhs;
  public:
      AssignIndexCommand( LHSSource l, IndexSource index, RHSSource r)
          : i(index),lhs( l ), rhs( r )
      {
      }

      void readArguments() {
          rhs->evaluate();
      }

      bool execute()
      {
          Index ind = i->get();
          if ( Pred()( lhs->get(), ind) && &(lhs->set()) != 0 ) {
              lhs->set()[ ind ] = rhs->get();
              return true;
          }
          return false;
      }

      virtual CommandInterface* clone() const
      {
          return new AssignIndexCommand( lhs.get(), i.get(), rhs.get() );
      }

      virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
          return new AssignIndexCommand( lhs->copy( alreadyCloned ), i->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
      }
  };
}}

#endif
