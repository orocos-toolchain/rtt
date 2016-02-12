/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ExpressionParser.cxx

                        ExpressionParser.cxx -  description
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "ExpressionParser.hpp"
//#include "DumpObject.hpp"

#include "ArgumentsParser.hpp"
#include "../types/Operators.hpp"
#include "DataSourceCondition.hpp"
#include "../internal/DataSourceCommand.hpp"
#include "CmdFunction.hpp"
#include "../internal/GlobalService.hpp"
#include "CommandDataSource.hpp"

#include "DataSourceTime.hpp"
#include "../TaskContext.hpp"
#include "PeerParser.hpp"
#include "../types/Types.hpp"
#include "SendHandleAlias.hpp"

#include <boost/lambda/lambda.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include "rtt-scripting-config.h"
#include <iostream>

namespace RTT
{
    using boost::bind;
    using boost::ref;

    using namespace detail;
    using namespace std;

    namespace {
        boost::spirit::classic::assertion<std::string> expect_open("Open brace expected.");
        boost::spirit::classic::assertion<std::string> expect_close("Closing brace expected ( or could not find out what this line means ).");
        boost::spirit::classic::assertion<std::string> expect_type("Unknown type. Please specify a type.");
        boost::spirit::classic::assertion<std::string> expect_expr("Expected a valid expression.");
        boost::spirit::classic::assertion<std::string> expect_ident("Expected a valid identifier.");
        boost::spirit::classic::assertion<std::string> expect_init("Expected an initialisation value of the value.");
        boost::spirit::classic::assertion<std::string> expect_comma("Expected the ',' separator after expression.");
        boost::spirit::classic::assertion<std::string> expect_timespec("Expected a time specification (e.g. > 10s or > varname ) after 'time' .");

        guard<std::string> my_guard;
    }



  DataCallParser::DataCallParser( ExpressionParser& p, CommonParser& cp, TaskContext* c, ExecutionEngine* caller )
      : ret(), mhandle(), mcmdcnd(0), mobject(), mmethod(),
        mcaller( caller ? caller : c->engine()), mcalltype(DEFAULT_CALLTYPE), commonparser(cp), expressionparser( p ),
        peerparser( c, cp, false ) // accept partial paths
  {
    BOOST_SPIRIT_DEBUG_RULE( datacall );
    BOOST_SPIRIT_DEBUG_RULE( arguments );
    BOOST_SPIRIT_DEBUG_RULE( peerpath );
    BOOST_SPIRIT_DEBUG_RULE( object );
    BOOST_SPIRIT_DEBUG_RULE( method );

    // this parser uses a neat boost.spirit trick to avoid keeping
    // loads of stacks for all parsing data ( this parser needs to be
    // reentrant because it can be called while parsing an argument of
    // a datacall, which has itself been called while parsing an
    // argument of a datacall... ).  Boost.Spirit allows you to change
    // the parser that a rule points to at runtime, so we only create
    // the parser just before it's going to be used, when we know what
    // arguments we want..  See the ArgumentsParser doc for more
    // details..
    peerpath = peerparser.locator();
    object= (commonparser.identifier >> ".")[boost::bind(&DataCallParser::seenobjectname, this, _1, _2)];
    method= ( commonparser.keyword | expect_ident(commonparser.tidentifier))[boost::bind( &DataCallParser::seenmethodname, this, _1, _2 ) ]; // may be send, call or method name.
    datacall =
        ( peerpath >> !object >> method[ boost::bind( &DataCallParser::seendataname, this ) ] >> !arguments)[ boost::bind( &DataCallParser::seendatacall, this ) ];
  }

  void DataCallParser::seenobjectname( iter_t begin, iter_t end )
    {
      std::string name( begin, end );
      mobject = name.substr(0, name.length() - 1);
    }

  void DataCallParser::seenmethodname( iter_t begin, iter_t end )
    {
      std::string name( begin, end );
      if ( name == "send" ) {
          mcalltype = CALLTYPE_SEND;
          mmethod = mobject;
          mobject.clear();
      } else if (name == "cmd" ) {
          mcalltype = CALLTYPE_CMD;
          mmethod = mobject;
          mobject.clear();
      } else if (name == "call" ) {
          mcalltype = CALLTYPE_CALL;
          mmethod = mobject;
          mobject.clear();
      } else {
          mcalltype = DEFAULT_CALLTYPE;
          mmethod = name;
      }
//      cout << "seenmethodname "<< mobject << "." << mmethod<<endl;
    }

