/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  DataSourceCommand.hpp

                        DataSourceCommand.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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


#ifndef DATASOURCECOMMAND_HPP
#define DATASOURCECOMMAND_HPP

#include "../internal/DataSource.hpp"
#include "../internal/CommandInterface.hpp"

namespace RTT
{

  /**
   * A class that wraps a Command in a DataSource<bool>
   * interface.
   */
  class RTT_API DataSourceCommand
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
