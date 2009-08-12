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

#include "parser-debug.hpp"
#include "parse_exception.hpp"
#include "ProgramGraphParser.hpp"
#include "ArgumentsParser.hpp"

#include "CommandNOP.hpp"
#include "CommandDataSource.hpp"
#include "ConditionTrue.hpp"
#include "../Logger.hpp"
#include "DataSourceCondition.hpp"

#include "ConditionComposite.hpp"
#include "CommandComposite.hpp"
#include "CommandBinary.hpp"

#include "TryCommand.hpp"
#include "FunctionFactory.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

namespace RTT
{
  using namespace boost;
  using namespace detail;



    namespace {
        assertion<std::string> expect_opencurly("Open curly brace '{' expected.");
        assertion<std::string> expect_closecurly("Closing curly brace '}' expected ( or could not find out what this line means ).");
        assertion<std::string> expect_open("Open brace '(' expected.");
        assertion<std::string> expect_close("Closing brace ')' expected.");
        assertion<std::string> expect_comma("Expected a comma separator.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_semicolon("Semicolon ';' expected after statement.");
        assertion<std::string> expect_condition("Expected a boolean expression ( a condition ).");
        assertion<std::string> expect_expression("Expected an expression.");
        assertion<std::string> expect_command("Expected a command after 'do'.");
        assertion<std::string> expect_nl("Expected a newline after statement.");
        assertion<std::string> expect_eof("Invalid input in file.");
        assertion<std::string> expect_term("No valid termination claues found in do ... until { } block.");
    }


  ProgramGraphParser::ProgramGraphParser( iter_t& positer, TaskContext* t)
      : rootc( t ),context( 0 ), fcontext(0), mpositer( positer ),
        mcallfunc(),
        implcond(0), mcondition(0), try_cond(0),
        conditionparser( rootc ),
        commandparser( rootc, true ), // as_action == true
        valuechangeparser( rootc ),
        expressionparser( rootc ),
        argsparser(0),
        peerparser(rootc),
        program_builder( new FunctionGraphBuilder() ),
        for_init_command(0),
        for_incr_command(0),
        exportf(false),
        ln_offset(0)
  {
      // putting the code in setup() works around a GCC 4.1 bug.
      this->setup();
      this->setup2();
  }

  void ProgramGraphParser::setup() {
    BOOST_SPIRIT_DEBUG_RULE( newline );
    BOOST_SPIRIT_DEBUG_RULE( openbrace );
    BOOST_SPIRIT_DEBUG_RULE( closebrace );
    BOOST_SPIRIT_DEBUG_RULE( opencurly );
    BOOST_SPIRIT_DEBUG_RULE( closecurly );
    BOOST_SPIRIT_DEBUG_RULE( semicolon );
    BOOST_SPIRIT_DEBUG_RULE( condition );
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
    BOOST_SPIRIT_DEBUG_RULE( functions );
    BOOST_SPIRIT_DEBUG_RULE( arguments );
    BOOST_SPIRIT_DEBUG_RULE( returnstatement );
    BOOST_SPIRIT_DEBUG_RULE( funcstatement );
    BOOST_SPIRIT_DEBUG_RULE( continuepart );
    BOOST_SPIRIT_DEBUG_RULE( callpart );
    BOOST_SPIRIT_DEBUG_RULE( returnpart );
    BOOST_SPIRIT_DEBUG_RULE( ifstatement );
    BOOST_SPIRIT_DEBUG_RULE( whilestatement );
    BOOST_SPIRIT_DEBUG_RULE( forstatement );
    BOOST_SPIRIT_DEBUG_RULE( breakstatement );
    BOOST_SPIRIT_DEBUG_RULE( ifblock );
    BOOST_SPIRIT_DEBUG_RULE( funcargs );

    //newline = ch_p( '\n' );
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
    production = (functions >> *program)[bind(&ProgramGraphParser::programtext,this, _1, _2)] >> expect_eof(end_p) ;

    functions = *function;

    // a function is very similar to a program, but it also has a name
    function = (
       !str_p( "export" )[bind(&ProgramGraphParser::exportdef, this)]
       >> str_p( "function" )
       >> expect_ident( commonparser.identifier[ bind( &ProgramGraphParser::functiondef, this, _1, _2 ) ] )
       >> !funcargs
       >> opencurly
       >> content
       >> closecurly[ bind( &ProgramGraphParser::seenfunctionend, this ) ]
       );

    // the function's definition args :
    funcargs = ch_p('(') >> ( ch_p(')') || (
        !( valuechangeparser.bareDefinitionParser()[bind(&ProgramGraphParser::seenfunctionarg, this)]
             >> *(ch_p(',')>> valuechangeparser.bareDefinitionParser()[bind(&ProgramGraphParser::seenfunctionarg, this)]) )
        >> closebrace ));

    // a program looks like "program { content }".
    program =
        str_p( "program" )
      >> expect_ident( commonparser.identifier[ bind( &ProgramGraphParser::programdef, this, _1, _2 ) ] )
      >> opencurly
      >> content
      >> closecurly[ bind( &ProgramGraphParser::seenprogramend, this ) ];

    // the content of a program can be any number of lines
    content = *line;

    // a line can be empty or contain a statement. Empty is
    // necessary, because comment's are skipped, but newline's
    // aren't.  So a line like "/* very interesting comment
    // */\n" will reach us as simply "\n"..
    //line = !( statement ) >> eol_p;
    line = statement[bind(&ProgramGraphParser::noskip_eol, this )] >> commonparser.eos[bind(&ProgramGraphParser::skip_eol, this )];

    statement = valuechange | dostatement | trystatement | funcstatement | returnstatement | ifstatement | whilestatement | forstatement | breakstatement;

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

  }

