#include "parser-common.hpp"

#include "ConditionParser.hpp"
#include "CommandParser.hpp"
#include "ParseContext.hpp"
#include "ValueChangeParser.hpp"
#include "ProgramNode.hpp"

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
  class ProgramParser
  {
    ParseContext context;

    our_pos_iter_t& mpositer;

    // the program node we're currently building..
    ProgramNode::shared_ptr mcurrentnode;

    // it is possible that a ProgramNode is only referenced from this
    // map, and only used later on, so therefore we need to keep
    // shared_ptr's in this map
    typedef std::map<std::string, ProgramNode::shared_ptr> labelmap;
    labelmap mlabels;

    // the label that the user wants to give to the current
    // statement.  If this is empty, then no label is necessary..
    std::string mcurlabel;

    // the node that the termination clause we're currently building
    // wants to jump to..  This is set in seentargetlabel..  This
    // variable is never the owner of the object it points to...
    ProgramNode::shared_ptr mcurrenttargetnode;

    // the node that will next becom mcurrentnode.  We need this to
    // support "next" as a target label.  If this is 0, then a next
    // label might not be necessary..
    ProgramNode::shared_ptr mnextnode;

    // the ProgramNode that the program will start at..
    ProgramNode::shared_ptr mstartnode;

    // the program has only one leaf node, and this member points to
    // it...
    ProgramNode::shared_ptr mleafnode;

    // The implicit termination condition we just got for the
    // current command from the CommandParser..
    std::auto_ptr<ConditionInterface> implcond;

    void seentargetlabel( iter_t begin, iter_t end )
      {
        std::string str( begin, end );
        if ( str == "next" )
          mcurrenttargetnode = mnextnode;
        else if ( str == "stop" )
          mcurrenttargetnode = mleafnode;
        else if ( str == mcurlabel )
          mcurrenttargetnode = mcurrentnode;
        else
        {
          labelmap::iterator me = mlabels.find( str );
          if ( me == mlabels.end() )
            throw parse_exception( "Label \"" + str + "\" not defined." );
          else
            mcurrenttargetnode = me->second;
        };
      };

    void seentermclause();
    void seencallstatement();
    void seencommandcall();

    void seenlabel( iter_t begin, iter_t end )
      {
        std::cerr << "seenlabel" << std::endl;
        std::string label( begin, end );
        mcurlabel = label;
      }

    void startofnewstatement();
    void seenprogramend();
    void seenvaluechange();

    rule_t newline, terminationclause, jumpdestination, terminationpart,
      callstatement, labelpart, statement, line, content, program,
      production, valuechange;

    ConditionParser conditionparser;
    CommonParser commonparser;
    CommandParser commandparser;
    ValueChangeParser valuechangeparser;
  public:
    ProgramParser( iter_t& positer, Processor* proc,
                   const GlobalFactory* ext );

    // tries to parse, returns the generated program on success, 0 on
    // failure..  should not throw..
    Program* parse( iter_t& begin, iter_t end );
  };
}
