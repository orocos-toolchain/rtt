/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  StatementProcessor.hpp

                        StatementProcessor.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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


#ifndef ORO_EXECUTION_STATEMENT_PROCESSOR_HPP
#define ORO_EXECUTION_STATEMENT_PROCESSOR_HPP

#include <string>
#include "../base/DispatchInterface.hpp"
#include "../rtt-config.h"
#include "../rtt-fwd.hpp"

namespace RTT
{ namespace scripting {

    /**
     * This class parses and executes a single scripting
     * statement.
     */
    class RTT_API StatementProcessor
    {
        class D;
        D* d;
    public:
        StatementProcessor(TaskContext* tc);

        ~StatementProcessor();
        /**
         * Execute a script statement.
         * @retval -1 a parse error occured.
         * @retval 0 statement executed
         * @return a positive number if it is a command, which
         * is the ticket number of the command.
         */
        int execute(const std::string& code);
    };


}}


#endif
