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


#include "parser-types.hpp"

#include "CommonParser.hpp"
#include "ConditionParser.hpp"
#include "CommandParser.hpp"
#include "corelib/Event.hpp"
#include "ValueChangeParser.hpp"
#include "ValueParser.hpp"

#include <map>
#include <string>
#include <iosfwd>
#include <boost/function.hpp>

namespace ORO_Execution
{
    using ORO_CoreLib::Event;

    class StateMachineBuilder;
    class ParsedStateMachine;
    class StateDescription;

    namespace detail {
        class EventHandle;
    }

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
      our_pos_iter_t& mpositer;
      // offset relative to StateMachine text.
      int ln_offset;

      typedef std::map<std::string, ParsedStateMachine*> contextnamemap_t;
      typedef std::map<std::string, TaskAttributeBase*> contextparams_t;
      typedef std::map<std::string, DataSourceBase::shared_ptr> contextparamvalues_t;
      typedef std::map<std::string, StateDescription*> contextstatesmap_t;
      typedef std::map<std::string, StateMachineBuilder*> contextbuilders_t;
      typedef std::map<std::string, detail::EventHandle*> handlemap;

      contextnamemap_t rootcontexts;
      contextbuilders_t contextbuilders;
      ParsedStateMachine* curtemplatecontext;
      std::vector<CommandInterface*> varinitcommands;
      std::vector<CommandInterface*> paraminitcommands;
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
      handlemap curhandles;
      ProgramGraph* curprogram;
      detail::EventHandle* curhand;
      Event<void(void)>* curevent;
      boost::function<void(void)> cureventsink;
      ConditionInterface* curcondition;
      std::string curscvccontextname;
      std::string curscvcparamname;

      /**
       * used to sort conditions as they are generated and
       * inserted in the StateMachine.
       */
      int rank;

      rule_t production;
      rule_t newline;
      rule_t rootcontextinstantiation;
      rule_t statecontext;
      rule_t contextinstantiation;
      rule_t statecontextcontent;
      rule_t varline;
      rule_t state;
      rule_t vardec;
      rule_t eventhandledecl;
      rule_t subMachinedecl;
      rule_t statecontent;
      rule_t statecontentline;
      rule_t entry;
      rule_t preconditions;
      rule_t handle;
      rule_t transitions;
      rule_t exit;
      rule_t eeline;
      rule_t varchanges;
      rule_t eecommand;
      rule_t handleline;
      rule_t handlecommand;
      rule_t docommand;
      rule_t statecommand;
      rule_t transline;
      rule_t selectcommand;
      rule_t disconnectevent;
      rule_t connectevent;
      rule_t emitcommand;
      rule_t brancher;
      rule_t selector;
      rule_t eventbinding;
      rule_t contextinstarguments;
      rule_t contextinstargument;
      rule_t contextmemvar;
      rule_t contextvariable;
      rule_t contextparam;
      rule_t contextconstant;
      rule_t contextalias;
      rule_t subMachinevarchange;

      ConditionParser conditionparser;
      CommonParser commonparser;
      CommandParser commandparser;
      ValueChangeParser valuechangeparser;
      ExpressionParser expressionparser;
      ValueParser valueparser;

      void clear();

      void handledecl( iter_t s, iter_t f);

      void seeninitialstate();
      void seenfinalstate();
      void statedef( iter_t s, iter_t f);
      void seenstateend();

      void inprogram( const std::string& name );
      void seenentry();
      void seenexit();
      void seenhandle();
      ProgramGraph* finishProgram();

      void seencommand();
      void seenstatement();
      void seencondition();
      void seenselect( iter_t s, iter_t f);

      void seenemit();

      void seenconnecthandler( iter_t s, iter_t f);
      void seenconnect();
      void seendisconnecthandler( iter_t s, iter_t f );

      void eventselected();
      void seensink();
      void finished();
      void seenvaluechange();

      void seenstatecontextname( iter_t begin, iter_t end );
      void storeOffset();
      void saveText( iter_t begin, iter_t end );
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
      void seenscvcexpression();

      ProgramGraph* emptyProgram(const std::string& name);
  public:
    StateGraphParser( iter_t& positer, TaskContext* tc );
    ~StateGraphParser();

    // tries to parse, returns the instantiated root contexts.  On all
    // returned contexts setName() will have been called with the
    // correct name.
    // will throw an file_parse_exception on error
    std::vector<ParsedStateMachine*> parse( iter_t& begin, iter_t end );
  };
}