    void ProgramGraphParser::initBodyParser(const std::string& name, OperationInterface* stck, int offset) {
        ln_offset = offset;
        assert(program_builder != 0 );
        program_builder->startFunction(name);
        this->setStack( stck );
    }

    rule_t& ProgramGraphParser::bodyParser() {
        // content is the bodyparser of a program or function
        return content;
    }

    ProgramInterfacePtr ProgramGraphParser::bodyParserResult() {

        // store the variables in the program's taskcontext object.
        valuechangeparser.store( context );
        valuechangeparser.reset();

        // Fake a 'return' statement at the last line.
        program_builder->returnFunction( new ConditionTrue, mpositer.get_position().line - ln_offset );
        program_builder->proceedToNext( mpositer.get_position().line - ln_offset);
        return program_builder->endFunction( mpositer.get_position().line - ln_offset );
    }

    void ProgramGraphParser::setStack(OperationInterface* st) {
        context = st;
    }

  void ProgramGraphParser::seencommands()
  {
      // Chain all implicit termination conditions into 'done' :
      std::vector<ConditionInterface*>::iterator it = implcond_v.begin();
      implcond = *it;
      while ( ++it != implcond_v.end() ) {
          implcond = new ConditionBinaryCompositeAND( implcond, *it ) ;
      }
      implcond_v.clear();

      rootc->attributes()
          ->setValue( new Alias<bool>("done",
                                      new DataSourceCondition( implcond->clone() ) ) );
  }

  void ProgramGraphParser::seendostatement()
  {
      // assert(implcond);
      // Called after a whole command statement is parsed.
      // a CommandNode should have at least one edge
      // If it doesn't, then we add a default one,
      // which just moves on to the next node..
      if ( program_builder->buildEdges() == 0 )
          {
              program_builder->proceedToNext( implcond->clone(), mpositer.get_position().line - ln_offset );
          }
      else
          {
              program_builder->proceedToNext( mpositer.get_position().line - ln_offset );
          }
      delete implcond;
      implcond = 0;

      // the done condition is no longer valid..
      rootc->attributes()->removeValue( "done" );
  }

    void ProgramGraphParser::startofprogram()
    {
    }

  void ProgramGraphParser::programdef( iter_t begin, iter_t end )
  {
      // Now that we got the name, set everything up:

      std::string def(begin, end);

      if ( rootc->getObject( def ) != 0 )
          throw parse_exception_semantic_error("Object with name '" + def + "' already present in task '"+rootc->getName()+"'.");

      FunctionGraphPtr pi(program_builder->startFunction( def ));
      // ptsk becomes the owner of pi.
      ProgramTask* ptsk(new ProgramTask( pi, rootc->engine() ));
      pi->setProgramTask(ptsk);
      context = ptsk;
      rootc->addObject( ptsk );
  }

