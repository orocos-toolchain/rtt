/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:57 CEST 2004  CommandFunction.hpp 

                        CommandFunction.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef ORO_COMMANDFunction_HPP
#define ORO_COMMANDFunction_HPP

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
     * d->execute(); // calls myFun2( _class );
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

        virtual CommandInterface* clone() const
        {
            return new CommandFunction(f);
        }
    };
}

#endif