  void DataCallParser::seendataname()
  {
      // re-init mobject, might have been cleared during parsing of send(), cmd() or call().
      mobject =  peerparser.object();
      TaskContext* peer = peerparser.peer();
      Service::shared_ptr ops  = peerparser.taskObject();
      peerparser.reset();
//      cout << "seendataname "<< mobject << "." << mmethod<<endl;
      if (true) {
          // it ain't...
          // set the proper object name again in case of a send()
          if ( (mcalltype != DEFAULT_CALLTYPE) && ops)
              mobject = ops->getName();
//          cout << "DCP saw method "<< mmethod <<" of object "<<mobject<<" of peer "<<peer->getName()<<endl;
          // Check sanity of what we parsed:
          if (mmethod != "collect" && mmethod != "collectIfDone" ) {
              if ( ops == 0 || (mobject != "this" && ops->getName() != mobject ) ) {
                  throw parse_exception_no_such_component( peer->getName(), mobject);
              }
              // Check if method exists on current object:
              if ( ops->hasMember(mmethod) == false ) {
                  // Check if it is a method of the global service:
                  if ( ops == peerparser.taskObject() && GlobalService::Instance()->hasMember(mmethod) ) {
                      mobject = "GlobalService";
                      ops = GlobalService::Instance();
                  } else {
                      if ( ops == peerparser.taskObject() && ops->hasService("scripting") && ops->provides("scripting")->hasMember(mmethod) ) {
                          mobject = "scripting";
                          ops = ops->provides("scripting");
                      } else {
                      //DumpObject( peer );
                      if ( mobject != "this" )
                          throw parse_exception_no_such_method_on_component( mobject, mmethod );
                      else
                          throw parse_exception_no_such_method_on_component( peer->getName(), mmethod );
                      }
                  }
              }
          }
      }

      // create an argument parser for the call..
      // Store everything in the ArgumentsParser ! This DataCallParser instance is recursively called !
      ArgumentsParser* argspar =
          new ArgumentsParser( expressionparser, peer, ops,
                               mobject, mmethod );
      // we no longer need these two..
      mobject.clear();
      mmethod.clear();
      mcmdcnd = 0;

      // keep hold of the argspar, we're still going to need it after
      // it's done its work..  ( in seendatacall(), that is.. )
      argparsers.push( argspar );

      // set the arguments parser to the parser provided by the
      // ArgumentsParser we just created..
      arguments = argspar->parser();
  }

