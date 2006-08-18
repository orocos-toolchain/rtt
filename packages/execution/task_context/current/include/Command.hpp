#ifndef ORO_TASK_COMMAND_HPP
#define ORO_TASK_COMMAND_HPP

#include <string>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include "CommandBase.hpp"
#include "LocalCommand.hpp"
#include "UnMember.hpp"

namespace RTT
{
    /**
     * A Command is a function which can be sent to a
     * task for execution and be queried for its execution status.
     * The sending is also called the 'invocation' of the Command object.
     *
     * It is good practice that the
     * receiving task defines the commands it can execute, but
     * this class allows otherwise as well.
     *
     * @param CommandT The function signature of the command. For
     * example, bool( int, Frame, double)
     *
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

     // Invoke the command:
     Frame f = //...
     mycom(3, f, 9.0);
     @endcode
     *
     */
    template<class CommandT>
    class Command 
        : public detail::InvokerSignature<boost::function_traits<CommandT>::arity, CommandT, detail::CommandBase<CommandT>* >
    {
    protected:
        std::string mname;
        typedef detail::InvokerSignature<boost::function_traits<CommandT>::arity, CommandT, detail::CommandBase<CommandT>* > Base;

        BOOST_STATIC_ASSERT(( boost::is_same<typename boost::function_traits<CommandT>::result_type,bool>::value ));
    public:
        typedef CommandT Signature;

        /**
         * Create an empty, nameless command object. Use assignment to
         * initialise it.
         *
         */
        Command()
            : Base(0), mname()
        {}

        /**
         * Create an empty command object. Use assignment to
         * initialise it.
         *
         * @param name The name of the command.
         */
        Command(std::string name)
            : Base(0), mname(name)
        {}

        /**
         * Command objects are copy constructible.
         */
        Command(const Command& c)
            : Base( c.impl ? c.impl->cloneI() : 0), mname(c.mname)
        {
        }

        /**
         * Command objects may be assigned to each other.
         */
        Command& operator=(const Command& c)
        {
            if ( &c == this )
                return *this;
            this->mname = c.mname;
            delete this->impl;
            this->impl = 0;
            if ( c.impl )
                this->impl = c.impl->cloneI();
            return *this;
        }

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
            : Base( new detail::LocalCommand<CommandT>(com,con,t, invert)),
              mname(name)
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
            : Base( new detail::LocalCommand<CommandT>(com,con,t,commandp, invert)),
              mname(name)
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
            : Base( new detail::LocalCommand<CommandT>(com,con,commandp, invert)),
              mname(name)
        {
        }

        /** 
         * Construct a Command which uses a ready-made implementation.
         * If the implementation is of the wrong type, it is freed.
         * 
         * @param implementation An implementation which will be owned
         * by the command. If it is unusable, it is freed.
         */
        Command(DispatchInterface* implementation)
            : Base( dynamic_cast< detail::CommandBase<CommandT>* >(implementation) ),
              mname()
        {
            // If not convertible, delete the implementation.
            if ( !this->impl && implementation) {
                log(Error) << "Tried to assign Command from incompatible type."<< endlog();
                delete implementation; 
            }
        }

        /**
         * Cleanup the command.
         */
        ~Command()
        {
            delete this->impl;
        }

        /** 
         * A Command objects may be assigned to an implementation.
         * If the implementation is of the wrong type, it is freed.
         * 
         * @param implementation An implementation which will be owned
         * by the command. If it is unusable, it is freed.
         */
        Command& operator=(DispatchInterface* implementation)
        {
            if ( this->impl && this->impl == implementation)
                return *this;
            delete this->impl;
            this->impl = dynamic_cast< detail::CommandBase<CommandT>* >(implementation);
            if (this->impl == 0 && implementation) {
                log(Error) << "Tried to assign Command from incompatible type."<< endlog();
                delete implementation;
            }
            return *this;
        }

        /**
         * Returns true if the command is ready for invocation.
         * If ready() returns true, this command may be invoked,
         * otherwise, invocation will fail.
         */
        bool ready() const {
            return this->impl && this->impl->ready();
        }

        /**
         * Returns true if the command is executed and the completion
         * condition returns true as well.
         */
        bool done() const {
            if (!this->impl) return false;
            return this->impl->evaluate();
        }
     
        /**
         * After reset(), another attempt to dispatch
         * the command will be done when invoked.
         */
        void reset() {
            if (!this->impl) return;
            return this->impl->reset();
        }

        /**
         * Returns true if the command was sent to the CommandProcessor.
         * You can use this flag to check whether this command was invoked.
         */
        bool sent() const {
            if (!this->impl) return false;
            return this->impl->sent();
        }

        /**
         * Returns true if the command was accepted when sent to the CommandProcessor.
         * A Command is accepted when the CommandProcessor was running and its queue
         * was not full.
         */
        bool accepted() const {
            if (!this->impl) return false;
            return this->impl->accepted();
        }

        /**
         * Returns true if the command function was executed by the CommandProcessor.
         * When executed() is true, you can check if it was found valid().
         */
        bool executed() const {
            if (!this->impl) return false;
            return this->impl->executed();
        }

        /**
         * Returns true if the command was valid, i.e. the command function itself
         * was executed and returned true. 
         */
        bool valid() const {
            if (!this->impl) return false;
            return this->impl->valid();
        }

        /** 
         * Returns the name of this Command object.
         * 
         * @return the name.
         */
        const std::string& getName() const {
            return mname;
        }

        /** 
         * The Command class is just a wrapper around an implementation,
         * this function returns the implementation. Make a clone() if you
         * want to keep the pointer.
         * 
         * @return The implementation
         */
        detail::CommandBase<CommandT>* getCommandImpl() const {
            return this->impl;
        }

        /** 
         * Change the implementation of this Command, delete the old one.
         * 
         * @param new_impl The new implementation.
         */
        void setCommandImpl(detail::CommandBase<CommandT>* new_impl) const {
            delete this->impl;
            return this->impl = new_impl;
        }
    };

