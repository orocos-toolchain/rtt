/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ProgramGraphParser.cxx 

                        ProgramGraphParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
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

    namespace {
        assertion<std::string> expect_opencurly("Open curly brace '{' expected.");
        assertion<std::string> expect_closecurly("Closing curly brace '}' expected ( or could not find out what this line means ).");
        assertion<std::string> expect_open("Open brace '(' expected.");
        assertion<std::string> expect_close("Closing brace ')' expected.");
        assertion<std::string> expect_then("Wrongly formatted \"if ... then\" clause.");
        assertion<std::string> expect_comma("Expected a comma separator.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_semicolon("Semicolon ';' expected after statement.");
        assertion<std::string> expect_ifblock("Expected a statement (block) after if .. then.");
        assertion<std::string> expect_elseblock("Expected a statement (block) after else.");
        assertion<std::string> expect_condition("Expected a boolean expression ( a condition ).");
        assertion<std::string> expect_expression("Expected an expression.");
        assertion<std::string> expect_command("Expected a command after 'do'.");
    }


  ProgramGraphParser::ProgramGraphParser( iter_t& positer,
                                Processor* proc,
                                const GlobalFactory* e )
      : context( proc, e ), mpositer( positer ),
        conditionparser( context ),
        commandparser( context ),
        valuechangeparser( context ),
        program_graph(0),
        for_init_command(0),
        for_incr_command(0)
  {
    BOOST_SPIRIT_DEBUG_RULE( newline );
    BOOST_SPIRIT_DEBUG_RULE( terminationclause );
    BOOST_SPIRIT_DEBUG_RULE( jumpdestination );
    BOOST_SPIRIT_DEBUG_RULE( terminationpart );
    BOOST_SPIRIT_DEBUG_RULE( callstatement );
    BOOST_SPIRIT_DEBUG_RULE( statement );
    BOOST_SPIRIT_DEBUG_RULE( line );
    BOOST_SPIRIT_DEBUG_RULE( content );
    BOOST_SPIRIT_DEBUG_RULE( program );
    BOOST_SPIRIT_DEBUG_RULE( production );
    BOOST_SPIRIT_DEBUG_RULE( valuechange );
    BOOST_SPIRIT_DEBUG_RULE( function );
    BOOST_SPIRIT_DEBUG_RULE( returnstatement );
    BOOST_SPIRIT_DEBUG_RULE( funcstatement );
    BOOST_SPIRIT_DEBUG_RULE( continuepart );
    BOOST_SPIRIT_DEBUG_RULE( callpart );
    BOOST_SPIRIT_DEBUG_RULE( returnpart );
    BOOST_SPIRIT_DEBUG_RULE( ifstatement );
    BOOST_SPIRIT_DEBUG_RULE( ifblock );
    BOOST_SPIRIT_DEBUG_RULE( elseblock );
    
    newline = ch_p( '\n' );
    openbrace = expect_open( ch_p('(') );
    closebrace = expect_close( ch_p(')') );
    opencurly = expect_opencurly( ch_p('{') );
    closecurly = expect_closecurly( ch_p('}') );
    semicolon = expect_semicolon( ch_p(';') );
    condition = expect_condition( conditionparser.parser()[ bind(&ProgramGraphParser::seencondition, this) ] );

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
       >> expect_ident( commonparser.identifier[ bind( &ProgramGraphParser::functiondef, this, _1, _2 ) ] )
       >> *newline
       >> opencurly
       >> content
       >> closecurly
       >> *newline ) [ bind( &ProgramGraphParser::seenfunctionend, this ) ];

    // a program looks like "program { content }".
    program = (
      *newline
      >> str_p( "program" )[ bind( &ProgramGraphParser::startofprogram, this)]
      >> expect_ident( commonparser.identifier[ bind( &ProgramGraphParser::programdef, this, _1, _2 ) ] )
      >> *newline
      >> opencurly
      >> content
      >> closecurly
      >> *newline ) [ bind( &ProgramGraphParser::seenprogramend, this ) ];

    // the content of a program can be any number of lines
    content = *line;

    // a line can be empty or contain a statement. Empty is
    // necessary, because comment's are skipped, but newline's
    // aren't.  So a line like "/* very interesting comment
    // */\n" will reach us as simply "\n"..
    line = !( statement ) >> newline;

    statement = valuechange | callstatement | funcstatement | returnstatement | ifstatement | whilestatement | forstatement; 

    valuechange_parsers =  valuechangeparser.constantDefinitionParser()
        | valuechangeparser.variableDefinitionParser()
        | valuechangeparser.aliasDefinitionParser()
        | valuechangeparser.variableAssignmentParser();

    valuechange = valuechange_parsers[ bind( &ProgramGraphParser::seenvaluechange, this ) ];

    // a call statement: "do xxx until { terminationclauses }"
    callstatement = (
      str_p( "do" ) [ bind( &ProgramGraphParser::startofnewstatement, this ) ]
      >> expect_command ( commandparser.parser()[ bind( &ProgramGraphParser::seencommandcall, this ) ] )
      >> !terminationpart
      ) [ bind( &ProgramGraphParser::seencallstatement, this ) ];

    // a function statement : "call functionname"
    funcstatement = (
      str_p( "call" )
      >> expect_ident( commonparser.identifier[bind( &ProgramGraphParser::seenfuncidentifier, this, _1, _2) ] )
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
        >> *newline >> opencurly >> *newline
        >> terminationclause
        >> *(newline >> !terminationclause)
        >> closecurly;

    forstatement = ( str_p("for") >> openbrace
                     >> !valuechange_parsers[bind(&ProgramGraphParser::seenforinit, this)] >> semicolon
                     >> condition >> semicolon
                     >> !valuechange_parsers[bind(&ProgramGraphParser::seenforincr, this)] >> closebrace
                     ) [bind(&ProgramGraphParser::seenforstatement, this)]
                                  >> ifblock[ bind(&ProgramGraphParser::endforstatement, this) ];

    ifstatement = (str_p("if")
                   >> condition
                   >> expect_then( str_p("then")[bind(&ProgramGraphParser::seenifstatement, this)] )
                   >> expect_ifblock( ifblock[ bind(&ProgramGraphParser::endifblock, this) ] )
                   >> !( *newline >> str_p("else") >> expect_elseblock(ifblock) )
                   )[ bind(&ProgramGraphParser::endifstatement, this) ];

    // ifblock is used for a group of statements or one statement (see also whilestatement)
    ifblock = ( *newline >> ch_p('{') >> *line >> closecurly ) | ( *newline >> statement );

    whilestatement =
        (str_p("while")
         >> condition )
        [bind(&ProgramGraphParser::seenwhilestatement, this)]
         >> ifblock[ bind(&ProgramGraphParser::endwhilestatement, this) ];

    // a termination clause: "if xxx then call yyy" where xxx is
    // a condition, and yyy is an identifier.
    terminationclause =
        str_p( "if" )
            >> condition
            >> expect_then( str_p("then") ) >> *newline
            >> (callpart | returnpart | continuepart);

    continuepart = str_p("continue")[ bind( &ProgramGraphParser::seencontinue, this)];

    callpart = str_p("call")
        >> commonparser.identifier[ bind( &ProgramGraphParser::seencallfunclabel,
                                          this, _1, _2 ) ] ;
    returnpart = str_p("return")[ bind( &ProgramGraphParser::seenreturnlabel, this)];

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
        program_graph->startProgram();
    }

  void ProgramGraphParser::programdef( iter_t begin, iter_t end )
  {
      std::string def(begin, end);
      program_graph->setName( def );
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
  }

  void ProgramGraphParser::startofnewstatement()
  {
      // Set the line number.
      //int ln = mpositer.get_position().line;
      //mcurrentnode->setLineNumber( ln );
  }


    void ProgramGraphParser::seenifstatement() {
        assert(mcondition);
        program_graph->startIfStatement( mcondition, mpositer.get_position().line );
        mcondition = 0;
    }

    void ProgramGraphParser::endifblock() {
        program_graph->endIfBlock();
    }


    void ProgramGraphParser::endifstatement() {
        program_graph->endElseBlock();
    }

    void ProgramGraphParser::seenwhilestatement() {
        assert(mcondition);
        program_graph->startWhileStatement( mcondition, mpositer.get_position().line );
        mcondition = 0;
    }

    void ProgramGraphParser::endwhilestatement() {
        program_graph->endWhileBlock();
    }


    void ProgramGraphParser::seenforinit()
    {
        for_init_command = valuechangeparser.assignCommand();
        valuechangeparser.reset();
    }

    void ProgramGraphParser::seenforincr()
    {
        for_incr_command = valuechangeparser.assignCommand();
        valuechangeparser.reset();
    }

    void ProgramGraphParser::seenforstatement() {
        assert( mcondition );

        if ( for_init_command )
            {
                program_graph->setCommand( for_init_command );
                program_graph->proceedToNext( new ConditionTrue, mpositer.get_position().line );
            }
        for_init_command = 0;

        // A for is nothing more than a while loop...
        program_graph->startWhileStatement( mcondition, mpositer.get_position().line );
        mcondition = 0;
    }

    void ProgramGraphParser::endforstatement() {
        // the last statement is an increment of the 'counter'
        if ( for_incr_command )
            {
                program_graph->setCommand( for_incr_command );
                // Since a valuechange does not add edges, we use this variant
                // to create one.
                program_graph->proceedToNext( new ConditionTrue, mpositer.get_position().line );
            }
        for_incr_command = 0;

        program_graph->endWhileBlock();
    }

  void ProgramGraphParser::seenprogramend()
  {
      // Fake a 'return' statement at the last line.
      program_graph->returnProgram( new ConditionTrue );
      program_graph->proceedToNext( mpositer.get_position().line );
      program_graph->endProgram( context.processor );
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
      program_graph->reset();
      return program_graph;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            std::cerr << "Parse error at line "
                      << mpositer.get_position().line
                      << ": " << e.descriptor << std::endl;
            delete program_graph;
            program_graph = 0;
            return 0;
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
          mcondition = 0;

    }

    void ProgramGraphParser::seencontinue( )
    {
        // Used for "continue"
        assert ( mcondition );

        // connect to next node under given condition.
        program_graph->addConditionEdge( mcondition, program_graph->nextNode() );

        mcondition = 0;
      }
}