  void DataCallParser::seendatacall()
  {
    ArgumentsParser* argspar = argparsers.top();
    argparsers.pop();
    std::string obj = argspar->objectname();
    std::string meth = argspar->methodname();
    std::vector<DataSourceBase::shared_ptr> args = argspar->result();
    Service::shared_ptr peer = argspar->object();
    delete argspar;
    assert(peer && "peer may never be null.");
//    cout << "seendatacall "<< mobject << "." << mmethod<<endl;

    if ( true ) {
        // plain method or collect/collectIfDone

        Service::shared_ptr ops = peer;
        // we already checked for the existence of this object and method
        // in seendataname()..
        peerparser.reset();

        try {
            if ( (meth == "collect" || meth == "collectIfDone") && !ops->hasMember(mmethod) ) {
                if ( ops->hasAttribute(obj) ) {
                    SendHandleAlias* sha = dynamic_cast<SendHandleAlias*>( peer->getValue(obj) );
                    if (sha) {
                        // add SendHandle DS for Collect:
                        args.insert( args.begin(), sha->getDataSource() );
                        if (meth == "collect")
                            ret = sha->getFactory()->produceCollect(args, new ValueDataSource<bool>(true) );// blocking
                        else // (meth == "collectIfDone")
                            ret = sha->getFactory()->produceCollect(args, new ValueDataSource<bool>(false) );// non-blocking
                        return;
                    }
                }
                throw parse_exception_fatal_semantic_error( obj + "."+meth +": "+ obj +" is not a valid SendHandle object.");
            }

            unsigned int arity = ops->getCollectArity(meth);
            switch(mcalltype) {
            case DEFAULT_CALLTYPE:
            case CALLTYPE_CALL:
                ret = ops->produce( meth, args, mcaller );
                mhandle.reset();
                break;
            case CALLTYPE_SEND:
                ret = ops->produceSend( meth, args, mcaller );
                mhandle.reset( new SendHandleAlias( meth, ops->produceHandle(meth), ops->getPart(meth)) );
                break;
            case CALLTYPE_CMD:
                DataSourceBase::shared_ptr sendds = ops->produceSend( meth, args, mcaller );
                args.clear();
                args.push_back( sendds ); // store the produceSend DS for collecting:
                for ( unsigned int i =0; i != arity; ++i) {
                    args.push_back( ops->getOperation(meth)->getCollectType( i + 1 )->buildValue() ); // this is only to satisfy produceCollect. We ignore the results...
                }

                DataSource<SendStatus>::shared_ptr collectds
                        = boost::dynamic_pointer_cast<DataSource<SendStatus> >(
                              ops->produceCollect( meth, args, new ValueDataSource<bool>(false) )
                          ); // non-blocking, need extra condition
                assert(collectds);

                ret = new ActionAliasDataSource<SendStatus>(new CommandDataSource( sendds ), collectds.get() );
                mcmdcnd = new CmdCollectCondition( collectds ); // Replaces RTT 1.x completion condition.
                break;
            }
        }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (obj, meth, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    (obj, meth, e.whicharg, e.expected_, e.received_ );
            }
        catch( const std::exception& e)
        {
            throw parse_exception_fatal_semantic_error("While calling "+obj+"."+meth+": "+e.what());
        }
    }
    assert( ret.get() );
  }

  DataCallParser::~DataCallParser()
  {
    // if argparsers is not empty, then something went wrong during
    // the parsing ( someone threw an exception ), and we're
    // responsible for cleaning up the argparsers we created..
    while ( ! argparsers.empty() )
    {
      delete argparsers.top();
      argparsers.pop();
    }
  }

  ConstructorParser::ConstructorParser( ExpressionParser& p, CommonParser& cp)
      : commonparser(cp), expressionparser( p )
  {
    BOOST_SPIRIT_DEBUG_RULE( type_name );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    type_name =
        ( commonparser.type_name[ boost::bind( &ConstructorParser::seen_type_name, this, _1, _2 ) ] >> !arguments)[ boost::bind( &ConstructorParser::seen_constructor, this ) ];
  }

  ConstructorParser::~ConstructorParser()
  {
    // if argparsers is not empty, then something went wrong during
    // the parsing ( someone threw an exception ), and we're
    // responsible for cleaning up the argparsers we created..
    while ( ! argparsers.empty() )
    {
      delete argparsers.top();
      argparsers.pop();
    }
  }


  void ConstructorParser::seen_type_name( iter_t begin, iter_t end )
  {
      std::string name( begin, end );
      TypeInfo* type = Types()->type( name );
      if ( type == 0 )
          throw_(iter_t(), "\"" + name + "\" is an unknown type...");

      ArgumentsParser* argspar =
          new ArgumentsParser( expressionparser, 0, Service::shared_ptr(), name, "" );

      // keep hold of the argspar, we're still going to need it after
      // it's done its work..  ( in seen_constructor(), that is.. )
      argparsers.push( argspar );

      // set the arguments parser to the parser provided by the
      // ArgumentsParser we just created..
      arguments = argspar->parser();

  }

  void ConstructorParser::seen_constructor( void )
  {
    ArgumentsParser* argspar = argparsers.top();
    argparsers.pop();
    std::string obj = argspar->objectname();
    std::vector<DataSourceBase::shared_ptr> args = argspar->result();
    delete argspar;

    ret = TypeInfoRepository::Instance()->type( obj )->construct( args );

    if (!ret) {
        throw parse_exception_no_such_constructor( obj, args );
    }

  }

    /** @cond */
    static void abort_rule(const string& reason) {
        throw_(iter_t(), reason);
    }

    static error_status<> fail_rule(scanner_t const& scan, parser_error<std::string, iter_t>&e )
    {
        return error_status<>( error_status<>::fail );
    }
    /** @endcond */

