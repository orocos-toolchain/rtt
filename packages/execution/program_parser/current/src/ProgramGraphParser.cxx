#include "execution/ProgramGraphParser.hpp"

#include "execution/Processor.hpp"
#include "corelib/CommandNOP.hpp"
#include "execution/ProgramGraph.hpp"
#include "corelib/ConditionTrue.hpp"
#include "execution/DataSourceCondition.hpp"
#include "execution/ParsedValue.hpp"

#include <iostream>
#include <boost/bind.hpp>

namespace ORO_Execution
{
  using namespace boost;
  using ORO_CoreLib::CommandNOP;
  using ORO_CoreLib::ConditionTrue;

  ProgramGraphParser::ProgramGraphParser( iter_t& positer,
                                Processor* proc,
                                const GlobalFactory* e )
      : context( proc, e ), mpositer( positer ),
        conditionparser( context ),
        commandparser( context ),
        valuechangeparser( context ),
        program_graph(0)
  {
    BOOST_SPIRIT_DEBUG_RULE( newline );
    BOOST_SPIRIT_DEBUG_RULE( terminationclause );
    BOOST_SPIRIT_DEBUG_RULE( jumpdestination );
    BOOST_SPIRIT_DEBUG_RULE( terminationpart );
    BOOST_SPIRIT_DEBUG_RULE( callstatement );
    BOOST_SPIRIT_DEBUG_RULE( labelpart );
    BOOST_SPIRIT_DEBUG_RULE( statement );
    BOOST_SPIRIT_DEBUG_RULE( line );
    BOOST_SPIRIT_DEBUG_RULE( content );
    BOOST_SPIRIT_DEBUG_RULE( program );
    BOOST_SPIRIT_DEBUG_RULE( production );
    BOOST_SPIRIT_DEBUG_RULE( valuechange );
    BOOST_SPIRIT_DEBUG_RULE( function );
    BOOST_SPIRIT_DEBUG_RULE( returnstatement );
    BOOST_SPIRIT_DEBUG_RULE( funcstatement );
    BOOST_SPIRIT_DEBUG_RULE( gotopart );
    BOOST_SPIRIT_DEBUG_RULE( callpart );
    BOOST_SPIRIT_DEBUG_RULE( returnpart );
    
    newline = ch_p( '\n' );

    // program is the production rule of this grammar.  The
    // production rule is the rule that the entire input should be
    // matched by...  This line basically means that we're finished
    // ;)
    // Zero or n functions can precede the program.
    production = *function >> program ;

    // a function is very similar to a program, but it also has a name
    function = (
       *newline
       >> str_p( "function" ) 
       >> commonparser.identifier[ bind( &ProgramGraphParser::functiondef, this, _1, _2 ) ]
       >> *newline
       >> ch_p( '{' )
       >> content
       >> ch_p( '}' )
       >> *newline ) [ bind( &ProgramGraphParser::seenfunctionend, this ) ];

    // a program looks like "program { content }".
    program = (
      *newline
      >> str_p( "program" )[ bind( &ProgramGraphParser::startofprogram, this)]
      >> *newline
      >> ch_p( '{' )
      >> content
      >> ch_p( '}' )
      >> *newline ) [ bind( &ProgramGraphParser::seenprogramend, this ) ];

    // the content of a program can be any number of lines
    content = *line;

    // a line can be empty or contain a statement. Empty is
    // necessary, because comment's are skipped, but newline's
    // aren't.  So a line like "/* very interesting comment
    // */\n" will reach us as simply "\n"..
    line = !( statement | valuechange ) >> newline;

    valuechange = !labelpart >> (
        valuechangeparser.constantDefinitionParser()
      | valuechangeparser.variableDefinitionParser()
      | valuechangeparser.aliasDefinitionParser()
      | valuechangeparser.variableAssignmentParser()
        )[ bind( &ProgramGraphParser::seenvaluechange, this ) ];
    // the only type of statement currently ( ever ? ) is the call
    // statement, see above..
    statement = !labelpart >> callstatement | funcstatement | returnstatement; 

    // the label part of a statement.  Statements can be given
    // labels.  The label then identifies the spot in the
    // program.  It can then be jumped to from a termination
    // clause.
    //
    // It looks like:
    //   "label: <statement>"
    // and should be put in front of a statement..
    labelpart =
         commonparser.identifier[
           bind( &ProgramGraphParser::seenlabel, this, _1, _2 ) ]
      >> ':';

    // a call statement: "do xxx until { terminationclauses }"
    callstatement = (
      str_p( "do" ) [ bind( &ProgramGraphParser::startofnewstatement, this ) ]
      >> commandparser.parser()[
        bind( &ProgramGraphParser::seencommandcall, this ) ]
      >> !terminationpart
      ) [ bind( &ProgramGraphParser::seencallstatement, this ) ];

    // a function statement : "call functionname"
    funcstatement = (
      str_p( "call" )
      >> commonparser.identifier[bind( &ProgramGraphParser::seenfuncidentifier, this, _1, _2) ]
      )[ bind( &ProgramGraphParser::seencallfuncstatement, this ) ];

    // a return statement : "return"
    returnstatement =
        str_p( "return" )[ bind( &ProgramGraphParser::seenreturnstatement, this ) ];

    // the termination clause part of a (call) statement.  A
    // call statement looks like "do xxx until {
    // terminationclauses }".  The termination clause part is
    // everything starting at "until"..
    terminationpart =
      str_p( "until" )
        >> *newline >> ch_p( '{' ) >> *newline
        >> terminationclause
        >> *(newline >> !terminationclause)
        >> ch_p( '}' );

    // a termination clause: "if xxx then goto|call yyy" where xxx is
    // a condition, and yyy is a label.
    terminationclause =
        str_p( "if" )
            >> conditionparser.parser()[ bind(&ProgramGraphParser::seencondition, this) ]
            >> "then" 
            >> (gotopart | callpart | returnpart);

    gotopart =  str_p("goto")
        >> jumpdestination[ bind( &ProgramGraphParser::seentargetlabel,
                                  this, _1, _2 ) ] ;
    callpart = str_p("call")
        >> commonparser.identifier[ bind( &ProgramGraphParser::seencallfunclabel,
                                          this, _1, _2 ) ] ;
    returnpart = str_p("return")[ bind( &ProgramGraphParser::seenreturnlabel, this)];

    // a label, a string identifying a place for a termination
    // clause to jump to..  There are two special cases: "next"
    // means go to the next statement, And "stop" means go to
    // the end of the program.  All other labels "xxx" should
    // have been defined by putting "xxx:" before a statement.
    // It is currently only possible to use labels defined
    // before the current line, or on this line.  You can't jump
    // forward, only backward.  Maybe this is too big a
    // limitation, and there should be a way to jump forward,
    // but I don't think so..
    jumpdestination =
      str_p( "next" ) | str_p( "stop" ) | commonparser.identifier;
  }

