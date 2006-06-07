#ifndef ORO_EXECUTION_PARSER_SCRIPTING_ACCESS_HPP
#define ORO_EXECUTION_PARSER_SCRIPTING_ACCESS_HPP

#include "ScriptingAccess.hpp"

namespace ORO_Execution
{
    class StatementProcessor;

    /**
     * This implementation provides access to the loaded programs
     * and state machines and allows execution of code, using the
     * Orocos parser framework.
     */
    class ParserScriptingAccess
        : public ScriptingAccess
    {
        StatementProcessor* sproc;
    public:
        ParserScriptingAccess( TaskContext* parent )
            : ScriptingAccess(parent), sproc(0)
        {}
        
        virtual ~ParserScriptingAccess();

        virtual bool execute( std::string code );

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