  ExpressionParser::ExpressionParser( TaskContext* pc, ExecutionEngine* caller, CommonParser& cp )
      : mcmdcnd(0),
        datacallparser( *this, cp, pc, caller ),
        constrparser(*this, cp),
        commonparser( cp ),
        valueparser( pc, cp ),
        _invert_time(false),
        opreg( OperatorRepository::Instance() ),
        context(pc)
  {
    BOOST_SPIRIT_DEBUG_RULE( expression );
    BOOST_SPIRIT_DEBUG_RULE( unarynotexp );
    BOOST_SPIRIT_DEBUG_RULE( unaryminusexp );
    BOOST_SPIRIT_DEBUG_RULE( unaryplusexp );
    BOOST_SPIRIT_DEBUG_RULE( div_or_mul );
    BOOST_SPIRIT_DEBUG_RULE( modexp );
    BOOST_SPIRIT_DEBUG_RULE( plus_or_min );
    BOOST_SPIRIT_DEBUG_RULE( smallereqexp );
    BOOST_SPIRIT_DEBUG_RULE( smallerexp );
    BOOST_SPIRIT_DEBUG_RULE( greatereqexp );
    BOOST_SPIRIT_DEBUG_RULE( greaterexp );
    BOOST_SPIRIT_DEBUG_RULE( equalexp );
    BOOST_SPIRIT_DEBUG_RULE( notequalexp );
    BOOST_SPIRIT_DEBUG_RULE( orexp );
    BOOST_SPIRIT_DEBUG_RULE( andexp );
    BOOST_SPIRIT_DEBUG_RULE( ifthenelseexp );
    BOOST_SPIRIT_DEBUG_RULE( groupexp );
    BOOST_SPIRIT_DEBUG_RULE( dotexp );
    BOOST_SPIRIT_DEBUG_RULE( atomicexpression );
    BOOST_SPIRIT_DEBUG_RULE( time_expression );
    BOOST_SPIRIT_DEBUG_RULE( time_spec );
    BOOST_SPIRIT_DEBUG_RULE( indexexp );
    BOOST_SPIRIT_DEBUG_RULE( comma );
    BOOST_SPIRIT_DEBUG_RULE( close_brace );
    BOOST_SPIRIT_DEBUG_RULE( value_expression );
    BOOST_SPIRIT_DEBUG_RULE( call_expression );
    BOOST_SPIRIT_DEBUG_RULE( constructor_expression );

    comma = expect_comma( ch_p(',') );
    close_brace = expect_close( ch_p(')') );
    expression = assignexp;

    // We parse expressions without regard of the types.  First we
    // parse the expressions, then we worry about whether what the
    // user says is actually valid.  You can try to add up two
    // booleans, and the parser will parse it, but it will notice
    // you're writing bogus when it tries to pass it to an operator
    // structure from Operators.hpp

    // TODO: implement the ifthenelse operator ?
    assignexp = andexp >> *( ch_p( '=' ) >> eps_p(~ch_p( '=' ))  // prevent parsing first '=' of "=="
            >> assignexp)[ bind( &ExpressionParser::seen_assign, this)];
    andexp =
      orexp >> *( ( str_p( "&&" ) ) >> orexp[
                    boost::bind( &ExpressionParser::seen_binary, this, "&&" ) ] );
    orexp =
      notequalexp >> *( ( str_p( "||" ) ) >> notequalexp[
                        boost::bind( &ExpressionParser::seen_binary, this, "||" ) ] );
    notequalexp =
      equalexp >> *( "!=" >> equalexp[
                       boost::bind( &ExpressionParser::seen_binary, this, "!=" ) ] );
    equalexp =
         greatereqexp
      >> *( "==" >> greatereqexp[
              boost::bind( &ExpressionParser::seen_binary, this, "==" ) ] );
    greatereqexp =
         greaterexp
      >> *( ">=" >> greaterexp[
              boost::bind( &ExpressionParser::seen_binary, this, ">=" ) ] );
    greaterexp =
         smallereqexp
      >> *( '>' >> smallereqexp[
              boost::bind( &ExpressionParser::seen_binary, this, ">" ) ] );
    smallereqexp =
         smallerexp
      >> *( "<=" >> smallerexp[
              boost::bind( &ExpressionParser::seen_binary, this, "<=" ) ] );
    smallerexp =
      plus_or_min >> *( '<' >> plus_or_min[
                       boost::bind( &ExpressionParser::seen_binary, this, "<" ) ] );

    plus_or_min =
              modexp >> *( ('-' >> modexp[
                    boost::bind( &ExpressionParser::seen_binary, this, "-" ) ] )
               | ('+' >> modexp[
                    boost::bind( &ExpressionParser::seen_binary, this, "+" ) ] ) );

    modexp =
      div_or_mul >> *( '%' >> div_or_mul[
                     boost::bind( &ExpressionParser::seen_binary, this, "%" ) ] );
    div_or_mul =
      unaryplusexp >> *( ('/' >> unaryplusexp[
            boost::bind( &ExpressionParser::seen_binary, this, "/" ) ] )
       | ('*' >> unaryplusexp[
            boost::bind( &ExpressionParser::seen_binary, this, "*" ) ] ) );

    unaryplusexp =
        '+' >> unaryminusexp[
          boost::bind( &ExpressionParser::seen_unary, this, "+" ) ]
      | unaryminusexp;
    unaryminusexp =
        '-' >> unarynotexp[
          boost::bind( &ExpressionParser::seen_unary, this, "-" ) ]
      | unarynotexp;
    unarynotexp =
        ch_p('!') >> atomicexpression[
            boost::bind( &ExpressionParser::seen_unary, this, "!" ) ]
        | atomicexpression;

    // note the order is important: commonparser.identifier throws a
    // useful "cannot use x as identifier" error if it fails, so we
    // must first show all non-identifier rules.
    atomicexpression = (
        // A parenthesis group.
      groupexp
        // or a time expression
      | time_expression
        // or a constant or user-defined value..
      | value_expression
      | constructor_expression
      | call_expression
        // or an index or dot expression
        ) >> *( dotexp | indexexp);

    constructor_expression = my_guard( constrparser.parser()[ boost::bind(&ExpressionParser::seenconstructor, this)])[&fail_rule];

    // if it's value.keyword then pass it on to the call_expression.
    value_expression = my_guard( valueparser.parser() >> !('.' >> commonparser.keyword[boost::bind(&abort_rule,"Rule must be handled by datacallparser.")]))[ &fail_rule ]
                                                        [ bind( &ExpressionParser::seenvalue, this ) ];
    call_expression  = my_guard( datacallparser.parser() )[&fail_rule]
                                [bind( &ExpressionParser::seendatacall, this ) ];
    // take index of an atomicexpression
    indexexp =
        (ch_p('[') >> expression[bind(&ExpressionParser::seen_index, this)] >> expect_close( ch_p( ']') ) );

    dotexp =
        ( ch_p('.') >> commonparser.identifier[ boost::bind(&ExpressionParser::seen_dotmember, this, _1, _2)]);

    // needs no semantic action, its result is already on top of
    // the stack, where it should be..
    groupexp = '(' >> expression >> close_brace;

    // the day i find a clean way to temporarily disable 'eol' skipping, a lot of
    // grammar will look better...
    time_expression =
        (str_p("time")>>eps_p(~commonparser.identchar | eol_p | end_p ))[bind(&ExpressionParser::seentimeexpr, this)]
        |
        ( (eps_p[boost::lambda::var(commonparser.skipeol) = false] >> uint_p[ bind( &ExpressionParser::seentimespec, this, _1 ) ]
           >> (str_p( "s" ) | "ms" | "us" | "ns" )[boost::lambda::var(commonparser.skipeol) = true][boost::bind( &ExpressionParser::seentimeunit, this, _1, _2 ) ])
          | (eps_p[boost::lambda::var(commonparser.skipeol) = true] >> nothing_p) // eps_p succeeds always, then fail.
          )
          ; // enable skipeol.

//         >> expect_timespec( (( str_p( ">=" ) | ">" )
//                             |
//                             (str_p("<=") | "<")[bind( &ExpressionParser::inverttime, this)])
//         >> time_spec);

//     time_spec =
//         ( uint_p[ bind( &ExpressionParser::seentimespec, this, _1 ) ]
//         >>
//       ( str_p( "s" ) | "ms" | "us" | "ns" )[
//         bind( &ExpressionParser::seentimeunit, this, _1, _2 ) ] ) | expression[bind(&ExpressionParser::seentimeexpr, this)];

  }

