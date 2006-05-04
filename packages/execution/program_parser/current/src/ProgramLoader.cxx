/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  ProgramLoader.cxx 

                        ProgramLoader.cxx -  description
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
 
 

#include "execution/ProgramLoader.hpp"
#include "execution/Parser.hpp"
#include "execution/ParsedStateMachine.hpp"
#include "corelib/Logger.hpp"

#include <iostream>
#include <fstream>

using namespace boost;
using namespace std;
using namespace ORO_CoreLib;

namespace ORO_Execution
{
    ProgramLoader::ProgramLoader(bool rethrow /*=false*/)
        : mrethrow(rethrow)
    {}


  ProgramLoader::Functions ProgramLoader::loadFunction( const std::string& file, TaskContext* c)
  {
      return c->scripting()->loadFunctions( file, mrethrow );
  }

  ProgramLoader::Functions ProgramLoader::loadFunction(const std::string& s, TaskContext* c, const std::string& filename)
  {
      return c->scripting()->loadFunctions( s, filename, mrethrow );
  }

  bool ProgramLoader::loadProgram( const std::string& file, TaskContext* c)
  {
      return c->scripting()->loadPrograms( file, mrethrow );
  }

  bool ProgramLoader::loadProgram( const std::string& s, TaskContext* c, const std::string& filename)
  {
      return c->scripting()->loadStateMachines(s, filename, mrethrow);
  }

    bool ProgramLoader::unloadProgram( const std::string& name, TaskContext* target)
    {
        return target->scripting()->unloadProgram(name);
    }

    bool ProgramLoader::unloadStateMachine( const std::string& name, TaskContext* target)
    {
        return target->scripting()->unloadStateMachine(name);
    }

  bool ProgramLoader::loadStateMachine( const std::string& file, TaskContext* c)
  {
      return c->scripting()->loadStateMachines( file, mrethrow );
  }

    bool ProgramLoader::loadStateMachine( const std::string& s, TaskContext* c, const std::string& filename)
    {
        return c->scripting()->loadStateMachines(s, filename, mrethrow);
    }
}
