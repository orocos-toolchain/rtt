 
#ifndef COMMANDFunction_HPP
#define COMMANDFunction_HPP

#include "CommandInterface.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief A Command which can be bound to a function using boost::bind.
     *
     * See the boost::bind documentation (www.boost.org) for more examples.
     *
     * @verbatim
     * Usage :
     * void myFun(void);
     * bool myFun( ClassA* a );
     * ClassA* _class;
     *
     * CommandInterface* c,d;
     * c = new CommandFunction( boost::bind( &myFun ) );
     * c->execute(); // calls myFun();
     * d = new CommandFunction( boost::bind(&myFun2, _class) );
     * d->exeucte(); // calls myFun2( _class );
     * @endverbatim
     */
    template< class F >
    class CommandFunction
        : public CommandInterface
    {
        F f;
    public:
        typedef F Function;

        /**
         * Create a command calling a function.
         */
        CommandFunction(Function& func) : f(func) {}

        virtual ~CommandFunction() {}

        virtual void execute() { f(); }

        virtual std::string toString() 
        {
            return std::string("CommandFunction");
        }

        virtual CommandInterface* clone() const
        {
            return new CommandFunction(f);
        }
    };
}

#endif