  void ProgramGraphParser::seencallstatement()
  {
      // Called after a whole command statement is parsed.
      // a CommandNode should have at least one edge
      // If it doesn't, then we add a default one,
      // which just moves on to the next node..
      if ( program_graph->currentEdges() == 0 )
          {
              // either use the implicit termination condition, or use
              // true..
              ConditionInterface* cond = implcond ? implcond : new ConditionTrue;
              program_graph->proceedToNext( cond, mpositer.get_position().line );
          }
      else
          program_graph->proceedToNext( mpositer.get_position().line );

    // the done condition is no longer valid..
    context.valueparser.removeValue( "done" );
  }

    void ProgramGraphParser::startofprogram()
    {
        program_graph->startProgram(context.processor);
    }

  void ProgramGraphParser::functiondef( iter_t begin, iter_t end )
  {
      // store the function in our map for later
      // referencing.
      std::string funcdef(begin, end);
      if ( mfuncs.count(funcdef) != 0 )
          throw parse_exception("function " + funcdef + " redefined.");

      mfunc = mfuncs[funcdef] = program_graph->startFunction();
  }

  void ProgramGraphParser::seenfunctionend()
  {
      assert( mfunc );
      // Fake a 'return' statement at the last line.
      program_graph->returnFunction( new ConditionTrue, mfunc );
      program_graph->proceedToNext( mpositer.get_position().line );
      program_graph->endFunction( mfunc );
      // close the function.
      mfunc = 0;
  }

  void ProgramGraphParser::seencondition()
  {
      mcondition = conditionparser.getParseResult();
      assert( mcondition );
      conditionparser.reset();
  }
  void ProgramGraphParser::seenreturnstatement()
  {
      // return statement can happen in program and in a function
      if ( mfunc == 0 )
          {
              program_graph->returnProgram( new ConditionTrue );
              program_graph->proceedToNext( mpositer.get_position().line );
          }
      else
          {
              program_graph->returnFunction( new ConditionTrue, mfunc);
              program_graph->proceedToNext(  mpositer.get_position().line );
          }
      mcurlabel.clear();
  }

