#ifndef ORO_TASKBROWSER_HPP
#define ORO_TASKBROWSER_HPP
 

#include "MethodFactoryInterface.hpp"
#include "DataSourceFactoryInterface.hpp"
#include "CommandFactoryInterface.hpp"
#include <deque>
#include <string>
#include <vector>


#pragma interface

namespace ORO_CoreLib {
    class ConditionInterface;
    class CommandInterface;
}

namespace ORO_Execution
{
    class TaskContext;
    class DataSourceBase;

    /**
     * @brief This class allows a text client to browse the
     * peers of a TaskContext and execute commands.
     */
    class TaskBrowser
    {
        // the 'current' task context
        static TaskContext* taskcontext;
        // the TC we are using for completion.
        static TaskContext* peer;
        ORO_CoreLib::ConditionInterface* condition;
        ORO_CoreLib::CommandInterface*   command;
        const CommandFactoryInterface* command_fact;
        const DataSourceFactoryInterface* datasource_fact;
        const MethodFactoryInterface* method_fact;

        std::string prompt;
        std::string coloron;
        std::string coloroff;

        /* A static variable for holding the line. */
        char *line_read;
        int lastc; // last command's number

        std::deque<TaskContext*> taskHistory;

        /* Read a string, and return a pointer to it.
           Returns NULL on EOF. */
        char *rl_gets ();

        static std::vector<std::string> completes;
        static std::vector<std::string>::iterator complete_iter;

        static std::string component;
        static std::string object;
        static std::string peerpath;
        static std::string method;
        static std::string datasource;
        static std::string text;

        // helper function
        static char* dupstr( const char *s );

        static TaskContext* findPeer( std::string comm );

        static void find_completes();
        
        static void find_command(std::string::size_type startpos);
        static void find_peers(std::string::size_type startpos);

        static void find_datasource(std::string::size_type startpos);

        static void find_method(std::string::size_type startpos);

        static char ** orocos_hmi_completion ( const char *text, int start, int end );

        void switchBack();

        void switchTask(std::string& c);

        void evalCommand(std::string& comm );

        void printResult( DataSourceBase* ds);

        void printHelp();
        
        void printCommand( const std::string m );
                
        void printSource( const std::string m );
                
        void printMethod( const std::string m );
    public :
        TaskBrowser( TaskContext* _c );

        ~TaskBrowser();

        /**
         * @brief Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop();

        static char *command_generator( const char *_text, int state );

    };

}

#endif