  void ProgramGraphParser::programtext( iter_t begin, iter_t end )
  {
      //program_text = std::string(begin, end);
  }

  void ProgramGraphParser::exportdef()
  {
      exportf = true;
  }
  void ProgramGraphParser::functiondef( iter_t begin, iter_t end )
  {
      // store the function in our map for later
      // referencing.
      std::string funcdef(begin, end);
      // store the function in the TaskContext current.__functions
//       TaskContext* __f = rootc->getPeer("__functions");
//       if ( __f == 0 ) {
//           // install the __functions if not yet present.
//           __f = new TaskContext("__functions", rootc->engine() );
//           rootc->connectPeers( __f );
//       }

//       if ( __f->hasPeer( funcdef ) )
      if ( mfuncs.count( funcdef ) )
          throw parse_exception_semantic_error("function " + funcdef + " redefined.");

      if ( exportf && rootc->commands()->hasMember( funcdef ))
          throw parse_exception_semantic_error("exported function " + funcdef + " is already defined in "+ rootc->getName()+".");;

      mfuncs[funcdef] = program_builder->startFunction( funcdef );

      // Connect the new function to the relevant contexts.
      // 'fun' acts as a stack for storing variables.
      context = fcontext = new TaskContext(funcdef, rootc->engine() );
  }

  void ProgramGraphParser::seenfunctionarg()
  {
      // the ValueChangeParser stores each variable in the
      // current stack's repository, but we need to inform the
      // FunctionGraph itself about its arguments.
      program_builder->getFunction()->addArgument( valuechangeparser.lastDefinedValue()->clone() );
      valuechangeparser.clear();
  }

  void ProgramGraphParser::seenfunctionend()
  {
      // Fake a 'return' statement at the last line.
      program_builder->returnFunction( new ConditionTrue, mpositer.get_position().line - ln_offset );
      program_builder->proceedToNext( mpositer.get_position().line - ln_offset );
      shared_ptr<ProgramInterface> mfunc = program_builder->endFunction( mpositer.get_position().line - ln_offset );

      // export the function in the context's interface.
      if (exportf) {
          std::map<const DataSourceBase*, DataSourceBase*> dummy;
          FunctionFactory* cfi = new FunctionFactory(ProgramInterfacePtr(mfunc->copy(dummy)), rootc->engine() ); // execute in the processor which has the command.
          rootc->commands()->add(mfunc->getName(), cfi );
          Logger::log() << Logger::Info << "Exported Function '" << mfunc->getName() << "' added to task '"<< rootc->getName() << "'" <<Logger::endl;
      }

      delete fcontext;
      context = fcontext = 0;

      // reset
      exportf = false;

      valuechangeparser.reset();
  }

  void ProgramGraphParser::seencondition()
  {
       mcondition = conditionparser.getParseResult();
       assert( mcondition );

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
      program_builder->returnFunction( new ConditionTrue, mpositer.get_position().line - ln_offset );
      program_builder->proceedToNext(  mpositer.get_position().line - ln_offset );
  }

  void ProgramGraphParser::seenbreakstatement()
  {
      if ( program_builder->inLoop() ) {
          program_builder->breakLoop();
          program_builder->proceedToNext( mpositer.get_position().line - ln_offset );
      } else
          throw parse_exception_syntactic_error("Illegal use of 'break'. Can only be used within for and while loops.");
  }

  void ProgramGraphParser::seenreturnlabel()
  {
      // return label can happen in program and in a function
      assert(mcondition);

      program_builder->returnFunction( mcondition->clone(), mpositer.get_position().line - ln_offset );

      delete mcondition;
      mcondition = 0;
  }

  void ProgramGraphParser::seenfuncidentifier( iter_t begin, iter_t end )
  {
      // store the part after 'call'
      std::string fname(begin, end);
      if ( mfuncs.count(fname) == 0 )
          throw parse_exception_semantic_error("calling function " + fname + " but it is not defined ( use 'do' for calling exported functions ).");
      if ( fname == program_builder->getFunction()->getName() )
          throw parse_exception_semantic_error("calling function " + fname + " recursively is not allowed.");

      mcallfunc = mfuncs[ fname ];

      // Parse the function's args in the programs context.
      argsparser = new ArgumentsParser( expressionparser, rootc, rootc,
                                        "this", fname );
      arguments = argsparser->parser();

  }

