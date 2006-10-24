/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  DataSourceCommand.cxx 

                        DataSourceCommand.cxx -  description
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
 
 
#include "DataSourceCommand.hpp"

#include "CommandInterface.hpp"

namespace RTT
{
  DataSourceCommand::DataSourceCommand( CommandInterface* c )
      : comm( c ), mresult(false)
  {
  }

  DataSourceCommand::DataSourceCommand( const DataSourceCommand& orig )
    : comm( orig.command()->clone() ), mresult(false)
  {
  }

  DataSourceCommand::~DataSourceCommand()
  {
    delete comm;
  }

  bool DataSourceCommand::get() const
  {
      comm->readArguments();
      mresult = comm->execute();
      return mresult;
  }

  bool DataSourceCommand::value() const
  {
    return mresult;
  }

  CommandInterface* DataSourceCommand::command() const
  {
      return comm;
  }

  void DataSourceCommand::reset()
  {
      mresult = false;
      comm->reset();
  }

  DataSourceCommand* DataSourceCommand::clone() const
  {
      return new DataSourceCommand( comm->clone() );
  }

  DataSourceCommand* DataSourceCommand::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
  {
      return new DataSourceCommand( comm->copy( alreadyCloned ) );
  }
}
