/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ParserScriptingAccess.hpp

                        ParserScriptingAccess.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_EXECUTION_PARSER_SCRIPTING_ACCESS_HPP
#define ORO_EXECUTION_PARSER_SCRIPTING_ACCESS_HPP

#include "../ScriptingAccess.hpp"
#include "../OperationInterface.hpp"
#include "../rtt-config.h"

namespace RTT
{
    class StatementProcessor;

    /**
     * This implementation provides access to the loaded programs
     * and state machines and allows execution of code, using the
     * Orocos parser framework.
     */
    class RTT_API ParserScriptingAccess
        : public ScriptingAccess
    {
    protected:
        StatementProcessor* sproc;
        bool doExecute(const std::string& code);

        bool doLoadPrograms( std::string filename );
        bool doLoadProgramText( std::string code );
        bool doUnloadProgram( std::string name );

        bool doLoadStateMachines( std::string filename );
        bool doLoadStateMachineText( std::string code );
        bool doUnloadStateMachine( std::string name );

        OperationInterface* createTaskObject(OperationInterface* obj);
    public:
        ParserScriptingAccess( TaskContext* parent );

        virtual ~ParserScriptingAccess();

        virtual int execute( const std::string& code );

        virtual DispatchInterface::shared_ptr getCommand( int ticket );

        virtual Functions loadFunctions(std::string filename, bool do_throw = false );
        virtual Functions loadFunctions(std::string code, std::string filename, bool do_throw = false );

        virtual bool loadPrograms( std::string filename, bool do_throw = false );
        virtual bool loadPrograms( std::string code, std::string filename, bool do_throw = false );
        virtual bool unloadProgram( std::string name, bool do_throw = false );

        virtual bool loadStateMachines( std::string filename, bool do_throw = false  );
        virtual bool loadStateMachines( std::string code, std::string filename, bool do_throw = false  );
        virtual bool unloadStateMachine( std::string name, bool do_throw = false );

    };
}

#endif
