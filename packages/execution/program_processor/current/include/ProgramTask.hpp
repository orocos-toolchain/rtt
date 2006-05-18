/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ProgramTask.hpp 

                        ProgramTask.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#ifndef PROGRAM_TASK_HPP
#define PROGRAM_TASK_HPP

#include "TaskContext.hpp"
#include "ProgramInterface.hpp"
#include <corelib/DataSources.hpp>

namespace ORO_Execution
{

    /**
     * @brief This class represents a program as a task in
     * the Orocos TaskContext system.
     */
    class ProgramTask
        : public TaskContext
    {
        ORO_CoreLib::ValueDataSource<ProgramInterfaceWPtr>::shared_ptr program;
    public:
        /**
         * By constructing this object, a program is added to a taskcontext
         * as a TaskContext, with its commands and methods.
         */
        ProgramTask( ProgramInterfacePtr, ExecutionEngine* ee = 0 );

        ~ProgramTask();

        /**
         * Returns the Program of this task.
         */
        ProgramInterfacePtr getProgram() const { return program->get().lock(); }

    };
}

#endif


