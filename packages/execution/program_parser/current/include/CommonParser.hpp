#ifndef COMMONPARSER_HPP
#define COMMONPARSER_HPP

#include "parser-types.hpp"

namespace ORO_Execution
{

  /**
   * @brief This class contains some very common parser definitions.
   */
  class CommonParser
  {
  public:
      symbols<nil_t> keywords;
      rule_t identifier;
      lexeme_rule_t lexeme_identifier;

      CommonParser();
  };
}

#endif
