/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Parser.hpp 

                        Parser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef PROGRAMPARSER_HPP
#define PROGRAMPARSER_HPP

#include <iosfwd>

namespace ORO_CoreLib
{
    class StateContext;
}

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
  using ORO_CoreLib::StateContext;
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
     * Reads out the stream, parses it, and returns a new @ref ProgramGraph ..
     */
      ProgramGraph* parseProgram( std::istream& s, Processor*,
                                  const GlobalFactory* );

    /**
     * Reads out the stream, parses it, and returns a new @ref StateContext ..
     */
      StateContext* parseStateContext( std::istream& s, Processor*,
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
