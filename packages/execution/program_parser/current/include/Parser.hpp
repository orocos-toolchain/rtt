#ifndef PROGRAMPARSER_HPP
#define PROGRAMPARSER_HPP

#include <iosfwd>

namespace ORO_Execution
{
    class ProgramGraph;
    class Processor;
    class GlobalFactory;
}

namespace ORO_CoreLib
{
  class PropertyBase;
  class CommandInterface;
  class ConditionInterface;
}

namespace ORO_Execution
{
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::CommandInterface;
  using ORO_CoreLib::ConditionInterface;
  /**
   * This class is the public interface
   */
  class Parser
  {
  public:
    /**
     * Reads out the stream, parses it, and returns a new @ref ProgramGraph..
     */
    ProgramGraph* parseProgram( std::istream& s, Processor*,
                           const GlobalFactory* );

    /**
     * parses the string as a condition, and returns a new
     * ConditionInterface or 0 on parse failure..  Use ext to get the
     * data from the components..
     */
    ORO_CoreLib::ConditionInterface* parseCondition(
      std::string& s, const GlobalFactory* ext );
  };
};
#endif
