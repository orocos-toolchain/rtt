/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ScriptingAccess.hpp 

                        ScriptingAccess.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_EXECUTION_SCRIPTING_ACCESS
#define ORO_EXECUTION_SCRIPTING_ACCESS

#include <vector>
#include <string>
#include "ProgramInterface.hpp"

namespace RTT
{
    class TaskContext;

    /**
     * This interface provides access to the loaded programs
     * and state machines and allows execution of code.
     */
    class ScriptingAccess
    {
    protected:
        TaskContext* mparent;
    public:
        ScriptingAccess( TaskContext* parent )
            : mparent(parent)
        {}
        
        virtual ~ScriptingAccess();

        /**
         * Parse and execute a statement.
         */
        virtual bool execute( std::string code );

        /**
         * List of executed functions.
         */
        typedef std::vector< ProgramInterfacePtr > Functions;

        virtual Functions loadFunctions(std::string filename, bool do_throw = false );
        virtual Functions loadFunctions(std::string code, std::string filename, bool do_throw = false );

        virtual bool loadPrograms( std::string filename, bool do_throw = false );
        virtual bool loadPrograms( std::string code, std::string filename, bool do_throw = false );
        virtual bool unloadProgram( std::string name, bool do_throw = false );

        virtual std::vector<std::string> getPrograms() const;
        virtual bool hasProgram(std::string name) const;
        virtual int getProgramLine(std::string name) const;
        virtual std::string getProgramText(std::string name ) const;
        virtual std::string getProgramStatus(std::string name ) const;

        virtual bool loadStateMachines( std::string filename, bool do_throw = false  );
        virtual bool loadStateMachines( std::string code, std::string filename, bool do_throw = false  );
        virtual bool unloadStateMachine( std::string name, bool do_throw = false );

        virtual bool hasStateMachine(std::string name) const;
        virtual std::vector<std::string> getStateMachines() const;
        virtual std::string getStateMachineText(std::string name ) const;
        virtual std::string getStateMachineStatus(std::string name ) const;
        virtual int getStateMachineLine(std::string name ) const;
        virtual std::string getCurrentState(std::string name ) const;
    };
}

#endif
