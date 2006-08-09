#ifndef ORO_LOCAL_COMMAND_HPP
#define ORO_LOCAL_COMMAND_HPP

#include <boost/function.hpp>
#include <string>
#include "DispatchInterface.hpp"
#include "CommandProcessor.hpp"
#include "CommandFunctors.hpp"
#include "BindStorage.hpp"
#include "Invoker.hpp"
#include "CommandBase.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A Command which is dispatched locally to a CommandProcessor.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         *
         */
        template<class CommandT>
        class LocalCommandImpl
            : public CommandBase<CommandT>,
              protected detail::BindStorage<CommandT>
        {
        protected:
            CommandProcessor* mcp;

            bool minvoked, maccept, mvalid, mexec, minvert;
        public:
            /**
             * The defaults are reset by the constructor.
             */
            LocalCommandImpl()
                : mcp(0),
                  minvoked(false), maccept(false),
                  mvalid(false), mexec(false), minvert(false)
            {}

            CommandProcessor* getCommandProcessor() const { return mcp; }

            bool isInverted() const { return minvert; }

            /** 
             * Call this operator if the LocalCommand takes no arguments.
             * 
             * @return true if ready and succesfully queued.
             */
            bool invoke() {
                if (!mcp ||(minvoked && !this->evaluate()) ) // if invoked and not ready.
                    return false;
                this->reset();
                minvoked = true;
                return maccept = mcp->process( this );
            }

            template<class T1>
            bool invoke( T1 a1 ) {
                if (!mcp ||(minvoked && !this->evaluate()) ) // if invoked and not ready.
                    return false;
                this->reset();
                // bind types from Storage<Function>
                this->store( a1 );
                minvoked = true;
                return maccept = mcp->process( this );
            }

            template<class T1, class T2>
            bool invoke( T1 a1, T2 a2 ) {
                if (!mcp ||(minvoked && !this->evaluate()) ) // if invoked and not ready.
                    return false;
                this->reset();
                // bind types from Storage<Function>
                this->store( a1, a2 );
                minvoked = true;
                return maccept = mcp->process( this );
            }

            template<class T1, class T2, class T3>
            bool invoke( T1 a1, T2 a2, T3 a3 ) {
                if (!mcp ||(minvoked && !this->evaluate()) ) // if invoked and not ready.
                    return false;
                this->reset();
                // bind types from Storage<Function>
                this->store( a1, a2, a3 );
                minvoked = true;
                return maccept = mcp->process( this );
            }

            template<class T1, class T2, class T3, class T4>
            bool invoke( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                if (!mcp ||(minvoked && !this->evaluate()) ) // if invoked and not ready.
                    return false;
                this->reset();
                // bind types from Storage<Function>
                this->store( a1, a2, a3, a4 );
                minvoked = true;
                return maccept = mcp->process( this );
            }
        };


        /**
         * A Command which is dispatched locally to a CommandProcessor.
         * @param CommandT The function signature of the command. For example,
         * bool( int, Frame, double)
         *
         */
        template<class CommandT>
        class LocalCommand 
            : public Invoker<CommandT,LocalCommandImpl<CommandT> >
        {
        public:
            typedef CommandT Signature;

            /** 
             * Create a LocalCommand object which executes a member function of a class that
             * inherits from a TaskContext.
             * 
             * @param name The name of this command.
             * @param com A pointer to the member function to execute when the command is invoked.
             * @param con A pointer to the member function that evaluates if the command is done.
             * @param t A pointer to an object of this class, which will receive and process the command.
             * @param invert Invert the result of \a con when evaluating the completion of the command.
             * 
             */
            template<class CommandF, class ConditionF, class ObjectT>
            LocalCommand(CommandF com, ConditionF con, ObjectT t, bool invert = false)
            {
                this->setup(com,con,t);
                this->minvert = invert;
                this->mcp = t->engine()->commands();
            }

            /** 
             * Create a LocalCommand object which executes a member function of a class that
             * is \b not a TaskContext.
             * 
             * 
             * @param name The name of this command.
             * @param com A pointer to the member function to execute when the command is invoked.
             * @param con A pointer to the member function that evaluates if the command is done.
             * @param t A pointer to an object of the class which has \a com and \a con.
             * @param commandp The CommandProcessor which will execute this LocalCommand.
             * @param invert Invert the result of \a con when evaluating the completion of the command.
             * 
             */
            template<class CommandF, class ConditionF, class ObjectT>
            LocalCommand(CommandF com, ConditionF con, ObjectT t, CommandProcessor* commandp, bool invert = false)
            {
                this->setup(com,con,t);
                this->minvert = invert;
                this->mcp = commandp;
            }

            /** 
             * Create a LocalCommand object which executes a plain 'C' function.
             * 
             * @param name The name of this command.
             * @param com A pointer to the 'C' function to execute when the command is invoked.
             * @param con A pointer to the 'C' function that evaluates if the command is done.
             * @param commandp The CommandProcessor which will execute this LocalCommand.
             * @param invert Invert the result of \a con when evaluating the completion of the command.
             */
            template<class CommandF, class ConditionF>
            LocalCommand(CommandF com, ConditionF con, CommandProcessor* commandp, bool invert = false)
            {
                this->setup(com,con);
                this->minvert = invert;
                this->mcp = commandp;
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                return ( !this->minvoked || this->evaluate() );
            }

            virtual bool dispatch() {
                if (this->minvoked && !this->evaluate() ) // if invoked and not ready.
                    return false;
                this->reset();
                this->maccept = this->mcp->process( this );
                this->minvoked = true;
                return this->maccept;
            }

            virtual bool execute() {
                this->mvalid = this->exec();
                this->mexec = true;
                return this->mvalid;
            }
        
            virtual bool evaluate() const {
                if (this->mexec && this->mvalid )
                    return this->check() != this->minvert;
                return false;
            }
     
            virtual void reset() {
                this->minvoked = (false);
                this->maccept = (false);
                this->mvalid = (false); 
                this->mexec = (false);
            }

            virtual bool sent() const {
                return this->minvoked;
            }

            virtual bool accepted() const {
                return this->maccept;
            }

            virtual bool executed() const {
                return this->mexec;
            }

            virtual bool valid() const {
                return this->mvalid;
            }

            virtual ConditionInterface* createCondition() const
            {
                // LocalCommands are not used by the Parser, so this method is actually
                // not used within Orocos.
                return new detail::ConditionFunctor<bool(void)>( boost::bind<bool>( boost::mem_fn(&LocalCommand::evaluate), this) );
            }

            /** 
             * Creates a clone of this LocalCommand object.
             * Use this method to get a new command object
             * which has its own state information.
             * 
             * @return 
             */
            virtual DispatchInterface* clone() const {
                return new LocalCommand(*this);
            }

            virtual CommandBase<CommandT>* cloneI() const {
                return new LocalCommand(*this);
            }

            boost::function<CommandT> getCommandFunction() const {
                return this->command();
            }

            boost::function<CommandT> getConditionFunction() const {
                return this->condition();
            }

        };
    }
}
#endif
