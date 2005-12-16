#ifndef ORO_PROGRAM_EXCEPTIONS_HPP
#define ORO_PROGRAM_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace ORO_Execution
{
    /**
     * This exception is thrown when a program or state machine
     * could not be loaded into a ProgramProcessor or StateMachineProcessor.
     */
    class program_load_exception
        : public std::exception
    {
        std::string merror;
    public:
        program_load_exception( const std::string& error )
            : merror( error )
        {
        }
        ~program_load_exception() throw() {}
        const char* what() const throw()
        {
            return merror.c_str();
        }
    };

    /**
     * This exception is thrown when a program or state machine
     * could not be unloaded into a ProgramProcessor or StateMachineProcessor.
     */
    class program_unload_exception
        : public std::exception
    {
        std::string merror;
    public:
        program_unload_exception( const std::string& error )
            : merror( error )
        {
        }
        ~program_unload_exception() throw() {}
        const char* what() const throw()
        {
            return merror.c_str();
        }
    };
}
#endif