    /** 
     * Factory function to create a Command object which executes a member function
     * of an object. The object inherits from the TaskCore class and the command
     * is executed in the ExecutionEngine's CommandProcessor of that object.
     * 
     * @param name The name of the command.
     * @param command A pointer to a member function of \a object, which is executed as
     * the command function
     * @param condition A pointer to a member function of \a object, which is evaluated
     * as completion condition
     * @param object A pointer to an object which has \a command and \a condition as functions
     * and inherits from TaskCore.
     * @param invert Set to true to invert the result of \a condition.
     * 
     * @return A new Command object.
     */
    template<class ComF, class ConF, class Object>
    Command< typename detail::UnMember<ComF>::type > command(std::string name, ComF command, ConF condition, Object object, bool invert = false) {
        return Command<  typename detail::UnMember<ComF>::type >(name, command, condition, object, invert);
    }

    /** 
     * Factory function to create a Command object which executes a member function
     * of an object. A CommandProcessor is given in which the command is executed.
     * 
     * @param name The name of the command
     * @param command A pointer to a member function of \a object, which is executed as
     * the command function
     * @param condition A pointer to a member function of \a object, which is evaluated
     * as completion condition
     * @param object A pointer to an object which has \a command and \a condition as functions
     * @param cp The command processor which will execute the command.
     * @param invert Set to true to invert the result of \a condition.
     * 
     * @return A new Command object.
     */
    template<class ComF, class ConF, class Object>
    Command< typename detail::UnMember<ComF>::type > command(std::string name, ComF command, ConF condition, Object object, CommandProcessor* cp, bool invert = false) {
        return Command<  typename detail::UnMember<ComF>::type >(name, command, condition, object, cp, invert);
    }

    /** 
     * Factory function to create a Command object which executes a 'C' function.
     * 
     * @param name The name of the command object
     * @param command A pointer to a function, which is executed as
     * the command function
     * @param condition A pointer to a function, which is evaluated
     * as completion condition
     * @param cp The command processor which will execute the command.
     * @param invert Set to true to invert the result of \a condition.
     * 
     * @return A new Command object.
     */
    template<class ComF, class ConF>
    Command< typename detail::UnMember<ComF>::type > command(std::string name, ComF command, ConF condition, CommandProcessor* cp, bool invert = false) {
        return Command<  typename detail::UnMember<ComF>::type >(name, command, condition, cp, invert);
    }

}
#endif
