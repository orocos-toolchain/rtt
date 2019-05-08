/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ProgramGraphParser.hpp

                        ProgramGraphParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef EXECUTION_PROGRAM_PARSER_HPP
#define EXECUTION_PROGRAM_PARSER_HPP

#include "parser-types.hpp"

#include "CommonParser.hpp"
#include "PeerParser.hpp"
#include "ConditionParser.hpp"
#include "ExpressionParser.hpp"
#include "../TaskContext.hpp"
#include "FunctionGraphBuilder.hpp"
#include "ValueChangeParser.hpp"

#include <map>
#include <vector>
#include <string>


#include "ProgramService.hpp"
//namespace RTT {
//    class ProgramService;
//}

namespace RTT { namespace scripting
{
  /**
   * @brief A Parser for Orocos Program Scripts.
   *
   * This class does the actual work.  It generates a Program, by
   * constructing the ProgramNode's.  We just go over the code one
   * single time, and construct the Program as we go.  For every
   * statement, we construct a new ProgramNode, and fill it up as we
   * get the information we need.
   */
  class ProgramGraphParser
  {
      typedef FunctionGraphBuilder::CommandNode CommandNode;
      typedef FunctionGraphBuilder::ConditionEdge ConditionEdge;
      typedef FunctionGraphBuilder::Graph  Graph;

      /**
       * The context given to us by the user to root
       * the parsed programs into.
       */
      TaskContext* rootc;

      /**
       * The current subcontext of root where we are working in.
       */
      Service::shared_ptr context;

      /**
       * The current function context where we are working in.
       */
      Service::shared_ptr fcontext;

      /**
       * peer context for events.
       */
      TaskContext* peer;

      our_pos_iter_t& mpositer;

      // The current function we are parsing.
      FunctionGraphPtr mfunc;

      // The function we will call next
      FunctionGraphPtr mcallfunc;

      // A map of all functions
      typedef std::map<std::string, FunctionGraphPtr> funcmap;
      funcmap mfuncs;

      // The return type of the current function.
      std::string rettype;

      // The implicit termination condition we just got for the
      // current command from the CommandParser..
      ConditionInterface* implcond;
      std::vector<ConditionInterface*> implcond_v;
      std::vector<base::DataSourceBase::shared_ptr> callfnargs;

      // last seen condition
      ConditionInterface* mcondition;
      // try-block condition
      ConditionInterface* try_cond;

      void seencondition();

      void seencallfunclabel( iter_t begin, iter_t end );

      void seencontinue( );

      void skip_eol();
      void noskip_eol();
      void seenyield();
      void seenstatement();
      void seentrystatement();
      void startcatchpart();
      void seencatchpart();

      void seenvaluechange();

      void functiondef( iter_t begin, iter_t end );
      void exportdef(  );
      void globaldef(  );
      void seenfunctionarg();
      void seenfunctionend();
      void seenvalidinput();

      void seenfuncidentifier( iter_t begin, iter_t end);
      void seencallfuncstatement();
      void seencallfuncargs();

      void seenreturnstatement();
      void seenreturnlabel();
      void seenreturnvalue();
      void seenreturntype( iter_t begin, iter_t end );

      void seenifstatement();
      void endifblock();
      void endifstatement();

      void seenwhilestatement();
      void endwhilestatement();

      void seenbreakstatement();

      void seenforstatement();
      void seenforinit();
      void seenforinit_expr();
      void seenforincr();
      void seenemptyforincr();
      void endforstatement();

      void startofprogram();
      void programdef( iter_t begin, iter_t end );
      void seenprogramend();
      void programtext(iter_t, iter_t);

      void setStack(Service::shared_ptr st);
      //! Clears helper variables of parser.
      void clearParseState();
      void setup();
      void setup2();
      void cleanup(bool remove_service);

      rule_t newline, terminationclause, jumpdestination, terminationpart, andpart,
          dostatement, trystatement, statement, line, content, program,
          production, valuechange, returnstatement, function, functions, arguments, funcstatement,
          continuepart, returnpart, callpart, ifstatement, ifblock, whilestatement, breakstatement,
          openbrace, closebrace, opencurly, closecurly, forstatement, semicolon,
          condition, catchpart, funcargs, functionarg, emitstatement ;

      CommonParser& commonparser;
      ConditionParser conditionparser;
      ValueChangeParser valuechangeparser;
      ExpressionParser expressionparser;
      ArgumentsParser* argsparser;
      PeerParser peerparser;

      boost::shared_ptr<FunctionGraphBuilder> program_builder;
      std::vector< FunctionGraphPtr > program_list;

      base::ActionInterface* for_init_command;
      std::stack<base::ActionInterface*> for_incr_command;
      std::string program_text;
      bool exportf,globalf, parserused;
      int ln_offset;
  public:
      ProgramGraphParser( iter_t& positer, TaskContext* context, ExecutionEngine* caller, CommonParser& cp);
      ~ProgramGraphParser();

      /**
       * @brief Tries to parse programs, returns the generated programs on success.
       * @throw file_parse_exception The parser found an error.
       */
      std::vector<ProgramInterfacePtr> parse( iter_t& begin, iter_t end );

      std::vector<ProgramInterfacePtr> parseFunction( iter_t& begin, iter_t end );

      /**
       * Initialize the bodyParser to parse and store each statement it sees.
       * The result is retrieved with bodyParserResult().
       */
      void initBodyParser(const std::string& name, Service::shared_ptr stck, int offset);

      /**
       * Parses a multi-line program, which you can
       * retrieve with bodyParserResult().
       * You need to initialize it with initBodyParser
       */
      rule_t& bodyParser();

      /**
       * Parses a single-line statement.
       */
      rule_t& statementParser();

      /**
       * Returns a program foo {} parser.
       */
      rule_t& programParser();

      /**
       * Returns the last program parsed by programParser()
       */
      ProgramInterfacePtr programParserResult();
      /**
       * Parses a function foo {} definition.
       */
      rule_t& functionParser();

      ProgramInterfacePtr bodyParserResult();

      /**
       * Returns true if the parser was already used (even partially).
       * an initBodyParser will reset this to false.
       */
      bool parserUsed() const;
  };
}}

#endif
