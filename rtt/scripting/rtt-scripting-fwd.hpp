#ifndef ORO_RTT_scripting_FWD_HPP
#define ORO_RTT_scripting_FWD_HPP

namespace RTT {
    class Scripting;
    namespace scripting {
        class ArgumentsParser;
        class CallFunction;
        class CommandComposite;
        class CommandCounter;
        class CommandIllegal;
        class CommandNOP;
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
        class ConditionExpire;
        class ConditionFalse;
        class ConditionInterface;
        class ConditionInvert;
        class ConditionOnce;
        class ConditionParser;
        class ConditionTrue;
        class DataCallParser;
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
        class ProgramService;
        class PropertyParser;
        class ScriptingService;
        class SendHandleAlias;
        class StateDescription;
        class StateGraphParser;
        class StateInterface;
        class StateMachine;
        class StateMachineBuilder;
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
        template<typename T, typename compare_op>
        class ConditionCompare;
    }
    namespace detail {
        using namespace scripting;
    }
}
#endif
