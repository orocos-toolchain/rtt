/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  CommandIllegal.cxx 

                        CommandIllegal.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#include "execution/CommandIllegal.hpp"

#include <iostream>
namespace ORO_Execution
{
    using namespace std;

    CommandIllegal::CommandIllegal( const std::string& cmd )
            : illCmd( cmd ), description( "" )
    {}

    CommandIllegal::CommandIllegal( const std::string& cmd, const std::string& descr )
            : illCmd( cmd ), description( descr )
    {}

    void CommandIllegal::execute()
    {
        cout << "Tried to execute an illegal command: " << endl;
        cout << illCmd << endl;
        cout << description << endl;
    }

    void CommandIllegal::warn()
    {
        cout << "Tried to create an illegal command: " << endl;
        cout << illCmd << endl;
        cout << description << endl;
    }

    std::string CommandIllegal::toString()
    {
        return "CommandIllegal";
    }
}
