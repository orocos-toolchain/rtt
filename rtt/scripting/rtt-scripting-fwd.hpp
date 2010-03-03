/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  rtt-scripting-fwd.hpp

                        rtt-scripting-fwd.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_RTT_scripting_FWD_HPP
#define ORO_RTT_scripting_FWD_HPP

namespace RTT {
    namespace scripting {
        class ArgumentsParser;
        class AsynchCommandDecorator;
        class CallFunction;
        class CommandComposite;
        class CommandCounter;
        class CommandDispatch;
        class CommandIllegal;
        class CommandNOP;
        class CommandParser;
        class CommandString;
        class CommonParser;
        class ConditionBinaryCompositeAND;
        class ConditionBinaryCompositeOR;
        class ConditionBool;
        class ConditionBoolDataSource;
        class ConditionBoolProperty;
        class ConditionCache;
        class ConditionCompositeNOT;
        class ConditionDSDuration;
        class ConditionDuration;
        class ConditionExecFunction;
        class ConditionExpire;
        class ConditionFalse;
        class ConditionInvert;
        class ConditionOnce;
        class ConditionParser;
        class ConditionTrue;
        class DataCallParser;
        class DataSourceCommand;
        class DataSourceCondition;
        class DataSourceTime;
        class EdgeCondition;
        class EvalCommand;
        class EvalCommandResult;
        class ExpressionParser;
        class FunctionFactory;
        class FunctionGraph;
        class FunctionGraphBuilder;
        class ParsedStateMachine;
        class Parser;
        class PeerParser;
        class ProgramGraphParser;
        class ProgramInterface;
        class ProgramLoader;
        class ProgramProcessor;
        class ProgramService;
        class PropertyParser;
        class ScriptingService;
        class StateDescription;
        class StateGraphParser;
        class StateInterface;
        class StateMachine;
        class StateMachineBuilder;
        class StateMachineProcessor;
        class StateMachineService;
        class StatementProcessor;
        class TryCommand;
        class TryCommandResult;
        class ValueChangeParser;
        class ValueParser;
        class VertexNode;
        struct CommandBinary;
        struct CommandDataSource;
        struct CommandDataSourceBool;
        struct Dispatcher;
        template<class CommandT>
        class CommandDS;
        template<typename FunctionT>
        class FunctorDS0;
        template<typename FunctionT>
        class FunctorDS1;
        template<typename T, typename compare_op>
        class ConditionCompare;
    }
    namespace detail {
        using namespace scripting;
    }
}
#endif
