#ifndef PARSE_EXCEPTION_HPP
#define PARSE_EXCEPTION_HPP

#include <string>

#pragma interface

namespace ORO_Execution
{
  /**
   * @brief This is the exception class that is thrown by all of the parsers
   * to indicate a parse error.  It holds a string describing what
   * went wrong.
   */
  class parse_exception
  {
    const std::string mwhat;
  public:
    parse_exception( const std::string& what )
      : mwhat( what ) {}
    const char* what() const throw () { return mwhat.c_str(); }
  };

}

#endif
