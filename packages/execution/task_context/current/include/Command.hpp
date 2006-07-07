#ifndef ORO_TASK_COMMAND_HPP
#define ORO_TASK_COMMAND_HPP

#include <boost/function.hpp>
#include <string>
#include "DispatchInterface.hpp"
#include "CommandProcessor.hpp"
#include "CommandFunctors.hpp"
#include "BindStorage.hpp"

namespace ORO_Execution
{
    /**
     * A Command which is dispatched locally to a CommandProcessor.
     * @param CommandT The function signature of the command. For example,
     * bool( int, Frame, double)
     @code
     class X : public TaskContext
     {
     public:
       // ....
       bool command_1(int i, Frame f, double d) {
         // ....
       }
       bool condition_1(int i, Frame f, double d) {
         // ....
       }
     };
     X x;
     Command<bool(int, Frame, double)> mycom("Command1",&X::command_1,&X::condition_1, &x);

     Frame f = //...
     mycom(3, f, 9.0);   // executes the command.
     @endcode
     *
     */
    template<class CommandT>
    class Command 
        : public DispatchInterface,
          private detail::BindStorage<CommandT>
    {
    protected:
        std::string mname;

        CommandProcessor* mcp;

        bool minvoked, maccept, mvalid, mexec, minvert;
    public:
        typedef CommandT Signature;

        /** 
         * Create a Command object which executes a member function of a class that
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
        Command(std::string name, CommandF com, ConditionF con, ObjectT t, bool invert = false)
            : detail::BindStorage<CommandT>( com, con, t),
              mname(name),
              mcp( t->engine()->commands() ),
              minvoked(false), maccept(false),
              mvalid(false), mexec(false), minvert(invert)
        {
        }

        /** 
         * Create a Command object which executes a member function of a class that
         * is \b not a TaskContext.
         * 
         * 
         * @param name The name of this command.
         * @param com A pointer to the member function to execute when the command is invoked.
         * @param con A pointer to the member function that evaluates if the command is done.
         * @param t A pointer to an object of the class which has \a com and \a con.
         * @param commandp The CommandProcessor which will execute this Command.
         * @param invert Invert the result of \a con when evaluating the completion of the command.
         * 
         */
        template<class CommandF, class ConditionF, class ObjectT>
        Command(std::string name, CommandF com, ConditionF con, ObjectT t, CommandProcessor* commandp, bool invert = false)
            : detail::BindStorage<CommandT>( com, con, t),
              mname(name),
              mcp( commandp ),
              minvoked(false), maccept(false),
              mvalid(false), mexec(false), minvert(invert)
        {
        }

        /** 
         * Create a Command object which executes a plain 'C' function.
         * 
         * @param name The name of this command.
         * @param com A pointer to the 'C' function to execute when the command is invoked.
         * @param con A pointer to the 'C' function that evaluates if the command is done.
         * @param commandp The CommandProcessor which will execute this Command.
         * @param invert Invert the result of \a con when evaluating the completion of the command.
         */
        template<class CommandF, class ConditionF>
        Command(std::string name, CommandF com, ConditionF con, CommandProcessor* commandp, bool invert = false)
            : detail::BindStorage<CommandT>( com, con),
              mname(name),
              mcp( commandp ),
              minvoked(false), maccept(false),
              mvalid(false), mexec(false), minvert(invert)
        {
        }

        /** 
         * Call this operator if the Command takes no arguments.
         * 
         * @return true if ready and succesfully queued.
         */
        bool operator()() {
            if (minvoked && !evaluate() ) // if invoked and not ready.
                return false;
            this->reset();
            minvoked = true;
            return maccept = mcp->process( this );
        }

        template<class T>
        bool operator()( T a1 ) {
            if (minvoked && !evaluate() ) // if invoked and not ready.
                return false;
            this->reset();
            // bind types from Storage<Function>
            this->invoke = boost::bind<bool>( this->comm, a1 );
            this->check  = boost::bind<bool>( this->cond,  a1 );
            minvoked = true;
            return maccept = mcp->process( this );
        }

        template<class T1, class T2>
        bool operator()( T1 a1, T2 a2 ) {
            if (minvoked && !evaluate() ) // if invoked and not ready.
                return false;
            this->reset();
            // bind types from Storage<Function>
            this->invoke = boost::bind<bool>( this->comm, a1, a2 );
            this->check = boost::bind<bool>( this->cond, a1, a2 );
            minvoked = true;
            return maccept = mcp->process( this );
        }

        template<class T1, class T2, class T3>
        bool operator()( T1 a1, T2 a2, T3 a3 ) {
            if (minvoked && !evaluate() ) // if invoked and not ready.
                return false;
            this->reset();
            // bind types from Storage<Function>
            this->invoke = boost::bind<bool>( this->comm, a1, a2, a3 );
            this->check = boost::bind<bool>( this->cond, a1, a2, a3 );
            minvoked = true;
            return maccept = mcp->process( this );
        }

        template<class T1, class T2, class T3, class T4>
        bool operator()( T1 a1, T2 a2, T3 a3, T4 a4 ) {
            if (minvoked && !evaluate() ) // if invoked and not ready.
                return false;
            this->reset();
            // bind types from Storage<Function>
            this->invoke = boost::bind<bool>( this->comm, a1, a2, a3, a4 );
            this->check = boost::bind<bool>( this->cond, a1, a2, a3, a4 );
            minvoked = true;
            return maccept = mcp->process( this );
        }


        virtual void readArguments() {}

        virtual bool dispatch() {
            if (minvoked && !evaluate() ) // if invoked and not ready.
                return false;
            this->reset();
            minvoked = true;
            return maccept = mcp->process( this );
        }

        virtual bool execute() {
            mexec = true;
            return mvalid = this->invoke();
        }
        
        virtual bool evaluate() const {
            if (mexec && mvalid )
                return this->check() != minvert;
            return false;
        }
     
        virtual void reset() {
            minvoked = (false);
            maccept = (false);
            mvalid = (false); 
            mexec = (false);
        }

        virtual bool sent() const {
            return minvoked;
        }

        virtual bool accepted() const {
            return maccept;
        }

        virtual bool executed() const {
            return mexec;
        }

        virtual bool valid() const {
            return mvalid;
        }

        virtual ORO_CoreLib::ConditionInterface* createCondition() const
        {
            return new detail::ConditionFunctor<bool(void)>( this->check, minvert );
        }

        /** 
         * Creates a clone of this Command object.
         * Use this method to get a new command object
         * which has its own state information.
         * 
         * @return 
         */
        virtual Command* clone() const {
            return new Command(*this);
        }

        boost::function<CommandT> getCommandFunction() const {
            return this->command();
        }

        boost::function<CommandT> getConditionFunction() const {
            return this->condition();
        }

        /** 
         * Returns true if the condition is inverted.
         * 
         * @return true if inverted
         */
        bool isInverted() const {
            return minvert;
        }

        /** 
         * Returns a pointer to the CommandProcessor which will
         * process this command.
         * 
         * @return the pointer.
         */
        CommandProcessor* getCommandProcessor() const {
            return mcp;
        }

        /** 
         * Returns the name of this Command object.
         * 
         * @return the name.
         */
        const std::string& getName() const {
            return mname;
        }
        
    };

}
#endif
