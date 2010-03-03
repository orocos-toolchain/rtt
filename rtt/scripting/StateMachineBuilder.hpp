/***************************************************************************
  tag: Peter Soetens  Mon Jul 19 09:38:16 CEST 2004  StateMachineBuilder.hpp

                        StateMachineBuilder.hpp -  description
                           -------------------
    begin                : Mon July 19 2004
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

#include <map>
#include <string>
#include "ParsedStateMachine.hpp"

namespace RTT {

    namespace scripting {

    struct wrong_context_params_exception {
        std::string mwhat;
    public:
        wrong_context_params_exception( const std::string& what )
            : mwhat( what ) { };
        const std::string& what() {
            return mwhat;
        }
    };

    class StateMachineBuilder
    {
    public:
        StateMachineBuilder(
            ParsedStateMachinePtr templatecontext );

        ~StateMachineBuilder();

        /**
         * Throws wrong_context_params_exception if params is wrong...
         */
        ParsedStateMachinePtr build( bool instantiate );

        ParsedStateMachinePtr item() { return templatecontext; }
    private:
        ParsedStateMachinePtr templatecontext;
    };
}}
