/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandCounter.hpp

                        CommandCounter.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef COMMANDCOUNTER_HPP
#define COMMANDCOUNTER_HPP

#include "rtt/scripting/rtt-scripting-config.h"
#include "rtt/base/ActionInterface.hpp"

namespace RTT
{ namespace scripting {

    /**
     * @brief This command increments a counter when executed.
     *
     * @deprecated Remove this low-level class since it's nowhere used.
     */
    class RTT_SCRIPTING_API CommandCounter
        : public base::ActionInterface
    {

    public:

        CommandCounter();

        virtual ~CommandCounter();

        virtual void readArguments();

        virtual bool execute();

        /**
         * set number of times executed to zero
         */
        virtual void reset();

        /**
         * return the number of times this command has been
         * executed since the last reset
         */
        virtual int getCounter();

        virtual base::ActionInterface* clone() const
        {
            return new CommandCounter( *this );
        }

    private:
        int counter;
    };

}}

#endif

