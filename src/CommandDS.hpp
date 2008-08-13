/***************************************************************************
  tag: FMTC  do nov 2 13:06:12 CET 2006  CommandDS.hpp

                        CommandDS.hpp -  description
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


#ifndef ORO_TASK_COMMAND_DS_HPP
#define ORO_TASK_COMMAND_DS_HPP

#include <boost/function.hpp>
#include <string>
#include "CommandProcessor.hpp"
#include "BindStorage.hpp"
#include "UnMember.hpp"

namespace RTT
{
    /**
     * A Command storage container.
     */
    template<class CommandT>
    class CommandDS
        : private detail::BindStorage<CommandT>
    {
    protected:
        std::string mname;

        CommandProcessor* mcp;

        bool minvert;
    public:
        typedef CommandT Signature;

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
        CommandDS(std::string name, CommandF com, ConditionF con, CommandProcessor* commandp, bool invert = false)
            : mname(name),
              mcp( commandp ),
              minvert(invert)
        {
            this->setup(boost::function<CommandT>(com), boost::function<CommandT>(con) );
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

    template<class ComF, class ConF>
    CommandDS<typename detail::ArgMember<ComF>::type > command_ds(std::string name, ComF command, ConF condition, CommandProcessor* cp, bool invert = false) {
        return CommandDS<typename detail::ArgMember<ComF>::type >(name, command, condition, cp, invert);
    }


}
#endif
