/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ProgramGraphParser.hpp 

                        ProgramGraphParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#include "parser-types.hpp"

#include "CommonParser.hpp"
#include "ConditionParser.hpp"
#include "CommandParser.hpp"
#include "ParseContext.hpp"
#include "ValueChangeParser.hpp"
#include "ProgramGraph.hpp"

#include <map>
#include <string>

namespace ORO_Execution
{
  /**
   * This is not a parser in the Boost.spirit sense of the word, it's
   * just a class used to hold the parser and semantic actions..
   *
   * This class does the actual work.  It generates a Program, by
   * constructing the ProgramNode's.  We just go over the code one
   * single time, and construct the Program as we go.  For every
   * statement, we construct a new ProgramNode, and fill it up as we
   * get the information we need.
   */
  class ProgramGraphParser
  {
      typedef ProgramGraph::CommandNode CommandNode;
      typedef ProgramGraph::ConditionEdge ConditionEdge;
      typedef ProgramGraph::Graph  Graph;

      ParseContext context;

      our_pos_iter_t& mpositer;

      // The current function we are parsing.
      FunctionGraph* mfunc;

      // The function we will call next
      FunctionGraph* mcallfunc;

      // A map of all functions
      typedef std::map<std::string, FunctionGraph*> funcmap;
      funcmap mfuncs;

      // the label that the user wants to give to the current
      // statement.  If this is empty, then no label is necessary..
      std::string mcurlabel;

      // The implicit termination condition we just got for the
      // current command from the CommandParser..
      ConditionInterface* implcond;

      // last seen condition
      ConditionInterface* mcondition;

      void seencondition();

      void seencallfunclabel( iter_t begin, iter_t end );

      void seencontinue( );

      void startofnewstatement();
      void seencommandcall();
      void seencallstatement();

      void seenvaluechange();

      void functiondef( iter_t begin, iter_t end );
      void seenfunctionend();

      void seenfuncidentifier( iter_t begin, iter_t end);
      void seencallfuncstatement();

      void seenreturnstatement();
      void seenreturnlabel();

      void seenifstatement();
      void endifblock();
      void endifstatement();

      void seenwhilestatement();
      void endwhilestatement();

      void startofprogram();
      void programdef( iter_t begin, iter_t end );
      void seenprogramend();

      rule_t newline, terminationclause, jumpdestination, terminationpart,
          callstatement, statement, line, content, program,
          production, valuechange, returnstatement, function, funcstatement,
          continuepart, returnpart, callpart, ifstatement, ifblock, whilestatement;

      ConditionParser conditionparser;
      CommonParser commonparser;
      CommandParser commandparser;
      ValueChangeParser valuechangeparser;

      ProgramGraph* program_graph;
  public:
    ProgramGraphParser( iter_t& positer, Processor* proc,
                   const GlobalFactory* ext );

    // tries to parse, returns the generated program on success, 0 on
    // failure..  should not throw..
    ProgramGraph* parse( iter_t& begin, iter_t end );
  };
}
