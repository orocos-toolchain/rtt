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
#include "execution/ConditionComposite.hpp"
#include "execution/TryCommand.hpp"

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
        assertion<std::string> expect_and_command("Expected a command after 'and'.");
    }


  ProgramGraphParser::ProgramGraphParser( iter_t& positer, TaskContext* t)
      : context( t ), mpositer( positer ),
        mfunc(0), mcallfunc(0), 
        implcond(0), mcondition(0), try_cond(0), dc(0),
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
    BOOST_SPIRIT_DEBUG_RULE( dostatement );
    BOOST_SPIRIT_DEBUG_RULE( trystatement );
    BOOST_SPIRIT_DEBUG_RULE( catchpart );
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
    production = (*function >> *program)[bind(&ProgramGraphParser::programtext,this, _1, _2)] ;

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

    statement = valuechange | dostatement | trystatement | funcstatement | returnstatement | ifstatement | whilestatement | forstatement;

    valuechange_parsers =  valuechangeparser.constantDefinitionParser()
        | valuechangeparser.variableDefinitionParser()
        | valuechangeparser.aliasDefinitionParser()
        | valuechangeparser.variableAssignmentParser();

    valuechange = valuechange_parsers[ bind( &ProgramGraphParser::seenvaluechange, this ) ];

    // a do statement: "do xxx <and y> <and...> until { terminationclauses }"
    dostatement =
        (str_p( "do" ) [ bind( &ProgramGraphParser::startofnewstatement, this, "do" ) ]
         >> (expect_command ( commandparser.parser()[ bind( &ProgramGraphParser::seencommandcall, this ) ] )
             >> *andpart)[bind( &ProgramGraphParser::seencommands, this )] >> !terminationpart 
         ) [ bind( &ProgramGraphParser::seendostatement, this ) ];

    // a try statement: "try xxx <and y> <and...> until { terminationclauses } catch { stuff to do once on any error} "
    trystatement = 
        (str_p("try") [ bind(&ProgramGraphParser::startofnewstatement, this, "try")]
         >> (expect_command ( commandparser.parser()[ bind( &ProgramGraphParser::seencommandcall, this ) ] )
             >> *andpart)[bind( &ProgramGraphParser::seencommands, this )] >> !terminationpart
         ) [ bind( &ProgramGraphParser::seendostatement, this ) ]
         >> !catchpart;

    andpart = *newline >> str_p("and")
        >> expect_and_command ( commandparser.parser()[ bind( &ProgramGraphParser::seenandcall, this ) ] );

    catchpart = *newline >> (str_p("catch") [bind(&ProgramGraphParser::startcatchpart, this)]
                 >> ifblock )[bind(&ProgramGraphParser::seencatchpart, this)];

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
        *newline >>
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

  void ProgramGraphParser::seencommands()
  {
      // Chain all implicit termination conditions into 'done' :
      std::vector<ConditionInterface*>::iterator it = implcond_v.begin();
      implcond = *it;
      while ( ++it != implcond_v.end() ) {
          implcond = new ConditionBinaryComposite< std::logical_and<bool> >( implcond, *it ) ;
      }
      implcond_v.clear();

      context->valueRepository.setValue(
      "done", new ParsedAliasValue<bool>(
        new DataSourceCondition( implcond->clone() ) ) );
      dc = 0;
  }

  void ProgramGraphParser::seendostatement()
  {
      // assert(implcond);
      // Called after a whole command statement is parsed.
      // a CommandNode should have at least one edge
      // If it doesn't, then we add a default one,
      // which just moves on to the next node..
      if ( program_graph->currentEdges() == 0 )
          {
              program_graph->proceedToNext( implcond, mpositer.get_position().line );
          }
      else
          {
              // do not need it.
              delete implcond;
              program_graph->proceedToNext( mpositer.get_position().line );
      }
      implcond = 0;

    // the done condition is no longer valid..
    context->valueRepository.removeValue( "done" );
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

  void ProgramGraphParser::programtext( iter_t begin, iter_t end )
  {
      //program_text = std::string(begin, end);
  }

  void ProgramGraphParser::functiondef( iter_t begin, iter_t end )
  {
      // store the function in our map for later
      // referencing.
      std::string funcdef(begin, end);
      if ( mfuncs.count(funcdef) != 0 )
          throw parse_exception_semantic_error("function " + funcdef + " redefined.");

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
       // do we need to wrap the condition in a dispatch condition ?
       // if so, mcondition is only evaluated if the command is dispatched.
       if ( dc )
          mcondition = new ConditionBinaryComposite< std::logical_and<bool> >( dc, mcondition );

       // leaves the condition in the parser, if we want to use
       // getParseResultAsCommand();
       // mcondition is only used with seen*label statements,
       // when the command and condition are associated,
       // not in the branching where the evaluation of the
       // condition is the command.
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
          throw parse_exception_semantic_error("calling function " + fname + " but it is not defined.");

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

  void ProgramGraphParser::startofnewstatement(const std::string& type)
  {
      // Set the line number.
      //int ln = mpositer.get_position().line;
      //mcurrentnode->setLineNumber( ln );

      // a 'do' fails on the first rejected command,
      // a 'try' tries all commands.
      if ( type == "do")
          try_cmd = false;
      else if (type == "try")
          try_cmd = true;
      else
          assert(false);
  }

    void ProgramGraphParser::startcatchpart() {
        // we saved the try_cond in the previous try statement,
        // now process like it said if ( try_cond ) then {...}
        assert( try_cond );
        program_graph->startIfStatement( try_cond, mpositer.get_position().line );
        try_cond = 0;
    }

    void ProgramGraphParser::seencatchpart() {
        this->endifblock();
        this->endifstatement(); // there is no 'else' part, so close the statement
    }

    void ProgramGraphParser::seenifstatement() {
        assert(mcondition);
        // transform the evaluation in a command, and pass the result
        // as a condition
        std::pair<CommandInterface*, ConditionInterface*> comcon;
        comcon = conditionparser.getParseResultAsCommand();
        program_graph->setCommand( comcon.first );
        program_graph->startIfStatement( comcon.second, mpositer.get_position().line );

        // we did not need this.
        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endifblock() {
        program_graph->endIfBlock(mpositer.get_position().line);
    }


    void ProgramGraphParser::endifstatement() {
        program_graph->endElseBlock(mpositer.get_position().line);
    }

    void ProgramGraphParser::seenwhilestatement() {
        // analogous to seenifstatement
        // the evaluation is a command.
        assert(mcondition);
        std::pair<CommandInterface*, ConditionInterface*> comcon;
        comcon = conditionparser.getParseResultAsCommand();
        program_graph->setCommand( comcon.first );
        program_graph->startWhileStatement( comcon.second, mpositer.get_position().line );

        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endwhilestatement() {
        program_graph->endWhileBlock(mpositer.get_position().line);
    }


    void ProgramGraphParser::seenforinit()
    {
        // the for loop is different from the while and if branch
        // structures in that it places an init command before the loop.
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

        // first insert the initialisation command.
        if ( for_init_command )
            {
                program_graph->setCommand( for_init_command );
                program_graph->proceedToNext( new ConditionTrue, mpositer.get_position().line );
            }
        for_init_command = 0;

        // A for is nothing more than a while loop...
        std::pair<CommandInterface*, ConditionInterface*> comcon;
        comcon = conditionparser.getParseResultAsCommand();
        program_graph->setCommand( comcon.first );
        program_graph->startWhileStatement( comcon.second, mpositer.get_position().line );
        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endforstatement() {
        // the last statement is a _conditional_ increment of the 'counter'
        if ( for_incr_command )
            {
                program_graph->setCommand( for_incr_command );
                // Since a valuechange does not add edges, we use this variant
                // to create one.
                program_graph->proceedToNext( new ConditionTrue, mpositer.get_position().line );
            }
        for_incr_command = 0;

        program_graph->endWhileBlock(mpositer.get_position().line);
    }

  void ProgramGraphParser::seenprogramend()
  {
      // Fake a 'return' statement at the last line.
      program_graph->returnProgram( new ConditionTrue );
      program_graph->proceedToNext( mpositer.get_position().line );
      program_graph->endProgram();
      program_graph->reset();
      program_list.push_back(program_graph);
      program_graph = new ProgramGraph(); // will be deleted if no other progs follow
  }

  std::vector<ProgramGraph*> ProgramGraphParser::parse( iter_t& begin, iter_t end )
  {
      // end is not used !
    iter_t begin_copy = begin;
    skip_parser_t skip_parser = SKIP_PARSER;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );
    program_list.clear();

    // we need this, because if we encounter a function def,
    // a program_graph must be present.
    program_graph = new ProgramGraph();
    
    try {
      if ( ! production.parse( scanner ) )
      {
        std::cerr << "Syntax error at line "
                  << mpositer.get_position().line
                  << std::endl;
        delete program_graph;
        program_graph = 0;
        for (std::vector<ProgramGraph*>::iterator it= program_list.begin();it!=program_list.end();++it)
            delete *it;
        program_list.clear();
        return std::vector<ProgramGraph*>();
      }
      // this program_graph is empty...( seenprogramend() )
      delete program_graph;
      program_graph = 0;
      program_text = std::string( begin_copy, begin ); // begin is by reference.
      // set the program text in each program :
      for (std::vector<ProgramGraph*>::iterator it= program_list.begin();it!=program_list.end();++it)
          (*it)->setText( program_text );
      return program_list;
    }
    // Catch Boost::Spirit exceptions
    catch( const parser_error<std::string, iter_t>& e )
        {
            delete program_graph;
            program_graph = 0;
            for (std::vector<ProgramGraph*>::iterator it= program_list.begin();
                 it!=program_list.end();
                 ++it)
                delete *it;
            program_list.clear();
            throw file_parse_exception(
                new parse_exception_syntactic_error( e.descriptor ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );

        }
    // Catch our Orocos exceptions
    catch( const parse_exception& e )
    {
      delete program_graph;
      program_graph = 0;
      for (std::vector<ProgramGraph*>::iterator it= program_list.begin();
           it!=program_list.end();
           ++it)
          delete *it;
      program_list.clear();
      throw file_parse_exception(
                e.copy(), mpositer.get_position().file,
                mpositer.get_position().line, mpositer.get_position().column );
    }
  }

  void ProgramGraphParser::seencommandcall()
  {
      // we get the data from commandparser
      CommandInterface*   command;
      command  = commandparser.getCommand();
      implcond = commandparser.getImplTermCondition();

      if ( !try_cmd ) {
          program_graph->setCommand( command );
      } else {
          // try-wrap the command, store the result in try_cond.
          TryCommand* trycommand =  new TryCommand( command );
          try_cond = new TryCommandResult( trycommand->result() );
          program_graph->setCommand( trycommand );
      }

    implcond_v.push_back(implcond); // store

    // check if we must store the dispatchCondition
    if ( commandparser.dispatchCondition() != 0 )
        dc = commandparser.dispatchCondition()->clone();

    commandparser.reset();
  }

    namespace {
        struct CommandComposite : public CommandInterface
        {
            CommandInterface* _f;
            CommandInterface* _s;
            CommandComposite( CommandInterface* f, CommandInterface* s)
                : _f(f), _s(s) {}
            virtual ~CommandComposite() {
                delete _f;
                delete _s;
            }
            virtual bool execute() {
                return _f->execute() && _s->execute();
            }
            virtual void reset() {
                _f->reset();
                _s->reset();
            }
            virtual CommandInterface* clone() const {
                return new CommandComposite( _f->clone(), _s->clone() );
            }
            virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new CommandComposite( _f->copy( alreadyCloned ), _s->copy( alreadyCloned ) );
            }
        };
    }

  void ProgramGraphParser::seenandcall()
  {
      // retrieve a clone of the previous 'do' or 'and' command:
    CommandInterface* oldcmnd = program_graph->getCommand( program_graph->currentNode() )->clone();
    assert(oldcmnd);
    // set composite command : (oldcmnd can not be zero)
    CommandInterface* compcmnd;
    if ( !try_cmd )
        compcmnd = new CommandComposite( oldcmnd,
                                         commandparser.getCommand() );
    else {
        TryCommand*      trycommand = new TryCommand( commandparser.getCommand() );
        TryCommandResult* tryresult = new TryCommandResult( trycommand->result() );
        compcmnd = new CommandComposite( oldcmnd,
                                           trycommand );
        try_cond = new ConditionBinaryComposite< std::logical_or<bool> >( try_cond, tryresult );
    }
        
    program_graph->setCommand( compcmnd ); // this deletes the old command (hence the clone) !

    implcond_v.push_back( commandparser.getImplTermCondition() );

    // check if we must store the dispatchCondition
    // They are composed into one big condition, guarding all
    // the other condition branches. Only if all dc's say the
    // command is accepted, the branch opens for evaluation.
    if ( commandparser.dispatchCondition() != 0 ) {
        if ( dc )
            dc = new ConditionBinaryComposite< std::logical_and<bool> >( dc, commandparser.dispatchCondition()->clone() );
        else
            dc = commandparser.dispatchCondition()->clone();
    }

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
