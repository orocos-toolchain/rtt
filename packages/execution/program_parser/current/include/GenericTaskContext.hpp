/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  GenericTaskContext.hpp 

                        GenericTaskContext.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef GENERIC_TASK_CONTEXT_HPP
#define GENERIC_TASK_CONTEXT_HPP

#include <execution/TaskContext.hpp>

namespace ORO_Execution
{
    /**
     * This class specialises the TaskContext to offer some
     * default methods and commands which are for controlling
     * the task (being peroidic or not). Most methods are NOT
     * REALTIME, since they access the disk and allocate memory.
     * Thus you need to call them from the TaskBrowser or 
     * a non realtime task.
     *
     * All the commands and methods are virtual, to allow
     * detection of invocation in a subclass, and possibly
     * reject the invocation or tie other operations to it.
     */
    class GenericTaskContext
        :public TaskContext
    {
    protected:
        /**
         * Script assertion method. Use this to check
         * for consistency and bring the Program Script to
         * the error state if corruption is detected.
         * @return The value of \a must_be_true.
         */
        bool assertion( bool must_be_true );

    public:
        /**
         * The constructor sets up a task with \a name to have some
         * common methods.
         */
        GenericTaskContext(const std::string& name);

        virtual ~GenericTaskContext();

        /**
         * @name Script Methods
         * @{
         */
        /**
         * Start is a method which starts the execution engine's task.
         * It can not be a command because if the engine is not running,
         * it does not accept commands. Also, RunnableInterface::initialize()
         * is then called in the context of the caller.
         * You can override this method to do something else or in addition
         * to starting the Processor.
         * @return false if the engine was not assigned to a ActivityInterface
         * or if initialize() returned false or it was already started.
         */
        virtual bool start();
        
        /**
         * Stop is a method which stops the execution engine's task.
         * RunnableInterface::finalize()
         * is called in the context of the caller.
         * You can override this method to do something else or in addition
         * to stopping the engine.
         * @return false if the engine was not assigned to a ActivityInterface
         * or if it was not running.
         */
        virtual bool stop();
  
        /**
         * DataSource to inspect if this Task is running.
         * Defaults to this->getProcessor()->getActivity()
         * && this->getProcessor()->getActivity()->isRunning()
         */
        virtual bool isRunning() const;

        /**
         * Read this Task's properties from a file.
         * You can override this method to check for consistent
         * Properties.
         */
        virtual bool readProperties(const std::string& filename);

        /**
         * Write this Task's properties to a file.
         */
        virtual bool writeProperties(const std::string& filename);

        /**
         * Load an Orocos Program Script from disk.
         * You can override this method to accept or reject new
         * programs in certain cases.
         */
        virtual bool loadProgram(const std::string& filename);

        /**
         * Load an Orocos State Description from disk.
         * You can override this method to accept or reject new
         * State Machines in certain cases.
         */
        virtual bool loadStateMachine(const std::string& filename);

        /**
         * Unload an Orocos Program Script from disk.
         * You can override this method to accept or reject unloading
         * of programs in certain cases.
         */
        virtual bool unloadProgram(const std::string& progname);

        /**
         * Unload an Orocos State Description from disk.
         * You can override this method to accept or reject unloading
         * of State Machines in certain cases.
         */
        virtual bool unloadStateMachine(const std::string& instancename);

        /**
         * Invoke this method to instruct the TaskContext to execute
         * its ExecutionEngine and the update() method.
         * @retval false if this->engine()->getActivity()->trigger == false
         * @retval true otherwise.
         */
        virtual bool trigger();
        /**
         *@}
         */
    };
}
#endif
