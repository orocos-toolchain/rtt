/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  DataSourceCommand.hpp 

                        DataSourceCommand.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef DATASOURCECOMMAND_HPP
#define DATASOURCECOMMAND_HPP

#include "DataSource.hpp"

namespace ORO_CoreLib
{
  class CommandInterface;
}

namespace ORO_Execution
{
  using ORO_CoreLib::CommandInterface;

  /**
   * A class that wraps a Command in a DataSource<bool>
   * interface.
   */
  class DataSourceCommand
    : public DataSource<bool>
  {
      CommandInterface* comm;
      mutable bool mresult;
  public:
    /**
     * DataSourceCommand takes ownership of the command you pass
     * it.
     */
      DataSourceCommand( CommandInterface* c );
      DataSourceCommand( const DataSourceCommand& orig );
      ~DataSourceCommand();
      bool get() const;
      bool value() const;
      void reset();
      CommandInterface* command() const;
      virtual DataSourceCommand* clone() const;
      virtual DataSourceCommand* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
  };
}

#endif
