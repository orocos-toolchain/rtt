/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:57 CEST 2004  CommandFunction.hpp 

                        CommandFunction.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
 
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