    void ExpressionParser::inverttime()
    {
        _invert_time = true;
    }

    void ExpressionParser::seentimeexpr()
    {
        parsestack.push( new DataSourceTime() );

//         DataSourceBase::shared_ptr res = parsestack.top();
//         parsestack.pop();
//         DataSource<double>::shared_ptr dres = dynamic_cast<DataSource<double>*>( res.get() );
//         if ( !dres )
//             throw parse_exception_semantic_error("Expected time in seconds but expression is not a floating point number.");
//         DataSourceBase::shared_ptr dsb( new DataSourceCondition( new ConditionDSDuration( dres, _invert_time ) ) );
//         _invert_time = false;
//         parsestack.push( dsb );
    }

  void ExpressionParser::seentimeunit( iter_t begin, iter_t end)
  {
    // the string starting at begin, ending at end is either ms, us,
    // ns or s, so we only need to check the first letter...
      // Convert to seconds...
      TimeService::Seconds total = 0;
    switch( *begin )
    {
    case 's': total = TimeService::Seconds(tsecs);
        break;
    case 'm': total = tsecs / 1000.0;
        break;
    case 'u': total = tsecs / 1000000.0;
        break;
    case 'n': total = tsecs / 1000000000.0;
        break;
    default:
        std::string arg(begin, end);
        throw parse_exception_semantic_error("Expected time expression 's', 'ms', 'us' or 'ns' after integer value, got "+arg);
    }

    parsestack.push( new ConstantDataSource<double>( total ) );

//     DataSourceBase::shared_ptr dsb( new DataSourceCondition(
//       new ConditionDuration( total, _invert_time ) ) );
//     _invert_time = false;
//     parsestack.push( dsb );
  }

