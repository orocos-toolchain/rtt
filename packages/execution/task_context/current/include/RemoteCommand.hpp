#ifndef ORO_REMOTE_COMMAND_HPP
#define ORO_REMOTE_COMMAND_HPP

#include <boost/function.hpp>
#include <string>
#include "DispatchInterface.hpp"
#include "CommandProcessor.hpp"
#include "CommandFunctors.hpp"
#include "DataSourceStorage.hpp"
#include "Invoker.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A Command which is dispatched remotely to a CommandProcessor.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         *
         */
        template<class CommandT>
        class RemoteCommandImpl
            : public CommandBase<CommandT>,
              protected detail::DataSourceStorage<CommandT>
        {
        protected:
            CommandC mcom;
        public:
            /**
             * The defaults are reset by the constructor.
             */
            RemoteCommandImpl()
                : mcom()
            {}

            /** 
             * Call this operator if the RemoteCommand takes no arguments.
             * 
             * @return true if ready and succesfully sent.
             */
            bool operator()() {
                return mcom.execute();
            }

            template<class T1>
            bool operator()( T1 a1 ) {
                this->store( a1 );
                return mcom.execute();
            }

            template<class T1, class T2>
            bool operator()( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                return mcom.execute();
            }

            template<class T1, class T2, class T3>
            bool operator()( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                return mcom.execute();
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                return mcom.execute();
            }
        };


        /**
         * A Command which is dispatched remotely to a CommandProcessor.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         *
         */
        template<class CommandT>
        class RemoteCommand 
            : public Invoker<CommandT,RemoteCommandImpl<CommandT> >
        {
        public:
            typedef CommandT Signature;

            /** 
             * Create a RemoteCommand object which executes a remote command
             * 
             * @param name The name of this command.
             * @param com The OperationFactory for commands.
             */
            RemoteCommand(CommandFactory* of, std::string name)
            {
                // create the command.
                this->mcom = CommandC(of, name);
                // add the arguments to the command.
                this->initArgs( this->mcom );
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                return this->mcom.ready();
            }

            virtual bool dispatch() {
                return this->mcom.execute();
            }

            virtual bool execute() {
                return this->mcom.execute();
            }
        
            virtual bool evaluate() const {
                return this->mcom.evaluate();
            }
     
            virtual void reset() {
                return this->mcom.reset();
            }

            virtual bool sent() const {
                return this->mcom.sent();
            }

            virtual bool accepted() const {
                return this->mcom.accepted();
            }

            virtual bool executed() const {
                return this->mcom.executed();
            }

            virtual bool valid() const {
                return this->mcom.valid();
            }

            virtual ConditionInterface* createCondition() const
            {
                return new detail::ConditionFunctor<bool(void)>( boost::bind<bool>( boost::mem_fn(&RemoteCommand::evaluate), this) );
            }

            /** 
             * Creates a clone of this RemoteCommand object.
             * Use this method to get a new command object
             * which has its own state information.
             * 
             * @return 
             */
            virtual RemoteCommand* clone() const {
                return new RemoteCommand(*this);
            }
        };
    }
}
#endif
