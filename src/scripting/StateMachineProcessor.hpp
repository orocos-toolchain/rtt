/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  Processor.hpp

                        Processor.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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

#ifndef ORO_STATE_MACHINE_PROCESSOR_HPP
#define ORO_STATE_MACHINE_PROCESSOR_HPP

#include "../base/RunnableInterface.hpp"
#include "StateMachine.hpp"
#include "ProgramExceptions.hpp"

#include <map>
#include <string>
#include <vector>
#include "../internal/List.hpp"

#include "../rtt-config.h"

namespace RTT
{
    /**
     * This class implements a real-time state machine executor.
     *
     * @par Changing the State Machine Processing Policy
     *
     * The default policy of the StateMachineProcessor is to call
     * StateMachine::execute() of all loaded state machines in each
     * step(). The parent state machines are executed before the children.
     *
     * If you want to change this policy, subclass the
     * StateMachineProcessor and override the virtual functions, such
     * as step().
     * @ingroup Processor
     */
    class RTT_API StateMachineProcessor
        : public RunnableInterface
    {
    public:
        /**
         * The status of a StateMachine.
         */
        typedef StateMachine::Status Status;

        /**
         * Constructs a new StateMachineProcessor
         */
        StateMachineProcessor();

        virtual ~StateMachineProcessor();

        /**
         * Load a new State Machine and all its children.
         * @throw program_load_exception if a state machine with the same name already exists.
         */
        bool loadStateMachine( StateMachinePtr sc );

        /**
         * Unload a deactivated State Machine and all its children.
         * @throw program_unload_exception if the state machine or one of its children is
         * still active.
         */
        bool unloadStateMachine( const std::string& name );

        /**
         * Delete a deactivated State Machine and all its children.
         * @throw program_unload_exception if the state machine or one of its children is
         * still active.
         * @deprecated by unloadStateMachine
         */
        bool deleteStateMachine( const std::string& name );

        /**
         * Stop and unload all programs from the StateMachineProcessor.
         */
        void clear();

        /**
         * Return the status of a StateMachine.
         */
        Status::StateMachineStatus getStateMachineStatus(const std::string& name) const;

        /**
         * Return the status of a StateMachine as a human readable string
         */
        std::string getStateMachineStatusStr(const std::string& name) const;

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        /**
         * Get a list of all loaded StateMachines and their children.
         */
        std::vector<std::string> getStateMachineList() const;

        /**
         * Get a pointer to a loaded StateMachine.
         */
        const StateMachinePtr getStateMachine(const std::string& name) const;

        /**
         * Get a pointer to a loaded StateMachine.
         */
        StateMachinePtr getStateMachine(const std::string& name);

    protected:
        void recursiveLoadStateMachine( StateMachinePtr sc );
        bool recursiveCheckLoadStateMachine( StateMachinePtr sc );
        void recursiveUnloadStateMachine( StateMachinePtr sc );
        bool recursiveCheckUnloadStateMachine( StateMachinePtr si );

        typedef List<StateMachinePtr> StateMap;
        StateMap*   states;

    };

}

#endif