  void ExpressionParser::seentimespec( int n )
  {
    tsecs = n;
  }

  void ExpressionParser::seenvalue()
  {
    DataSourceBase::shared_ptr ds = valueparser.lastParsed();
    parsestack.push( ds );
  }

  void ExpressionParser::seendatacall()
  {
      DataSourceBase::shared_ptr n( datacallparser.getParseResult() );
      parsestack.push( n );
      mhandle = datacallparser.getParseHandle();
      mcmdcnd = datacallparser.getParseCmdResult();
  }

  void ExpressionParser::seenconstructor()
  {
      DataSourceBase::shared_ptr n( constrparser.getParseResult() );
      parsestack.push( n );
  }

  ExpressionParser::~ExpressionParser()
  {
      // if parsestack is not empty, then something went wrong, someone
      // threw an exception, so we clean up..
      while ( !parsestack.empty() )
          parsestack.pop();
  }

  rule_t& ExpressionParser::parser()
  {
    return expression;
  }

  DataSourceBase::shared_ptr ExpressionParser::getResult()
  {
    assert( !parsestack.empty() );
    return parsestack.top();
  }

  ConditionInterface*  ExpressionParser::getCmdResult()
  {
    assert( !parsestack.empty() );
    return mcmdcnd;
  }

  boost::shared_ptr<AttributeBase> ExpressionParser::getHandle()
  {
    assert( !parsestack.empty() );
    return mhandle;
  }

