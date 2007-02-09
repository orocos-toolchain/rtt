/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineTask.hpp 

                        StateMachineTask.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#ifndef STATEMACHINE_TASK_HPP
#define STATEMACHINE_TASK_HPP

#include "../TaskContext.hpp"
#include "../StateMachine.hpp"

namespace RTT
{
    class StateMachineTask;
    typedef boost::shared_ptr<StateMachineTask> StateMachineTaskPtr;

    /**
     * @brief This class represents a stateMachine as a task in
     * the Orocos TaskContext system.
     */
    class StateMachineTask
        : public TaskContext
    {
        //boost::weak_ptr<StateMachine> sm;
        // used when state machines are copied.
        // functions have a similar mechanism
        ValueDataSource< StateMachineWPtr >::shared_ptr _this; 
        //VariableDataSource<StateMachineTask*> _this; 
        bool true_gen() const;

        void createCommandFactory();
        void createMethodFactory();

    public:
        StateMachineTask* copy(StateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate );

        /**
         * By constructing this object, a stateMachine is added to a taskcontext
         * as a TaskContext, with its commands and methods.
         */
        StateMachineTask(StateMachinePtr statemachine, ExecutionEngine* ee=0);

        ~StateMachineTask();

        /**
         * Returns the StateMachine of this task.
         */
        StateMachinePtr getStateMachine() const { return _this->get().lock(); }

    };
}

#endif


/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineTask.hpp 

                        StateMachineTask.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#ifndef STATEMACHINE_TASK_HPP
#define STATEMACHINE_TASK_HPP

#include "../TaskContext.hpp"
#include "../StateMachine.hpp"

namespace RTT
{
    class StateMachineTask;
    typedef boost::shared_ptr<StateMachineTask> StateMachineTaskPtr;

    /**
     * @brief This class represents a stateMachine as a task in
     * the Orocos TaskContext system.
     */
    class StateMachineTask
        : public TaskContext
    {
        //boost::weak_ptr<StateMachine> sm;
        // used when state machines are copied.
        // functions have a similar mechanism
        ValueDataSource< StateMachineWPtr >::shared_ptr _this; 
        //VariableDataSource<StateMachineTask*> _this; 
        bool true_gen() const;

        void createCommandFactory();
        void createMethodFactory();

    public:
        StateMachineTask* copy(StateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate );

        /**
         * By constructing this object, a stateMachine is added to a taskcontext
         * as a TaskContext, with its commands and methods.
         */
        StateMachineTask(StateMachinePtr statemachine, ExecutionEngine* ee=0);

        ~StateMachineTask();

        /**
         * Returns the StateMachine of this task.
         */
        StateMachinePtr getStateMachine() const { return _this->get().lock(); }

    };
}

#endif