  void ProgramGraphParser::seencallfuncargs()
  {
      callfnargs = argsparser->result();
  }

  void ProgramGraphParser::seencallfuncstatement()
  {
      // This function is called if the 'call func' is outside
      // a termination clause.

      // add it to the main program line of execution.
      assert( mcallfunc );
        try
            {
                program_builder->setFunction( mcallfunc, callfnargs );
                // only delete parser, when the args are used.
                delete argsparser;
                argsparser = 0;
                callfnargs.clear();
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    ( rootc->getName(), mcallfunc->getName(), e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( rootc->getName(), mcallfunc->getName(), e.whicharg, e.expected_, e.received_ );
            }
        catch( ... )
            {
                assert( false );
            }

      // The exit node of the function is already connected
      // to program->nextNode().
      program_builder->proceedToNext(mpositer.get_position().line - ln_offset);
  }

    void ProgramGraphParser::skip_eol() {
        eol_skip_functor::skipeol = true;
    }

    void ProgramGraphParser::noskip_eol() {
        eol_skip_functor::skipeol = false;
    }

  void ProgramGraphParser::startofnewstatement(const std::string& type)
  {
      // cleanup previous left conds (should do this in endofnewstatement func)
      // try_cond will be zero if used, othewise, delete it
      delete try_cond;
      try_cond = 0;

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
        program_builder->startIfStatement( try_cond, mpositer.get_position().line - ln_offset );
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
        program_builder->setCommand( comcon.first );
        program_builder->startIfStatement( comcon.second, mpositer.get_position().line - ln_offset );

        // we did not need this.
        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endifblock() {
        program_builder->endIfBlock(mpositer.get_position().line - ln_offset);
    }


    void ProgramGraphParser::endifstatement() {
        program_builder->endElseBlock(mpositer.get_position().line - ln_offset);
    }

    void ProgramGraphParser::seenwhilestatement() {
        // analogous to seenifstatement
        // the evaluation is a command.
        assert(mcondition);
        std::pair<CommandInterface*, ConditionInterface*> comcon;
        comcon = conditionparser.getParseResultAsCommand();
        program_builder->setCommand( comcon.first );
        program_builder->startWhileStatement( comcon.second, mpositer.get_position().line - ln_offset );

        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endwhilestatement() {
        program_builder->endWhileBlock(mpositer.get_position().line - ln_offset);
    }


    void ProgramGraphParser::seenforinit()
    {
        // the for loop is different from the while and if branch
        // structures in that it places an init command before the loop.
      CommandInterface* ac = 0;
      std::vector<CommandInterface*> acv = valuechangeparser.assignCommands();
      // and not forget to reset()..
      valuechangeparser.clear();
      if ( acv.size() == 1) {
          ac = acv.front();
      }
      else if (acv.size() > 1) {
          ac = new CommandComposite( acv );
      }
      for_init_command = ac;
    }

    void ProgramGraphParser::seenforincr()
    {
      CommandInterface* ac = 0;
      std::vector<CommandInterface*> acv = valuechangeparser.assignCommands();
      if ( acv.size() == 1) {
          ac = acv.front();
      }
      else if (acv.size() > 1) {
          ac = new CommandComposite( acv );
      }
      for_incr_command = ac;
      valuechangeparser.clear();
    }

    void ProgramGraphParser::seenforstatement() {
        assert( mcondition );

        // first insert the initialisation command.
        if ( for_init_command )
            {
                program_builder->setCommand( for_init_command );
                program_builder->proceedToNext( new ConditionTrue, mpositer.get_position().line - ln_offset );
            }
        for_init_command = 0;

        // A for is nothing more than a while loop...
        std::pair<CommandInterface*, ConditionInterface*> comcon;
        comcon = conditionparser.getParseResultAsCommand();
        program_builder->setCommand( comcon.first );
        program_builder->startWhileStatement( comcon.second, mpositer.get_position().line - ln_offset );
        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endforstatement() {
        // the last statement is a _conditional_ increment of the 'counter'
        if ( for_incr_command )
            {
                program_builder->setCommand( for_incr_command );
                // Since a valuechange does not add edges, we use this variant
                // to create one.
                program_builder->proceedToNext( new ConditionTrue, mpositer.get_position().line - ln_offset );
            }
        for_incr_command = 0;

        program_builder->endWhileBlock(mpositer.get_position().line - ln_offset);
    }

  void ProgramGraphParser::seenprogramend()
  {
      // Fake a 'return' statement at the last line.
      program_builder->returnFunction( new ConditionTrue, mpositer.get_position().line - ln_offset );
      program_builder->proceedToNext( mpositer.get_position().line - ln_offset );
      program_list.push_back(program_builder->endFunction( mpositer.get_position().line - ln_offset ) );

      // store the variables in the program's taskcontext object.
      valuechangeparser.store( context );
      valuechangeparser.reset();
  }

  std::vector< ProgramInterfacePtr > ProgramGraphParser::parse( iter_t& begin, iter_t end )
  {
      // end is not used !
    iter_t begin_copy = begin;
    skip_parser_t skip_parser = SKIP_PARSER;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );
    program_list.clear();

    try {
      if ( ! production.parse( scanner ) )
      {
          // This gets shown if we didn't even get the chance to throw an exception :
        cleanup();
        throw file_parse_exception(new parse_exception_syntactic_error( " no valid input found." ),
                                   mpositer.get_position().file, mpositer.get_position().line,
                                   mpositer.get_position().column );
      }
      program_text = std::string( begin_copy, begin ); // begin is by reference.
      // set the program text in each program :
      for (std::vector<FunctionGraphPtr>::iterator it= program_list.begin();it!=program_list.end();++it)
          (*it)->setText( program_text );
      this->cleanup();
      std::vector<ProgramInterfacePtr> result;
      for (std::vector<FunctionGraphPtr>::iterator it= program_list.begin();it!=program_list.end();++it)
          result.push_back( *it );
      program_list.clear();
      return result;
    }
    // Catch Boost::Spirit exceptions
    catch( const parser_error<std::string, iter_t>& e )
        {
            cleanup();
            program_list.clear();
            throw file_parse_exception(
                new parse_exception_syntactic_error( e.descriptor ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );

        }
    // Catch our Orocos exceptions
    catch( const parse_exception& e )
    {
        cleanup();
      program_list.clear();
      throw file_parse_exception(
                e.copy(), mpositer.get_position().file,
                mpositer.get_position().line, mpositer.get_position().column );
    }
  }

  std::vector< ProgramInterfacePtr > ProgramGraphParser::parseFunction( iter_t& begin, iter_t end )
  {
      // end is not used !
    iter_t begin_copy = begin;
    skip_parser_t skip_parser = SKIP_PARSER;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );

    std::vector< ProgramInterfacePtr > function_list;

    try {
      if ( ! functions.parse( scanner ) )
      {
          // This gets shown if we didn't even get the chance to throw an exception :
        cleanup();
        throw file_parse_exception(new parse_exception_syntactic_error( " no valid input found." ),
                                   mpositer.get_position().file, mpositer.get_position().line,
                                   mpositer.get_position().column );
      }
      program_text = std::string( begin_copy, begin ); // begin is by reference.
      // set the program text in each function :
      for (funcmap::iterator it= mfuncs.begin();it!=mfuncs.end();++it) {
          it->second->setText( program_text );      // set text.
          function_list.push_back( it->second );
      }

      this->cleanup();
      return function_list;
    }
    // Catch Boost::Spirit exceptions
    catch( const parser_error<std::string, iter_t>& e )
        {
            cleanup();
            throw file_parse_exception(
                new parse_exception_syntactic_error( e.descriptor ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );

        }
    // Catch our Orocos exceptions
    catch( const parse_exception& e )
    {
        cleanup();
        throw file_parse_exception(
                e.copy(), mpositer.get_position().file,
                mpositer.get_position().line, mpositer.get_position().column );
    }
  }

  void ProgramGraphParser::cleanup()
  {
      // after an exception, we can be in any state, so cleanup
      // all temp objects.
      delete argsparser;
      argsparser = 0;
      delete implcond;
      implcond = 0;
      delete mcondition;
      mcondition = 0;
      delete try_cond;
      try_cond = 0;
      delete for_init_command;
      for_init_command = 0;
      delete for_incr_command;
      for_incr_command = 0;
      // cleanup all functions :
      delete fcontext;
      fcontext = 0;
      if ( rootc == 0)
          return;
//       TaskContext* __f = rootc->getPeer("__functions");
//       if ( __f != 0 ) {
//           // first remove rootc from __f itself
//           rootc->disconnectPeers( __f->getName() );
//           delete __f;
//       }
      while ( ! mfuncs.empty() ) {
          mfuncs.erase( mfuncs.begin() );
      }
      context = 0;

      valuechangeparser.reset();
      commandparser.reset();
      conditionparser.reset();
      peerparser.reset();
  }

  void ProgramGraphParser::seencommandcall()
  {
      // we get the data from commandparser
      CommandInterface*   command;
      command  = commandparser.getCommand();
      implcond = commandparser.getImplTermCondition();

      if ( !try_cmd ) {
          program_builder->setCommand( command );
      } else {
          // try-wrap the asyn or dispatch command, store the result in try_cond.
          TryCommand* trycommand =  new TryCommand( command );
          // returns true if failure :
          TryCommandResult* tryresult = new TryCommandResult( trycommand->result(), true );
          program_builder->setCommand( trycommand );
          try_cond = tryresult;
          // go further if command failed or if command finished.
          implcond = new ConditionBinaryCompositeOR(try_cond->clone(), implcond );
      }

    implcond_v.push_back(implcond); // store

    commandparser.reset();
  }

  void ProgramGraphParser::seenandcall()
  {
      // retrieve a clone of the previous 'do' or 'and' command:
    CommandInterface* oldcmnd = program_builder->getCommand( program_builder->buildNode() )->clone();
    assert(oldcmnd);
    // set composite command : (oldcmnd can not be zero)
    CommandInterface* compcmnd;
    // The implcond is already 'corrected' wrt result of evaluate().
    implcond = commandparser.getImplTermCondition();

    if ( !try_cmd )
        compcmnd = new CommandBinary( oldcmnd,
                                      commandparser.getCommand() );
    else {
        TryCommand*      trycommand = new TryCommand( commandparser.getCommand() );
        TryCommandResult* tryresult = new TryCommandResult( trycommand->result(), true );
        compcmnd = new CommandBinary( oldcmnd,
                                      trycommand );
        // chain the failure detections: if try_cond evaluates true, the catch phrase is executed
        // See : condition to enter the 'catch' block.
        try_cond = new ConditionBinaryCompositeOR( try_cond, tryresult );
        // chain the implicit term. conditions: a command is implicitly done if it failed or
        // its implcond is true.
        // See : adding implicit term condition if no if .. then branches were defined.
        implcond = new ConditionBinaryCompositeOR( tryresult->clone(), implcond );
    }

    program_builder->setCommand( compcmnd ); // this deletes the old command (hence the clone) !

    implcond_v.push_back( implcond );

    commandparser.reset();
  }

  void ProgramGraphParser::seenvaluechange()
  {
    // some value changes generate a command, we need to add it to
    // the program.
      CommandInterface* ac = 0;
      std::vector<CommandInterface*> acv = valuechangeparser.assignCommands();
      // and not forget to reset()..
      valuechangeparser.clear();
      if ( acv.size() == 1) {
          ac = acv.front();
      }
      else if (acv.size() > 1) {
          ac = new CommandComposite(acv);
      }
      if (ac) {
          program_builder->setCommand( ac );
          // Since a valuechange does not add edges, we use this variant
          // to create one.
          program_builder->proceedToNext( new ConditionTrue, mpositer.get_position().line - ln_offset );
      }
  }

    void ProgramGraphParser::seencallfunclabel( iter_t begin, iter_t end )
    {
          // Used for "call xyz"
          // lookup mcallfunc
          seenfuncidentifier( begin, end );

          assert( mcondition );
          assert( mcallfunc );
          program_builder->appendFunction( mcondition, mcallfunc, callfnargs);
          mcondition = 0;

    }

    void ProgramGraphParser::seencontinue( )
    {
        // Used for "continue"
        assert ( mcondition );

        // connect to next node under given condition.
        program_builder->addConditionEdge( mcondition, program_builder->nextNode() );

        mcondition = 0;
      }
}
