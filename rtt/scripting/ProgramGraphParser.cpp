/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ProgramGraphParser.cxx

                        ProgramGraphParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "rtt/scripting/parser-debug.hpp"
#include "rtt/scripting/parse_exception.hpp"
#include "rtt/scripting/ProgramGraphParser.hpp"
#include "rtt/scripting/ArgumentsParser.hpp"

#include "rtt/scripting/CommandNOP.hpp"
#include "rtt/scripting/CommandDataSource.hpp"
#include "rtt/scripting/ConditionTrue.hpp"
#include "rtt/Logger.hpp"
#include "rtt/scripting/DataSourceCondition.hpp"

#include "rtt/scripting/ConditionComposite.hpp"
#include "rtt/scripting/ConditionFalse.hpp"
#include "rtt/scripting/ConditionOnce.hpp"
#include "rtt/scripting/CommandComposite.hpp"
#include "rtt/scripting/CommandBinary.hpp"

#include "rtt/scripting/TryCommand.hpp"
#include "rtt/scripting/FunctionFactory.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/internal/GlobalService.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#ifdef WIN32
    #ifdef NDEBUG
        #pragma optimize( "", off)
    #endif
#endif

namespace RTT
{
  using namespace boost;
  using namespace detail;



    namespace {
        boost::spirit::classic::assertion<std::string> expect_opencurly("Open curly brace '{' expected.");
        boost::spirit::classic::assertion<std::string> expect_closecurly("Closing curly brace '}' expected in statement block (or could not find out what this line means).");
        boost::spirit::classic::assertion<std::string> expect_closefunction("Closing curly brace '}' expected at end of program or function (or could not find out what this line means).");
        boost::spirit::classic::assertion<std::string> expect_open("Open brace '(' expected.");
        boost::spirit::classic::assertion<std::string> expect_close("Closing brace ')' expected.");
        boost::spirit::classic::assertion<std::string> expect_comma("Expected a comma separator.");
        boost::spirit::classic::assertion<std::string> expect_ident("Expected a valid identifier.");
        boost::spirit::classic::assertion<std::string> expect_semicolon("Semicolon ';' expected after statement.");
        boost::spirit::classic::assertion<std::string> expect_condition("Expected a boolean expression ( a condition ).");
        boost::spirit::classic::assertion<std::string> expect_expression("Expected an expression.");
        boost::spirit::classic::assertion<std::string> expect_command("Expected a command after 'do'.");
        boost::spirit::classic::assertion<std::string> expect_nl("Expected a newline after statement.");
        boost::spirit::classic::assertion<std::string> expect_eof("Invalid input in file.");
        boost::spirit::classic::assertion<std::string> expect_term("No valid termination claues found in do ... until { } block.");
    }


  ProgramGraphParser::ProgramGraphParser( iter_t& positer, TaskContext* t, ExecutionEngine* caller, CommonParser& cp)
      : rootc( t ),context(), fcontext(), mpositer( positer ),
        mcallfunc(),
        implcond(0), mcondition(0), try_cond(0),
        commonparser(cp),
        conditionparser( rootc, caller, cp ),
        valuechangeparser( rootc, cp, t->provides(), caller ),
        expressionparser( rootc, caller, cp ),
        argsparser(0),
        peerparser(rootc, commonparser),
        program_builder( new FunctionGraphBuilder() ),
        for_init_command(0),
        exportf(false),globalf(false),
        ln_offset(0)
  {
      // putting the code in setup() works around a GCC 4.1 bug.
      this->setup();
      this->setup2();
  }

