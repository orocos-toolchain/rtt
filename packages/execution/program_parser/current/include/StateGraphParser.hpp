 
#include "parser-common.hpp"

#include "ConditionParser.hpp"
#include "CommandParser.hpp"
#include "ParseContext.hpp"
#include "corelib/EventInterfaces.hpp"
#include "StateGraph.hpp"

#include <map>
#include <string>

namespace ORO_Execution
{

    namespace detail {
        class EventHandle;
    }
    using ORO_CoreLib::HandlerRegistrationInterface;
    using ORO_CoreLib::EventListenerInterface;

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
      typedef StateGraph::CommandNode CommandNode;
      typedef StateGraph::ConditionEdge ConditionEdge;
      typedef StateGraph::Graph  Graph;

      ParseContext context;

      our_pos_iter_t& mpositer;
      std::string minit;

      // A map of all States
      typedef std::map<std::string, StateDescription*> statemap;
      statemap mstates;

      // A map of all Event Handles
      typedef std::map<std::string, detail::EventHandle*> handlemap;
      handlemap mhandles;

      rule_t newline, 
          line, content, program, production,
          state, entry, exit, handle, vardec,transitions,
          eecommand, selectcommand, docommand,
          brancher, selector, connectevent,
          disconnectevent, eventbinding;

      rule_t varline, eeline, transline, handleline;

      ConditionParser conditionparser;
      CommonParser commonparser;
      CommandParser commandparser;
      ValueParser valueparser;

      StateGraph* state_graph;
      detail::EventHandle* mhand;

      HandlerRegistrationInterface* mevent;
      EventListenerInterface*     meventsink;

      ConditionInterface* mcondition;
      StateDescription*   mstate;

      void handledecl( iter_t s, iter_t f);

      void initstate( iter_t s, iter_t f);
      void statedef( iter_t s, iter_t f);
      void seenstateend();

      void inentry();
      void inexit();
      void inhandle();

      void seencommand();
      void seenstatement();
      void seencondition();
      void selecting( iter_t s, iter_t f);


      void selecthandler( iter_t s, iter_t f);
      void seenconnect();
      void disconnecthandler( iter_t s, iter_t f );

      void eventselected();
      void seensink();
      void finished();
  public:
    StateGraphParser( iter_t& positer, Processor* proc,
                   const GlobalFactory* ext );

    // tries to parse, returns the generated program on success, 0 on
    // failure..  should not throw..
    StateGraph* parse( iter_t& begin, iter_t end );
  };
}
