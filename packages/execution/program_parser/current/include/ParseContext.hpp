#ifndef PARSECONTEXT_HPP
#define PARSECONTEXT_HPP

#include "parser-common.hpp"
#include "ValueParser.hpp"

namespace ORO_Execution
{

  struct ParseContext
  {
    ParseContext( Processor* proc,
                  const GlobalFactory* se )
      : processor( proc ), scriptableextension( se )
      {
      };
    ValueParser valueparser;
    Processor* processor;
    const GlobalFactory* scriptableextension;
  };
}

#endif
