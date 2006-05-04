#ifndef ORO_EXECUTION_SCRIPTING_ACCESS
#define ORO_EXECUTION_SCRIPTING_ACCESS

#include <vector>
#include <string>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    class TaskContext;
    class StatementProcessor;

    /**
     * This interface provides access to the loaded programs
     * and state machines and allows execution of code.
     */
    class ScriptingAccess
    {
    protected:
        TaskContext* mparent;
        StatementProcessor* sproc;
    public:
        ScriptingAccess( TaskContext* parent )
            : mparent(parent), sproc(0)
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
#ifndef ORO_EXECUTION_SCRIPTING_ACCESS
#define ORO_EXECUTION_SCRIPTING_ACCESS

#include <vector>
#include <string>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    class TaskContext;
    class StatementProcessor;

    /**
     * This interface provides access to the loaded programs
     * and state machines and allows execution of code.
     */
    class ScriptingAccess
    {
    protected:
        TaskContext* mparent;
        StatementProcessor* sproc;
    public:
        ScriptingAccess( TaskContext* parent )
            : mparent(parent), sproc(0)
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
#ifndef ORO_EXECUTION_SCRIPTING_ACCESS
#define ORO_EXECUTION_SCRIPTING_ACCESS

#include <vector>
#include <string>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    class TaskContext;
    class StatementProcessor;

    /**
     * This interface provides access to the loaded programs
     * and state machines and allows execution of code.
     */
    class ScriptingAccess
    {
    protected:
        TaskContext* mparent;
        StatementProcessor* sproc;
    public:
        ScriptingAccess( TaskContext* parent )
            : mparent(parent), sproc(0)
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
#ifndef ORO_EXECUTION_SCRIPTING_ACCESS
#define ORO_EXECUTION_SCRIPTING_ACCESS

#include <vector>
#include <string>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    class TaskContext;
    class StatementProcessor;

    /**
     * This interface provides access to the loaded programs
     * and state machines and allows execution of code.
     */
    class ScriptingAccess
    {
    protected:
        TaskContext* mparent;
        StatementProcessor* sproc;
    public:
        ScriptingAccess( TaskContext* parent )
            : mparent(parent), sproc(0)
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
