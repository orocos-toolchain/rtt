#ifndef ORO_RTT_scripting_FWD_HPP
#define ORO_RTT_scripting_FWD_HPP

namespace RTT {
    namespace scripting {
        class ArgumentsParser;
        class CommandComposite;
        class CommandCounter;
        class CommandDispatch;
        class CommandExecFunction;
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
        class ParserExecutionAccess;
        class ParserScriptingAccess;
        class PeerParser;
        class ProgramGraphParser;
        class ProgramLoader;
        class ProgramProcessor;
        class ProgramTask;
        class PropertyParser;
        class ScriptingAccess;
        class StateDescription;
        class StateGraphParser;
        class StateInterface;
        class StateMachine;
        class StateMachineBuilder;
        class StateMachineProcessor;
        class StateMachineTask;
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
