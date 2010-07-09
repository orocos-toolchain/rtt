/***************************************************************************
  tag: FMTC  do nov 2 13:06:11 CET 2006  CommandFunctors.hpp

                        CommandFunctors.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_COMMANDFUNCTORS_HPP
#define ORO_COMMANDFUNCTORS_HPP

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/type_traits/function_traits.hpp>

#include "../base/ActionInterface.hpp"
#include "ConditionInterface.hpp"

namespace RTT
{
    namespace scripting {

        /**
         * A functor with the base::ActionInterface, for the case where
         * the functor is a bool(void).
         */
        class CommandFunction
            :public base::ActionInterface
        {
        public:
            typedef boost::function<bool(void)> Function;

            Function com;

            /**
             *
             */
            CommandFunction(Function impl)
                : com(impl)
            {
            }

            virtual void readArguments() { }

            virtual bool execute() { return com(); }

            virtual CommandFunction* clone() const
            {
                return new CommandFunction( com );
            }
        };

        /**
         * A functor with the ConditionInterface, for the case where
         * the functor is a bool(void).
         */
        class ConditionFunction
            :public ConditionInterface
        {
        public:
            typedef boost::function<bool(void)> Function;

            Function con;
            bool minvert;

            /**
             *
             */
            ConditionFunction(Function impl, bool invert=false)
                : con(impl), minvert(invert)
            {
            }

            virtual bool evaluate() { return con() != minvert; }

            virtual ConditionFunction* clone() const
            {
                return new ConditionFunction( con, minvert );
            }
        };
    }
}

#endif
