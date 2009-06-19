/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:57 CEST 2004  CommandFunction.hpp

                        CommandFunctor.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#ifndef ORO_COMMANDFUNCTOR_HPP
#define ORO_COMMANDFUNCTOR_HPP

#include "CommandInterface.hpp"
#include <boost/function.hpp>

namespace RTT
{
    /**
     * @brief A Command which can be bound to a function using boost::bind.
     *
     * See the boost::bind documentation (www.boost.org) for more examples.
     * \a execute() will always return true. To have it return the boolean
     * result of the bound function, a boost::function<bool(void)> object
     * must be given. See the Commandfunctor specialisation class.
     *
     * Usage :
     * @verbatim
     * void myFun(void);
     * bool myFun( ClassA* a );
     * ClassA* _class;
     *
     * // Watch the 'newCommandFunctor' _function_ :
     * CommandInterface* c,d;
     * c = newCommandFunctor( boost::bind( &myFun ) );
     * c->execute(); // calls myFun();
     * d = newCommandFunctor( boost::bind(&myFun2, _class) );
     * d->execute(); // calls myFun2( _class );
     * @endverbatim
     */
    template< class F >
    class CommandFunctor
        : public CommandInterface
    {
        F f;
    public:
        typedef F Function;

        /**
         * Create a command calling a function.
         */
        CommandFunctor(const Function& func) : f(func) {}

        virtual ~CommandFunctor() {}

        virtual void readArguments() {}

        virtual bool execute() { f(); return true; }

        virtual CommandInterface* clone() const
        {
            return new CommandFunctor<F>(f);
        }
    };

    /**
     * Specialisation of CommandFunctor.
     *
     * When a boost::function< bool(void)>  object is given as template
     * parameter, the boolean result will be returned in execute.
     * @verbatim
     * bool myFun( ClassA* a );
     * ClassA* _class;
     *
     * // Watch the 'newCommandFunctor' _function_ :
     * CommandInterface* d;
     * d = newCommandFunctor( boost::function<bool(void)>(boost::bind(&myFun2, _class)) );
     * bool result = d->execute(); // returns myFun2( _class );
     * @endverbatim
     *
     */
    template<>
    class CommandFunctor< boost::function< bool(void) > >
        : public CommandInterface
    {
        boost::function<bool(void)> f;
    public:
        typedef boost::function<bool(void)> Function;

        /**
         * Create a command calling a function.
         */
        CommandFunctor(const Function& func) : f(func) {}

        virtual ~CommandFunctor() {}

        virtual void readArguments() {}

        virtual bool execute() { return f(); }

        virtual CommandInterface* clone() const
        {
            return new CommandFunctor<Function>(f);
        }
    };

    /**
     * @brief Helper function to create a new CommandFunctor.
     * @param f The function to wrap in a CommandFunctor.
     * @return a new CommandInterface object which will call the
     * function \a f when execute()'ed.
     */
    template<class F>
    CommandInterface* newCommandFunctor(const F& f )
    {
        return new CommandFunctor<F>(f);
    }
}

#endif
