#ifndef ORO_EXECUTION_SCRIPTING_ACCESS_PROXY_HPP
#define ORO_EXECUTION_SCRIPTING_ACCESS_PROXY_HPP

#include "execution/ScriptingAccess.hpp"
#include "ScriptingAccessC.h"

namespace ORO_Corba
{

    /**
     * A Corba Proxy for the ScriptingAccess class.
     */
    class ScriptingAccessProxy
        : public ORO_Execution::ScriptingAccess
    {
    protected:
        Orocos::ScriptingAccess_var msa;
    public:
        ScriptingAccessProxy( Orocos::ScriptingAccess_ptr sa )
            : ScriptingAccess( 0 ),
              msa( Orocos::ScriptingAccess::_duplicate( sa ) )
        {}
        
        virtual ~ScriptingAccessProxy();

        using ScriptingAccess::loadPrograms;
        using ScriptingAccess::loadStateMachines;

        virtual bool execute( std::string code );

        virtual bool hasProgram( std::string name) const;
        virtual bool loadPrograms( std::string code, std::string filename, bool do_throw = false );
        virtual bool unloadProgram( std::string name, bool do_throw = false );

        virtual std::vector<std::string> getPrograms() const;
        virtual int getProgramLine(std::string name) const;
        virtual std::string getProgramText(std::string name ) const;
        virtual std::string getProgramStatus(std::string name ) const;

        virtual bool loadStateMachines( std::string code, std::string filename, bool do_throw = false );
        virtual bool unloadStateMachine( std::string name, bool do_throw = false );

        virtual bool hasStateMachine( std::string name) const;
        virtual std::vector<std::string> getStateMachines() const;
        virtual std::string getStateMachineText(std::string name ) const;
        virtual std::string getStateMachineStatus(std::string name ) const;
        virtual int getStateMachineLine(std::string name ) const;
        virtual std::string getCurrentState(std::string name ) const;
    };
}

#endif