  void ProgramGraphParser::seenreturnlabel()
  {
      // return label can happen in program and in a function
      assert(mcondition);

      if ( mfunc == 0 )
          program_graph->returnProgram( mcondition );
      else
          program_graph->returnFunction( mcondition,  mfunc );

      mcondition = 0;
  }

  void ProgramGraphParser::seenfuncidentifier( iter_t begin, iter_t end )
  {
      // store the part after 'call'
      std::string fname(begin, end);
      if ( mfuncs.count(fname) == 0 )
          throw parse_exception("calling function " + fname + " but it is not defined.");

      mcallfunc = mfuncs[ fname ];
  }

  void ProgramGraphParser::seencallfuncstatement()
  {
      // This function is called if the 'call func' is outside
      // a termination clause.

      // add it to the main program line of execution.
      assert( mcallfunc );
      program_graph->setFunction( mcallfunc );

      // The exit node of the function is already connected
      // to program->nextNode().
      program_graph->proceedToNext(mpositer.get_position().line);
      mcurlabel.clear();
  }

  void ProgramGraphParser::startofnewstatement()
  {
      // Set the line number.
      //int ln = mpositer.get_position().line;
      //mcurrentnode->setLineNumber( ln );
  }

  void ProgramGraphParser::seenprogramend()
  {
      // Fake a 'return' statement at the last line.
      program_graph->returnProgram( new ConditionTrue );
      program_graph->proceedToNext( mpositer.get_position().line );
      program_graph->endProgram();
  }

  ProgramGraph* ProgramGraphParser::parse( iter_t& begin, iter_t end )
  {
    skip_parser_t skip_parser = SKIP_PARSER;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );

    program_graph = new ProgramGraph();

    try {
      if ( ! production.parse( scanner ) )
      {
        std::cerr << "Syntax error at line "
                  << mpositer.get_position().line
                  << std::endl;
        delete program_graph;
        program_graph = 0;
        return 0;
      }
      return program_graph;
    }
    catch( const parse_exception& e )
    {
      std::cerr << "Parse error at line "
                << mpositer.get_position().line
                << ": " << e.what() << std::endl;
      delete program_graph;
      program_graph = 0;
      return 0;
    }
  }

  void ProgramGraphParser::seencommandcall()
  {
    // we get the data from commandparser
    program_graph->setCommand( commandparser.getCommand() );
    implcond =  commandparser.getImplTermCondition();

    if ( !implcond )
      implcond = new ConditionTrue;
    context.valueparser.setValue(
      "done", new ParsedAliasValue<bool>(
        new DataSourceCondition( implcond->clone() ) ) );
    commandparser.reset();
    mcurlabel.clear();
  }

  void ProgramGraphParser::seenvaluechange()
  {
    // some value changes generate a command, we need to add it to
    // the program.
    CommandInterface* ac = valuechangeparser.assignCommand();
    // and not forget to reset()..
    valuechangeparser.reset();
    if ( ac )
    {
      // a value change can have a label too..
      if ( !mcurlabel.empty() )
          {
              program_graph->setLabel( mcurlabel );
              mcurlabel.clear();
          }
      program_graph->setCommand( ac );
      // Since a valuechange does not add edges, we use this variant
      // to create one.
      program_graph->proceedToNext( new ConditionTrue, mpositer.get_position().line );
    }
  }

    void ProgramGraphParser::seencallfunclabel( iter_t begin, iter_t end )
    {
          // Used for "call xyz"
          // lookup mcallfunc
          seenfuncidentifier( begin, end );
          
          assert( mcondition );
          assert( mcallfunc );
          program_graph->appendFunction( mcondition, mcallfunc);
    }

    void ProgramGraphParser::seentargetlabel( iter_t begin, iter_t end )
    {
        // Used for "goto xyz"
        std::string str( begin, end );
        if ( !mcondition )
            throw parse_exception( "Condition Error on call to label \"" +str+"\".");

        CommandNode target;
        if ( str == "next" )
            target = program_graph->nextNode();
        else if ( str == mcurlabel )
            target = program_graph->currentNode();
        else
            if ( !program_graph->hasLabel( str )  )
                throw parse_exception( "Label \"" + str + "\" not defined." );
            else
                target = program_graph->findNode( str ).first;
                    
        // connect to target under condition.
        program_graph->addConditionEdge( mcondition, target );
      }

    void ProgramGraphParser::seenlabel( iter_t begin, iter_t end )
    {
        std::string label( begin, end );
        mcurlabel = label;
    }

}