  ProgramGraphParser::~ProgramGraphParser() {
      // necessary in case we were used outside of our parse() methods.
      // we also try to remove the service, because the user will call programParserResult()
      // to do cleanup himself, in which case this becomes a no-op.
      cleanup(true);
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
    condition = expect_condition( conditionparser.parser()[ boost::bind(&ProgramGraphParser::seencondition, this) ] );

    // program is the production rule of this grammar.  The
    // production rule is the rule that the entire input should be
    // matched by...  This line basically means that we're finished
    // ;)
    // Zero or n functions can precede the program.
    production = *( program | function )[boost::bind(&ProgramGraphParser::programtext,this, _1, _2)] >> expect_eof(end_p) ;

    // a function is very similar to a program, but it also has a name
    function = (
            // optional visibility qualifiers:
       !( keyword_p( "export" )[boost::bind(&ProgramGraphParser::exportdef, this)] | keyword_p( "global" )[boost::bind(&ProgramGraphParser::globaldef, this)] | keyword_p("local") )[boost::bind(&ProgramGraphParser::seenvalidinput, this)]
       >> (keyword_p( "function" )[boost::bind(&ProgramGraphParser::seenvalidinput, this)] | commonparser.notassertingidentifier[boost::bind( &ProgramGraphParser::seenreturntype, this, _1, _2)] )
       >> expect_ident( commonparser.identifier[boost::bind(&ProgramGraphParser::seenvalidinput, this)][ boost::bind( &ProgramGraphParser::functiondef, this, _1, _2 ) ] )
       >> !funcargs
       >> opencurly
       >> content
       >> expect_closefunction( ch_p('}') )[ boost::bind( &ProgramGraphParser::seenfunctionend, this ) ]
       );

    // the function's definition args :
    funcargs = ch_p('(') >> ( !str_p("void") >> ch_p(')') | ((
         valuechangeparser.bareDefinitionParser()[boost::bind(&ProgramGraphParser::seenfunctionarg, this)]
             >> *(ch_p(',')>> valuechangeparser.bareDefinitionParser()[boost::bind(&ProgramGraphParser::seenfunctionarg, this)]) )
        >> closebrace ));

    // a program looks like "program { content }".
    program =
        keyword_p( "program" )[boost::bind(&ProgramGraphParser::seenvalidinput, this)]
      >> expect_ident( commonparser.identifier[ boost::bind( &ProgramGraphParser::programdef, this, _1, _2 ) ] )
      >> opencurly
      >> content
      >> expect_closefunction( ch_p('}') )[ boost::bind( &ProgramGraphParser::seenprogramend, this ) ];

    // the content of a program can be any number of lines
    content = *line;

    // a line can be empty or contain a statement. Empty is
    // necessary, because comment's are skipped, but newline's
    // aren't.  So a line like "/* very interesting comment
    // */\n" will reach us as simply "\n"..
    //line = !( statement ) >> eol_p;
    line = statement[boost::bind(&ProgramGraphParser::noskip_eol, this )] >> commonparser.eos[boost::bind(&ProgramGraphParser::skip_eol, this )];

    statement = valuechange | trystatement | funcstatement | returnstatement | ifstatement | whilestatement | forstatement | breakstatement | dostatement;

    valuechange = valuechangeparser.parser()[ boost::bind( &ProgramGraphParser::seenvaluechange, this ) ];

    // take into account deprecated 'do' and 'set'
    dostatement = !keyword_p("do") >> !keyword_p("set") >> !keyword_p("call") >>
            (
             ( keyword_p("yield") | keyword_p("nothing"))[boost::bind(&ProgramGraphParser::seenyield,this)]
            | expressionparser.parser()[ boost::bind(&ProgramGraphParser::seenstatement,this) ]
            );

    // a try statement: "try xxx catch { stuff to do once on any error} "
    trystatement =
        keyword_p("try")
         >> expect_command ( expressionparser.parser()[ boost::bind( &ProgramGraphParser::seentrystatement, this ) ] )
         >> !catchpart;

  }

    void ProgramGraphParser::initBodyParser(const std::string& name, Service::shared_ptr stck, int offset) {
        ln_offset = offset;
        assert(program_builder != 0 );
        program_builder->startFunction(name);
        this->setStack( stck );
        this->clearParseState();
    }

    rule_t& ProgramGraphParser::programParser() {
        return program;
    }

    rule_t& ProgramGraphParser::functionParser() {
        return function;
    }

