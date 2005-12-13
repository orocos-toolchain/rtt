#ifndef ORO_PROGRAM_EXCEPTIONS_HPP
#define ORO_PROGRAM_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace ORO_Execution
{
    class ProgramInterface;

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
