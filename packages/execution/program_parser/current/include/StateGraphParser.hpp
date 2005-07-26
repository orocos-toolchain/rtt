/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  StateGraphParser.hpp

                        StateGraphParser.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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

#ifndef EXECUTION_STATE_PARSER_HPP
#define EXECUTION_STATE_PARSER_HPP

#include "parser-types.hpp"

#include <map>
#include <string>
#include <boost/function.hpp>
#include "DataSource.hpp"

namespace ORO_Execution
{
    class StateDescription;
    class TaskContext;
    class TaskAttributeBase;
    class FunctionGraph;
    class ParsedStateMachine;
    class ProgramInterface;
}
namespace ORO_Execution { namespace detail
{
    class StateMachineBuilder;
    class ProgramGraphParser;
    class ConditionParser;
    class ValueChangeParser;
    class ExpressionParser;
    class CommonParser;
    class PeerParser;
    class ArgumentsParser;

  /**
   * This is not a parser in the Boost.spirit sense of the word, it's
   * just a class used to hold the parser and semantic actions..
   *
   * This class does the actual work.  It generates a Finite State Graph, by
   * constructing the State nodes.  We just go over the code one
   * single time, and construct the FSM as we go.  For every
   * statement, we construct a new GraphNode, and fill it up as we
   * get the information we need.
   */
  class StateGraphParser
  {
      // Our task we are loaded in :
      TaskContext* context;
      // The TC of the current StateMachine
      TaskContext* curcontext ;
      TaskContext* peer;
      our_pos_iter_t& mpositer;
      our_pos_iter_t saveStartPos;
      // offset relative to StateMachine text.
      int ln_offset;

      typedef std::map<std::string, ParsedStateMachine*> contextnamemap_t;
      typedef std::map<std::string, TaskAttributeBase*> contextparams_t;
      typedef std::map<std::string, DataSourceBase::shared_ptr> contextparamvalues_t;
      typedef std::map<std::string, StateDescription*> contextstatesmap_t;
      typedef std::map<std::string, StateMachineBuilder*> contextbuilders_t;

      contextnamemap_t rootcontexts;
      contextbuilders_t contextbuilders;
      ParsedStateMachine* curtemplatecontext;
      std::vector<CommandInterface*> paraminitcommands;
      std::vector<CommandInterface*> varinitcommands;
      ParsedStateMachine* curinstantiatedcontext;
      StateMachineBuilder* curcontextbuilder;
      std::string curinstcontextname;
      contextparamvalues_t curinstcontextparams;
      std::string curcontextinstargumentname;
      std::string curcontextname;
      bool curinitialstateflag;
      bool curfinalstateflag;
      StateDescription* curstate;
      StateDescription* curnonprecstate;
      ProgramGraphParser* progParser;
      ProgramInterface* transProgram;
      ConditionInterface* curcondition;
      std::string curscvccontextname;
      std::string curscvcparamname;

      /**
       * used to sort conditions as they are generated and
       * inserted in the StateMachine.
       */
      int rank;
      bool isroot; //! are we instantiating a rootmachine ?
      unsigned int selectln; //! store line number of select's 'if' statement.

      std::vector<DataSourceBase::shared_ptr> evargs;
      std::string evname;

      rule_t production;
      rule_t newline;
      rule_t rootcontextinstantiation;
      rule_t statecontext;
      rule_t contextinstantiation;
      rule_t statecontextcontent;
      rule_t varline;
      rule_t state;
      rule_t vardec;
      rule_t subMachinedecl;
      rule_t statecontent;
      rule_t statecontentline;
      rule_t entry;
      rule_t preconditions;
      rule_t precondition;
      rule_t run;
      rule_t transitions;
      rule_t transition;
      rule_t transprog;
      rule_t argslist;
      rule_t handle;
      rule_t exit;
      rule_t programBody;
      rule_t transline;
      rule_t selectcommand;
      rule_t brancher;
      rule_t selector;
      rule_t contextinstarguments;
      rule_t contextinstargument;
      rule_t contextmemvar;
      rule_t contextvariable;
      rule_t contextparam;
      rule_t contextconstant;
      rule_t contextalias;
      rule_t subMachinevarchange;

      ConditionParser* conditionparser;
      CommonParser* commonparser;
      ValueChangeParser* valuechangeparser;
      ExpressionParser* expressionparser;
      ArgumentsParser* argsparser;
      PeerParser* peerparser;

      void clear();

      void seeninitialstate();
      void seenfinalstate();
      void statedef( iter_t s, iter_t f);
      void seenstateend();

      void inprogram( const std::string& name );
      void seenentry();
      void seenexit();
      void seenhandle();
      void seenrun();
      FunctionGraph* finishProgram();

      void seencondition();
      void seenendcondition();
      void seentransprog();
      void seenselect( iter_t s, iter_t f);
      void seenprecondition();
      void seeneventname(iter_t s, iter_t f);
      void seeneventtrans();
      void seeneventargs();

      void seenstatecontextname( iter_t begin, iter_t end );
      void storeOffset();
      void saveText( iter_t begin, iter_t end );
      void startrootcontextinstantiation();
      void seenrootcontextinstantiation();
      void seenstatecontextend();
      void seensubMachineinstantiation();

      void inpreconditions();
      void seenpreconditions();

      void seencontexttypename( iter_t begin, iter_t end );
      void seeninstcontextname( iter_t begin, iter_t end );
      void seencontextinstantiation();
      void seencontextinstargumentname( iter_t begin,  iter_t end );
      void seencontextinstargumentvalue();

      void seencontextvariable();
      void seencontextparam();

      void seenscvcsubMachinename( iter_t begin, iter_t end );
      void seenscvcparamname( iter_t begin, iter_t end );

  public:
    StateGraphParser( iter_t& positer, TaskContext* tc );
    ~StateGraphParser();

    // tries to parse, returns the instantiated root contexts.  On all
    // returned contexts setName() will have been called with the
    // correct name.
    // will throw an file_parse_exception on error
    std::vector<ParsedStateMachine*> parse( iter_t& begin, iter_t end );
  };
}}

#endif
