/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  StateGraphParser.hpp

                        StateGraphParser.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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

#ifndef EXECUTION_STATE_PARSER_HPP
#define EXECUTION_STATE_PARSER_HPP

#include "parser-types.hpp"

#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "../internal/DataSource.hpp"
namespace RTT { namespace scripting
{

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
      // Our task we are parsing in :
      TaskContext* context;
      // The task that will execute it:
      TaskContext* caller;
      // The Service of the current StateMachine
      boost::shared_ptr<StateMachineService> curobject ;
      ServicePtr peer;
      our_pos_iter_t& mpositer;
      our_pos_iter_t saveStartPos;
      // offset relative to StateMachine text.
      int ln_offset;

      typedef boost::shared_ptr<ParsedStateMachine> ParsedStateMachinePtr;
      typedef std::map<std::string, ParsedStateMachinePtr> machinenamemap_t;
      typedef std::map<std::string, base::AttributeBase*> machineparams_t;
      typedef std::map<std::string, base::DataSourceBase::shared_ptr> machineparamvalues_t;
      typedef std::map<std::string, StateDescription*> machinestatesmap_t;
      typedef std::map<std::string, StateMachineBuilder*> machinebuilders_t;

      machinenamemap_t rootmachines;
      machinebuilders_t machinebuilders;
      ParsedStateMachinePtr curtemplate;
      std::vector<base::ActionInterface*> paraminitcommands;
      std::vector<base::ActionInterface*> varinitcommands;
      ParsedStateMachinePtr curinstantiatedmachine;
      StateMachineBuilder* curmachinebuilder;
      std::string curinstmachinename;
      machineparamvalues_t curinstmachineparams;
      std::string curmachineinstargumentname;
      std::string curmachinename;
      bool curinitialstateflag;
      bool curfinalstateflag;
      StateDescription* curstate;
      StateDescription* curnonprecstate;
      ProgramGraphParser* progParser;
      boost::shared_ptr<ProgramInterface> transProgram;
      StateDescription* elsestate;
      boost::shared_ptr<ProgramInterface> elseProgram;
      ConditionInterface* curcondition;
#if 0
      std::string curscvcmachinename;
      std::string curscvcparamname;
#endif
      /**
       * used to sort conditions as they are generated and
       * inserted in the StateMachine.
       */
      int rank;
      bool isroot; //! are we instantiating a rootmachine ?
      unsigned int selectln; //! store line number of select's 'if' statement.

      std::vector<base::DataSourceBase::shared_ptr> evargs;
      std::string evname;
      std::map<std::string,ConditionCache*> cur_port_events;
      std::map<std::string,ConditionCache*> global_port_events;

      rule_t production;
      rule_t newline;
      rule_t rootmachineinstantiation;
      rule_t statemachine;
      rule_t machineinstantiation;
      rule_t statemachinecontent;
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
      rule_t argslist;
      rule_t handle;
      rule_t exit;
      rule_t programBody;
      rule_t transline;
      rule_t eventline;
      rule_t program;
      rule_t progselect;
      rule_t ifbranch;
      rule_t elsebranch;
      rule_t selector;
      rule_t machineinstarguments;
      rule_t machineinstargument;
      rule_t machinememvar;
      rule_t machinevariable;
      rule_t machineparam;
      rule_t machineconstant;
      rule_t machinealias;
      rule_t subMachinevarchange;

      CommonParser* commonparser;
      ConditionParser* conditionparser;
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
      boost::shared_ptr<ProgramInterface> finishProgram();

      void seencondition();
      void seenendcondition();
      void seentransprog();
      void seenelseprog();
      void seenelse();
      void noselect();
      void seenselect( iter_t s, iter_t f);
      void doselect(const std::string& name);
      void seenprecondition();
      void seeneventname(iter_t s, iter_t f);
      void seeneventtrans();
      void seeneventargs();

      void seenstatemachinename( iter_t begin, iter_t end );
      void storeOffset();
      void saveText( iter_t begin, iter_t end );
      void startrootmachineinstantiation();
      void seenrootmachineinstantiation();
      void seenstatemachineend();
      void seensubMachineinstantiation();

      void inpreconditions();
      void seenpreconditions();

      void seenmachinetypename( iter_t begin, iter_t end );
      void seeninstmachinename( iter_t begin, iter_t end );
      void seenmachineinstantiation();
      void seenmachineinstargumentname( iter_t begin,  iter_t end );
      void seenmachineinstargumentvalue();

      void seenmachinevariable();
      void seenmachineparam();

#if 0
      void seenscvcsubMachinename( iter_t begin, iter_t end );
      void seenscvcparamname( iter_t begin, iter_t end );
#endif
  public:
    StateGraphParser( iter_t& positer, TaskContext* tc, TaskContext* caller );
    ~StateGraphParser();

    // tries to parse, returns the instantiated root machines.  On all
    // returned machines setName() will have been called with the
    // correct name.
    // will throw an file_parse_exception on error
    std::vector<ParsedStateMachinePtr> parse( iter_t& begin, iter_t end );
  };
}}

#endif