    rule_t& ProgramGraphParser::bodyParser() {
        // content is the bodyparser of a program or function
        return content;
    }

    rule_t& ProgramGraphParser::statementParser() {
        // line is the statement parser of a program or function
        return line;
    }

    ProgramInterfacePtr ProgramGraphParser::programParserResult() {
        ProgramInterfacePtr result;
        if (program_list.empty())
            return result;
        program_text = "Bug: Program Text to be set by Parser.";
        // set the program text in each program :
        program_list.front()->setText( program_text );
        result=program_list.front();
        this->cleanup(false);
        program_list.clear();
        return result;
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

//    ProgramInterfacePtr ProgramGraphParser::statementParserResult() {
//
//        // Fake a 'return' statement at the last line.
//        program_builder->returnFunction( new ConditionTrue, mpositer.get_position().line - ln_offset );
//        program_builder->proceedToNext( mpositer.get_position().line - ln_offset);
//        return program_builder->getFunction();
//    }

    bool ProgramGraphParser::parserUsed() const {
        return parserused;
    }

    void ProgramGraphParser::setStack(Service::shared_ptr st) {
        context = st;
        valuechangeparser.load(context);
    }

    void ProgramGraphParser::clearParseState() {
        exportf = false;
        parserused = false;
        rettype.clear();
    }

    void ProgramGraphParser::startofprogram()
    {
    }

  void ProgramGraphParser::programdef( iter_t begin, iter_t end )
  {
      // Now that we got the name, set everything up:

      std::string def(begin, end);

      if ( rootc->provides()->hasService( def ) )
          throw parse_exception_semantic_error("Service with name '" + def + "' already present in task '"+rootc->getName()+"'.");

      FunctionGraphPtr pi(program_builder->startFunction( def ));
      // ptsk becomes the owner of pi.
      ProgramServicePtr ptsk(new ProgramService( pi, rootc ));
      pi->setProgramService(ptsk);
      pi->setUnloadOnStop( false ); // since we assign a service, set this to false.
      context = ptsk;
      rootc->provides()->addService( ptsk );
  }

  void ProgramGraphParser::programtext( iter_t begin, iter_t end )
  {
      // we set it in the parse() function. It is set to the whole script such that line numbers are correct.
      //program_text = std::string(begin, end);
  }

  void ProgramGraphParser::exportdef()
  {
      exportf = true;
  }

  void ProgramGraphParser::globaldef()
  {
      globalf = true;
  }

  void ProgramGraphParser::seenreturntype( iter_t begin, iter_t end )
  {
      rettype = std::string(begin, end);
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
      // only redefining a function twice in the same file is an error. If the function
      // was already added to the scripting or component interface before, we replace it
      // and warn about it in seenfunctionend():
      if ( mfuncs.count( funcdef ) )
          throw parse_exception_semantic_error("function " + funcdef + " redefined.");

      AttributeBase* retarg = 0;
      if ( !rettype.empty() && rettype != "void") {
          TypeInfo* type = TypeInfoRepository::Instance()->type( rettype );
          if ( type == 0 )
              throw_( iter_t(), "Return type '" + rettype + "' for function '"+ funcdef +"' is an unknown type." );
          retarg = type->buildAttribute("result");
      }

      mfuncs[funcdef] = program_builder->startFunction( funcdef );
      program_builder->getFunction()->setResult( retarg );

      rettype.clear();

      // Connect the new function to the relevant contexts.
      // 'fun' acts as a stack for storing variables.
      fcontext.reset( new Service(funcdef) );
      context = fcontext;
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
      boost::shared_ptr<ProgramInterface> mfunc = program_builder->endFunction( mpositer.get_position().line - ln_offset );

      // export the function in the context's interface.
      if (exportf) {
          std::map<const DataSourceBase*, DataSourceBase*> dummy;
          FunctionFactory* cfi = new FunctionFactory(ProgramInterfacePtr(mfunc->copy(dummy)), rootc->engine() ); // execute in the processor which has the command.
          if (rootc->provides()->hasMember( mfunc->getName() ) )
              log(Warning) << "Redefining function '"<< rootc->getName() << "." << mfunc->getName() << "': only new programs will use this new function." <<endlog();
          rootc->provides()->add(mfunc->getName(), cfi );
          Logger::log() << Logger::Info << "Exported Function '" << mfunc->getName() << "' added to task '"<< rootc->getName() << "'" <<Logger::endl;
      }
      // attach the function to the global service interface.
      else if (globalf){
          std::map<const DataSourceBase*, DataSourceBase*> dummy;
          FunctionFactory* cfi = new FunctionFactory(ProgramInterfacePtr(mfunc->copy(dummy)), rootc->engine() ); // execute in the processor which has the command.
          if (GlobalService::Instance()->provides()->hasMember( mfunc->getName() ) )
              log(Warning) << "Redefining function '"<< GlobalService::Instance()->getName() << "."<< mfunc->getName() << "': only new programs will use this new function." <<endlog();
          GlobalService::Instance()->provides()->add(mfunc->getName(), cfi );
          Logger::log() << Logger::Debug << "Seen Function '" << mfunc->getName() << "' for Global Service." <<Logger::endl;
      } else {
          std::map<const DataSourceBase*, DataSourceBase*> dummy;
          FunctionFactory* cfi = new FunctionFactory(ProgramInterfacePtr(mfunc->copy(dummy)), rootc->engine() ); // execute in the processor which has the command.
          if (rootc->provides("scripting")->hasMember( mfunc->getName() ) )
              log(Warning) << "Redefining function '"<< rootc->getName() << ".scripting."<< mfunc->getName() << "': only new programs will use this new function." <<endlog();
          rootc->provides("scripting")->add(mfunc->getName(), cfi );
          Logger::log() << Logger::Debug << "Seen Function '" << mfunc->getName() << "' for scripting service of '"<< rootc->getName() << "'" <<Logger::endl;
      }

      fcontext.reset();
      context.reset();

      // reset
      exportf = false; globalf = false;

      valuechangeparser.reset();
  }

  void ProgramGraphParser::seenvalidinput() {
      parserused = true;
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

  void ProgramGraphParser::seenreturnvalue()
  {
      AttributeBase* ar =program_builder->getFunction()->getResult();
      if ( ar == 0) {
          throw parse_exception_syntactic_error("Returning a value in a function returning (void).");
      }
      DataSourceBase::shared_ptr expr  = expressionparser.getResult().get();
      expressionparser.dropResult();
      try {
          ActionInterface* assigncomm = ar->getDataSource()->updateAction( expr.get() );
          // assign the return value to the return argument.
          program_builder->setCommand( assigncomm );
          program_builder->proceedToNext( new ConditionTrue(), mpositer.get_position().line - ln_offset );
      }
      catch(...) {
          // catch exception from updateAction.
          throw parse_exception_syntactic_error("Could not convert '" + expr->getType() + "' to '"+ ar->getDataSource()->getType() +"' in return statement.");
      }
  }

  void ProgramGraphParser::seenbreakstatement()
  {
      if ( program_builder->inLoop() ) {
          program_builder->breakLoop();
          program_builder->proceedToNext( mpositer.get_position().line - ln_offset );
      } else
          throw parse_exception_syntactic_error("Illegal use of 'break'. Can only be used within for and while loops.");
  }

  void ProgramGraphParser::seenfuncidentifier( iter_t begin, iter_t end )
  {
      // store the part after 'call'
      std::string fname(begin, end);
      if ( mfuncs.count(fname) == 0 )
          throw parse_exception_semantic_error("calling function " + fname + " but it is not defined ( remove the 'call' keyword ).");
      if ( fname == program_builder->getFunction()->getName() )
          throw parse_exception_semantic_error("calling function " + fname + " recursively is not allowed.");

      mcallfunc = mfuncs[ fname ];

      // Parse the function's args in the programs context.
      argsparser = new ArgumentsParser( expressionparser, rootc, rootc->provides(),
                                        "this", fname );
      arguments = argsparser->parser();

  }

  void ProgramGraphParser::seencallfuncargs()
  {
      callfnargs = argsparser->result();
  }

  void ProgramGraphParser::seencallfuncstatement()
  {
      log(Warning) << " 'call' has been deprecated. Please remove this keyword." << endlog();
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
        commonparser.skipeol = true;
    }

    void ProgramGraphParser::noskip_eol() {
        commonparser.skipeol = false;
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
        std::pair<ActionInterface*, ConditionInterface*> comcon;
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
        std::pair<ActionInterface*, ConditionInterface*> comcon;
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
      ActionInterface* ac = 0;
      std::vector<ActionInterface*> acv = valuechangeparser.assignCommands();
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

    void ProgramGraphParser::seenforinit_expr()
    {
        DataSourceBase::shared_ptr expr = expressionparser.getResult();
        expressionparser.dropResult();
        for_init_command = new CommandDataSource( expr );
    }

    void ProgramGraphParser::seenforincr()
    {
        DataSourceBase::shared_ptr expr = expressionparser.getResult();
        expressionparser.dropResult();
        for_incr_command.push( new CommandDataSource( expr ) );
    }

    void ProgramGraphParser::seenemptyforincr()
    {
        for_incr_command.push( 0 );
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
        std::pair<ActionInterface*, ConditionInterface*> comcon;
        comcon = conditionparser.getParseResultAsCommand();
        program_builder->setCommand( comcon.first );
        program_builder->startWhileStatement( comcon.second, mpositer.get_position().line - ln_offset );
        delete mcondition;
        mcondition = 0;
    }

    void ProgramGraphParser::endforstatement() {
        // the last statement is a _conditional_ increment of the 'counter'
        ActionInterface* incr = for_incr_command.top();
        for_incr_command.pop();
        // is null or an action to increment
        if ( incr )
            {
                program_builder->setCommand( incr );
                // Since a valuechange does not add edges, we use this variant
                // to create one.
                program_builder->proceedToNext( new ConditionTrue, mpositer.get_position().line - ln_offset );
            }
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
    skip_parser_t skip_parser = comment_p( "#" ) | comment_p( "//" ) | comment_p( "/*", "*/" ) | (space_p - eol_p) | commonparser.skipper;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );
    program_list.clear();

    // todo :Add a universal collect/collectIfDone/ret(sendh, args) operationfactoryparts.
    //rootc->add("collect",&ProgramGraphParser::collectHandler, this)

    try {
      if ( ! production.parse( scanner ) )
      {
          // This gets shown if we didn't even get the chance to throw an exception :
        cleanup(true);
        throw file_parse_exception(new parse_exception_syntactic_error( " no valid input found." ),
                                   mpositer.get_position().file, mpositer.get_position().line,
                                   mpositer.get_position().column );
      }
      program_text = std::string( begin_copy, begin ); // begin is by reference.
      // set the program text in each program :
      for (std::vector<FunctionGraphPtr>::iterator it= program_list.begin();it!=program_list.end();++it)
          (*it)->setText( program_text );
      this->cleanup(false);
      std::vector<ProgramInterfacePtr> result;
      for (std::vector<FunctionGraphPtr>::iterator it= program_list.begin();it!=program_list.end();++it)
          result.push_back( *it );
      program_list.clear();
      return result;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            cleanup(true);
            program_list.clear();
            throw file_parse_exception(
                new parse_exception_syntactic_error( e.descriptor ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );

        }
    // Catch our Orocos exceptions
    catch( const parse_exception& e )
    {
        cleanup(true);
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
    //skip_parser_t skip_parser = SKIP_PARSER;
    //iter_pol_t iter_policy( skip_parser );
    iter_pol_t iter_policy( ( comment_p( "#" ) | comment_p( "//" ) | comment_p( "/*", "*/" ) | (space_p - eol_p) | commonparser.skipper  ) );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );

    std::vector< ProgramInterfacePtr > function_list;

    try {
      if ( ! functions.parse( scanner ) )
      {
          // This gets shown if we didn't even get the chance to throw an exception :
        cleanup(false);
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

      this->cleanup(false);
      return function_list;
    }
    // Catch Boost::Spirit exceptions
    catch( const parser_error<std::string, iter_t>& e )
        {
            cleanup(false);
            throw file_parse_exception(
                new parse_exception_syntactic_error( e.descriptor ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );

        }
    // Catch our Orocos exceptions
    catch( const parse_exception& e )
    {
        cleanup(false);
        throw file_parse_exception(
                e.copy(), mpositer.get_position().file,
                mpositer.get_position().line, mpositer.get_position().column );
    }
  }

  void ProgramGraphParser::cleanup(bool unload_service)
  {
      if (unload_service && rootc && context)
          rootc->provides()->removeService( context->getName() );
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
      while (!for_incr_command.empty() ) {
          delete for_incr_command.top();
          for_incr_command.pop();
      }
      // cleanup all functions :
      fcontext.reset();
      exportf = false; globalf = false;
      rettype.clear();
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
      context.reset();

      valuechangeparser.reset();
      conditionparser.reset();
      peerparser.reset();
  }

  void ProgramGraphParser::seentrystatement()
  {
      // a try expression/method call.
      ActionInterface*   command;
      DataSourceBase::shared_ptr expr  = expressionparser.getResult().get();
      expressionparser.dropResult();
      DataSource<bool>* bexpr = dynamic_cast<DataSource<bool>*>(expr.get());
      if (bexpr == 0) {
          // if not returning a bool, the try is useless.
          command = new CommandDataSource( expr );
          try_cond = new ConditionFalse(); // never execute catch part.
          program_builder->setCommand(command);
      } else {
          command = new CommandDataSourceBool( bexpr );

          // try-wrap the asyn or dispatch command, store the result in try_cond.
          TryCommand* trycommand =  new TryCommand( command );
          // returns true if failure :
          TryCommandResult* tryresult = new TryCommandResult( trycommand->result(), true );
          program_builder->setCommand( trycommand );
          try_cond = tryresult; // save try_cond for catch part (ie true if failure)
      }
      if ( program_builder->buildEdges() == 0 )
          program_builder->proceedToNext( new ConditionTrue(), mpositer.get_position().line - ln_offset );
      else
          program_builder->proceedToNext( mpositer.get_position().line - ln_offset );      // we get the data from commandparser
  }

  void ProgramGraphParser::seenstatement()
  {
      // an expression/method call (former do).
      DataSourceBase::shared_ptr expr  = expressionparser.getResult().get();
      expressionparser.dropResult();
      DataSource<bool>* bexpr = dynamic_cast<DataSource<bool>*>(expr.get());
      if (bexpr)
          program_builder->setCommand( new CommandDataSourceBool( bexpr ) );
      else
          program_builder->setCommand( new CommandDataSource( expr ) );
      if ( program_builder->buildEdges() == 0 )
          program_builder->proceedToNext( new ConditionTrue(), mpositer.get_position().line - ln_offset );
      else
          program_builder->proceedToNext( mpositer.get_position().line - ln_offset );
  }

  void ProgramGraphParser::seenyield()
  {
      // a yield branch
      program_builder->setCommand( new CommandNOP );
      program_builder->proceedToNext( new ConditionOnce(false), mpositer.get_position().line - ln_offset );
  }

  void ProgramGraphParser::seenvaluechange()
  {
    // some value changes generate a command, we need to add it to
    // the program.
      ActionInterface* ac = 0;
      std::vector<ActionInterface*> acv = valuechangeparser.assignCommands();
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
        // @todo complete the impl for for/while loops.
        // Used for "continue"
        assert ( mcondition );

        // connect to next node under given condition.
        program_builder->addConditionEdge( mcondition, program_builder->nextNode() );

        mcondition = 0;
      }
}