  void ExpressionParser::seen_unary( const std::string& op )
  {
    DataSourceBase::shared_ptr arg( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr ret =
        opreg->applyUnary( op, arg.get() );
    if ( ! ret )
        throw parse_exception_fatal_semantic_error( "Cannot apply unary operator \"" + op +
                                                    "\" to " + arg->getType() +"." );
    parsestack.push( ret );
  }

  void ExpressionParser::seen_dotmember( iter_t s, iter_t f )
  {
      std::string member(s,f);
      // inspirired on seen_unary
    DataSourceBase::shared_ptr arg( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr ret = arg->getMember(member);
    if ( ! ret )
      throw parse_exception_fatal_semantic_error( arg->getType() + " does not have member \"" + member +
                                            "\"." );
    parsestack.push( ret );
  }

  void ExpressionParser::seen_binary( const std::string& op )
  {
    DataSourceBase::shared_ptr arg1( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr arg2( parsestack.top() );
    parsestack.pop();

    // Arg2 is the first (!) argument, as it was pushed on the stack
    // first.
    DataSourceBase::shared_ptr ret =
      opreg->applyBinary( op, arg2.get(), arg1.get() );
    if ( ! ret )
      throw parse_exception_fatal_semantic_error( "Cannot apply binary operation "+ arg2->getType() +" " + op +
                                            " "+arg1->getType() +"." );
    parsestack.push( ret );
  }

  void ExpressionParser::seen_assign()
  {
    DataSourceBase::shared_ptr arg1( parsestack.top() );
    parsestack.pop(); // right hand side
    DataSourceBase::shared_ptr arg2( parsestack.top() );
    parsestack.pop(); // left hand side

    // hack to drop-in a new instance of SendHandle:
    if (arg2->getTypeName() == "SendHandle" && mhandle) {
//        cout << "Trying to replace SendHandle/..."<<endl;
        ConfigurationInterface::AttributeObjects attrs = context->attributes()->getValues();
        for( ConfigurationInterface::AttributeObjects::iterator it = attrs.begin(); it != attrs.end(); ++it) {
            if ( (*it)->getDataSource() == arg2 ) { // since the parsestack only saves the DSB, we need to do lookup by DSB and not by name :-(
//                cout << "Found !"<<endl;
                string name = (*it)->getName();
                context->attributes()->removeAttribute(name);
                // This aliasing may happen only once for each var SendHandle. Since 'arg1' is not assignable, a next assigning will fail.
                if ( dynamic_cast<ValueDataSource<SendStatus>*>(mhandle->getDataSource().get() ) ) {
                    // This goes quite far: we also wrap the SH DataSource in a collect such that evaluating it does not cause a reset()+send(), but merely returns the SendStatus:
                    context->attributes()->setValue( new SendHandleAlias( name,
                                                                          mhandle->getFactory()->produceCollect(std::vector<DataSourceBase::shared_ptr>(1,arg1), new ValueDataSource<bool>(false)),
                                                                          mhandle->getFactory() ) );
                    parsestack.push( arg1 ); // effectively aliases RHS to lhs. Don't use the SendHandleAlias since it would not do the reset()+send().
                    return;
                }
                AttributeBase* var = mhandle->clone();
                var->setName( name ); // fill in the final handle name.
                context->attributes()->setValue( var );
                arg2 = var->getDataSource(); // frees up dummy and puts real one in place.
                break;
            }
        }
    }

    DataSourceBase::shared_ptr ret;
    ActionInterface* act = 0;
    try {
        act = arg2->updateAction( arg1.get() );
    } catch(...) { // bad assignment
        throw parse_exception_fatal_semantic_error( "Incompatible types. Cannot assign: "+ arg2->getType() +" = " +
                " "+arg1->getType() +"." );
    }
    if (!act)
        throw parse_exception_fatal_semantic_error( "2:Cannot assign constant (or returned) variable of types: "+ arg2->getType() +" = " +
                " "+arg1->getType() );
    // only try this if not unknown_t.
    if (arg2->getTypeInfo()->getTypeName() != "unknown_t")
        ret = arg2->getTypeInfo()->buildActionAlias(act, arg2);
    if (!ret) { // no type info !
        ret = new DataSourceCommand( act ); // fall back into the old behavior of returning a boolean.
    }
    parsestack.push( ret );
  }

  void ExpressionParser::seen_index()
  {
    DataSourceBase::shared_ptr arg1( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr arg2( parsestack.top() );
    parsestack.pop();

    // Arg2 is the first (!) argument, as it was pushed on the stack
    // first.
    DataSourceBase::shared_ptr ret = arg2->getMember( arg1, 0 );
    if ( ! ret )
      throw parse_exception_fatal_semantic_error( "Illegal use of []: "+ arg2->getType() +"[ "
                                                +arg1->getType() +" ]." );
    parsestack.push( ret );
  }

  void ExpressionParser::dropResult()
  {
      mcmdcnd = 0;
    parsestack.pop();
  }
}
