#include "execution/ProgramParser.hpp"

#include "execution/ProgramNode.hpp"
#include "execution/Processor.hpp"
#include "execution/TerminationClause.hpp"
#include "corelib/CommandNOP.hpp"
#include "execution/Program.hpp"
#include "corelib/ConditionTrue.hpp"
#include "execution/DataSourceCondition.hpp"
#include "execution/ParsedValue.hpp"

#include <iostream>
#include <boost/bind.hpp>

namespace ORO_Execution
{
  using boost::bind;
  using ORO_CoreLib::CommandNOP;
  using ORO_CoreLib::ConditionTrue;

  ProgramParser::ProgramParser( iter_t& positer,
                                Processor* proc,
                                const GlobalFactory* e )
    : context( proc, e ), mpositer( positer ), mcurrentnode( 0 ),
      mcurrenttargetnode( 0 ),
      mnextnode( new ProgramNode ), mstartnode( mnextnode ),
      mleafnode( ProgramNode::leafNode( context.processor ) ),
      conditionparser( context ),
      commandparser( context ),
      valuechangeparser( context )
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
    
    newline = ch_p( '\n' );

    // program is the production rule of this grammar.  The
    // production rule is the rule that the entire input should be
    // matched by...  This line basically means that we're finished
    // ;)
    production = program;

    // a program looks like "program { content }".
    program = (
      *newline
      >> str_p( "program" )
      >> *newline
      >> ch_p( '{' )
      >> content
      >> ch_p( '}' )
      >> *newline ) [ bind( &ProgramParser::seenprogramend, this ) ];

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
        )[ bind( &ProgramParser::seenvaluechange, this ) ];
    // the only type of statement currently ( ever ? ) is the call
    // statement, see above..
    statement = !labelpart >> callstatement;

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
           bind( &ProgramParser::seenlabel, this, _1, _2 ) ]
      >> ':';

    // a call statement: "do xxx until { terminationclauses }"
    callstatement = (
      str_p( "do" ) [ bind( &ProgramParser::startofnewstatement, this ) ]
      >> commandparser.parser()[
        bind( &ProgramParser::seencommandcall, this ) ]
      >> !terminationpart
      ) [ bind( &ProgramParser::seencallstatement, this ) ];

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

    // a termination clause: "if xxx then goto yyy" where xxx is
    // a condition, and yyy is a label.
    terminationclause =
      (    str_p( "if" )
           >> conditionparser.parser()
           >> "then" >> "goto"
           >> jumpdestination[ bind( &ProgramParser::seentargetlabel,
                                     this, _1, _2 ) ]
        )[ bind( &ProgramParser::seentermclause, this ) ];

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
  };

  void ProgramParser::seencallstatement()
  {
    // startofnewcommand should have been run already..
    assert( mcurrentnode.get() );

    // a ProgramNode should have at least one termination
    // condition.  If it doesn't, then we add a default one,
    // which just moves on to the next node..
    if ( mcurrentnode->getNbClauses() == 0 )
    {
      // either use the implicit termination condition, or use
      // true..
      ConditionInterface* cond =
        implcond.get() ? implcond.release() : new ConditionTrue;
      TerminationClause* clause =
        new TerminationClause( cond, mnextnode.get() );
      mcurrentnode->addTerminationClause( clause );
    };

    assert( mcurrentnode->getNbClauses() > 0 );

    if ( !mcurlabel.empty() )
      mlabels[mcurlabel] = mcurrentnode;

    mcurlabel.clear();
    // the done condition is no longer valid..
    context.valueparser.removeValue( "done" );
  };

  void ProgramParser::seentermclause()
  {
    assert( mcurrentnode );
    assert( mcurrenttargetnode );

    ConditionInterface* condition = conditionparser.getParseResult();
    assert( condition );
    conditionparser.reset();

    TerminationClause* clause =
      new TerminationClause( condition, mcurrenttargetnode.get() );
    mcurrentnode->addTerminationClause( clause );

    mcurrenttargetnode = 0;
  };

  void ProgramParser::startofnewstatement()
  {
    mcurrentnode = mnextnode;
    mnextnode = new ProgramNode;

    int ln = mpositer.get_position().line;
    mcurrentnode->setLineNumber( ln );
  };

  void ProgramParser::seenprogramend()
  {
    // we need to fill up the last mnextnode.  We fill it up by a
    // CommandNOP node, that has a ConditionTrue Termination
    // clause that makes control flow to the leaf node..
    mnextnode->setCommand( new CommandNOP );
    mnextnode->addTerminationClause(
      new TerminationClause( new ConditionTrue, mleafnode.get() ) );
  };

  Program* ProgramParser::parse( iter_t& begin, iter_t end )
  {
    skip_parser_t skip_parser = SKIP_PARSER;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );
    try {
      if ( ! production.parse( scanner ) )
      {
        std::cerr << "Syntax error at line "
                  << mpositer.get_position().line
                  << std::endl;
        return 0;
      }
      return new Program( mstartnode.get() );
    }
    catch( const parse_exception& e )
    {
      std::cerr << "Parse error at line "
                << mpositer.get_position().line
                << ": " << e.what() << std::endl;
      return 0;
    }
  };

  void ProgramParser::seencommandcall()
  {
    // startofnewstatement should already have been called..
    assert( mcurrentnode.get() );
    // we get the data from commandparser, and put it into
    // mcurrentnode..
    mcurrentnode->setCommand( commandparser.getCommand() );
    assert( mcurrentnode->getCommand() );
    implcond.reset( commandparser.getImplTermCondition() );
    if ( !implcond.get() )
      implcond.reset( new ConditionTrue );
    context.valueparser.setValue(
      "done", new ParsedAliasValue<bool>(
        new DataSourceCondition( implcond->clone() ) ) );
    commandparser.reset();
  };

  void ProgramParser::seenvaluechange()
  {
    // some value changes generate a command, we need to add it to
    // the program.
    CommandInterface* ac = valuechangeparser.assignCommand();
    // and not forget to reset()..
    valuechangeparser.reset();
    if ( ac )
    {
      // First, mcurrentnode etc. need to be properly set up..  we
      // just call startofnewstatement to do it for us..
      startofnewstatement();
      assert( mcurrentnode.get() );
      mcurrentnode->setCommand( ac );
      TerminationClause* clause =
        new TerminationClause( new ConditionTrue, mnextnode.get() );
      mcurrentnode->addTerminationClause( clause );

      // a value change can have a label too..
      if ( !mcurlabel.empty() )
        mlabels[mcurlabel] = mcurrentnode;
      mcurlabel.clear();
    };
  };
}
