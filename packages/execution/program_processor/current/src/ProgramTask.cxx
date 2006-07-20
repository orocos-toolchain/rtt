/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ProgramTask.cxx 

                        ProgramTask.cxx -  description
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
 
 

#include "execution/ProgramTask.hpp"
#include "execution/CommandDS.hpp"
#include "execution/Method.hpp"
#include "execution/TaskContext.hpp"
#include "execution/FactoryExceptions.hpp"
#include <corelib/DataSources.hpp>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    using namespace ORO_CoreLib::detail;

    ProgramTask::ProgramTask(ProgramInterfacePtr prog, ExecutionEngine* ee)
        : TaskContext( prog->getName(), ee ),
          program( new ValueDataSource<ProgramInterfaceWPtr>(prog) ) // was: VariableDataSource.
    {
        DataSource<ProgramInterfaceWPtr>* ptr = program.get();
        // Commands :
        commands()->addCommandDS( ptr,
                                  command_ds("start",&ProgramInterface::start, &ProgramInterface::isRunning,engine()->commands()),
                                  "Start or continue this program.");
        commands()->addCommandDS( ptr,
                                  command_ds("pause",&ProgramInterface::pause, &ProgramInterface::isPaused,engine()->commands()),
                                  "Pause this program.");
        commands()->addCommandDS( ptr,
                                  command_ds("step", &ProgramInterface::step, &ProgramInterface::stepDone,engine()->commands()),
                                  "Step a paused program.");
        commands()->addCommandDS( ptr,
                                  command_ds("stop", &ProgramInterface::stop, &ProgramInterface::isStopped,engine()->commands()),
                                  "Stop and reset this program.");

        // DataSources:

        methods()->addMethodDS( ptr,
                                method_ds("isRunning",&ProgramInterface::isRunning),
                                "Is this program being executed and not paused ?");
        methods()->addMethodDS( ptr,
                                method_ds("inError", &ProgramInterface::inError),
                                "Has this program executed an erroneous command ?");
        methods()->addMethodDS( ptr,
                                method_ds("isPaused", &ProgramInterface::isPaused),
                                "Is this program running but paused ?");
    }

    ProgramTask::~ProgramTask() {
    }
}

