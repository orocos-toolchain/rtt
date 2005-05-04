#ifndef ORO_PROGRAM_LOADER_HPP
#define ORO_PROGRAM_LOADER_HPP

#include <string>
#include "TaskContext.hpp"

namespace ORO_Execution
{
    /**
     * This class loads and unloads Orocos Program Script and Orocos State Description files
     * to a TaskContext's Processor.
     * It wraps around Parser and can process both files and C++ streams.
     * Parse exceptions are directed to the ORO_CoreLib::Logger class.
     */
    class ProgramLoader
    {
    public:
        /**
         * List of executed functions.
         */
        typedef std::vector<FunctionGraph*> Functions;

        /**
         * Executes all not exported functions in \a target's
         * Processor or loads exported functions in \a target's Command Interface
         * @return All functions being executed.
         */
        Functions executeFunction( std::istream& s, TaskContext* target, const std::string& filename = "stream" );

        /**
         * Executes all not exported functions in \a target's
         * Processor or loads exported functions in \a target's Command Interface.
         * @return All functions being executed.
         */
        Functions executeFunction( const std::string& filename, TaskContext* target);

        /**
         * Load Programs and/or exported Functions from an Orocos Program Script into a TaskContext.
         * Reads out the stream \a s , parses it, and loads it in \a target's Processor.
         * @return true if all programs and functions could be loaded.
         */
        bool loadProgram( std::istream& s, TaskContext* target, const std::string& filename = "stream" );

        /**
         * Load Programs and/or exported Functions from an Orocos Program Script into a TaskContext.
         * Reads out the local file \a filename , parses it, and loads it in \a target's Processor.
         * @return true if all programs and functions could be loaded.
         */
        bool loadProgram( const std::string& filename, TaskContext* target);

        /**
         * Unload a Program from a TaskContext.
         * @param name the name of the program.
         */
        bool unloadProgram( const std::string& name, TaskContext* target);

        /**
         * Load StateMachines from an Orocos State Description into a TaskContext.
         * Reads out the stream \a s , parses it, and loads it in \a target's Processor.
         * @return true if all state machines could be loaded.
         */
        bool loadStateMachine(std::istream& s, TaskContext* target, const std::string& filename = "stream" );

        /**
         * Load StateMachines from an Orocos State Description into a TaskContext.
         * Reads out the local file \a filename , parses it, and loads it in \a target's Processor.
         * @return true if all state machines could be loaded.
         */
        bool loadStateMachine(const std::string& filename, TaskContext* target);

        /**
         * Unload a root (parent) StateMachine from a TaskContext.
         * @param name the name of the StateMachine.
         */
        bool unloadStateMachine( const std::string& name, TaskContext* target);
    };
}

#endif
