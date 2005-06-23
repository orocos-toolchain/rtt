#include "execution/FactoryExceptions.hpp"
#include <sstream>

namespace ORO_Execution
{

    name_not_found_exception::name_not_found_exception( const std::string& n )
          : name(n)
    {
        whatstr = name + " not found in this factory.";
    }

    name_not_found_exception::~name_not_found_exception() throw() {}

    const char* name_not_found_exception::what() const throw() {
        return whatstr.c_str();
    }

    wrong_number_of_args_exception::wrong_number_of_args_exception( int w, int r )
          : wanted( w ), received( r )
      {
        std::stringstream a;
        a << "Wrong number of arguments: Expected ";
        a << w;
        a << ", received ";
        a << r << ".";
        whatstr = a.str();
      }

    wrong_number_of_args_exception::~wrong_number_of_args_exception() throw() {}

    const char* wrong_number_of_args_exception::what() const throw() {
        return whatstr.c_str();
    }

      wrong_types_of_args_exception::wrong_types_of_args_exception( int w, const std::string& expected, const std::string& received )
          : whicharg( w ), expected_(expected), received_(received)
      {
        std::stringstream a;
        a << "In argument "<<w<<": wrong type of argument: Expected type ";
        a << expected_;
        a << ", received type ";
        a << received_ << ".";
        whatstr = a.str();
      }

    wrong_types_of_args_exception::~wrong_types_of_args_exception() throw() {}


    const char* wrong_types_of_args_exception::what() const throw() {
        return whatstr.c_str();
    }

    non_lvalue_args_exception::non_lvalue_args_exception( int w, const std::string& received )
          : whicharg( w ), received_(received)
      {
        std::stringstream a;
        a << "In argument "<<w<<": can not assign value to non-lvalue ";
        a << received_ << ".";
        whatstr = a.str();
      }

    non_lvalue_args_exception::~non_lvalue_args_exception() throw() {}

    const char* non_lvalue_args_exception::what() const throw() {
        return whatstr.c_str();
    }


}
