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

namespace ORO_Execution
{
  using ORO_CoreLib::CommandInterface;
  using ORO_Execution::DataSource;
  using ORO_Execution::VariableDataSource;

  /**
   * This is a command that will assign the value of an expression to
   * a variable at runtime.  You pass it the VariableDataSource that
   * you want to assign to, and the DataSource that you want to assign
   * on construction, and it will take care of the assignment.  Note
   * that both DataSource's need to be of the same type, and this
   * class needs that type as its template parameter..
   */
  template<typename T>
  class AssignVariableCommand
    : public CommandInterface
  {
      typedef typename VariableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<T>::shared_ptr RHSSource;
      RHSSource rhs;
  public:
      AssignVariableCommand( VariableDataSource<T>* l, DataSource<T>* r )
          : lhs( l ), rhs( r )
      {
      }

      bool execute()
      {
        lhs->set( rhs->get() );
        return true;
      }

      virtual CommandInterface* clone() const
      {
          return new AssignVariableCommand( lhs.get(), rhs.get() );
      }

      virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
          return new AssignVariableCommand( lhs->copy( alreadyCloned ), rhs->copy( alreadyCloned ) );
      }
  };

  template<typename T, typename Index, typename SetType>
  class AssignIndexCommand
    : public CommandInterface
  {
      typedef bool (*Pred)(const T&, Index);

      typedef typename DataSource<Index>::shared_ptr IndexSource;
      IndexSource i;
      typedef typename VariableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<SetType>::shared_ptr RHSSource;
      RHSSource rhs;
      Pred p;
  public:
      AssignIndexCommand( VariableDataSource<T>* l, DataSource<Index>* index, DataSource<SetType>* r, Pred _p )
          : i(index),lhs( l ), rhs( r ), p(_p)
      {
      }

      bool execute()
      {
          Index ind = i->get();
          if ( p(lhs->get(), ind) ) {
              lhs->set()[ ind ] = rhs->get();
              return true;
          }
          return false;
      }

      virtual CommandInterface* clone() const
      {
          return new AssignIndexCommand( lhs.get(), i.get(), rhs.get(), p );
      }

      virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
          return new AssignIndexCommand( lhs->copy( alreadyCloned ), i->copy( alreadyCloned ), rhs->copy( alreadyCloned ), p );
      }
  };
}

#endif
