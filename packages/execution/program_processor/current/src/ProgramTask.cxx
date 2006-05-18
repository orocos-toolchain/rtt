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
#include "execution/TemplateFactories.hpp"
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
        // Commands :
        TemplateCommandFactory< DataSource<ProgramInterfaceWPtr> >* fact =
            newCommandFactory( static_cast< DataSource<ProgramInterfaceWPtr>* >( program.get() ) );

        fact->add("start", command_ds(&ProgramInterface::start, &ProgramInterface::isRunning,"Start or continue this program.") );
        fact->add("pause", command_ds(&ProgramInterface::pause, &ProgramInterface::isPaused,"Pause this program.") );
        fact->add("step", command_ds(&ProgramInterface::step, &ProgramInterface::stepDone,"Step a paused program.") );
        fact->add("stop", command_ds(&ProgramInterface::stop, &ProgramInterface::isStopped,"Stop and reset this program.") );

        this->commands()->registerObject("this", fact);

        // DataSources:
        TemplateMethodFactory< DataSource<ProgramInterfaceWPtr> >* dfact =
            newMethodFactory( static_cast< DataSource<ProgramInterfaceWPtr>* >( program.get() ) );

        dfact->add("isRunning", data_ds( &ProgramInterface::isRunning, "Is this program being executed and not paused ?") );
        dfact->add("inError", data_ds(&ProgramInterface::inError,"Has this program executed an erroneous command ?") );
        dfact->add("isPaused", data_ds(&ProgramInterface::isPaused,"Is this program running but paused ?") );

        this->methods()->registerObject("this", dfact);
    }

    ProgramTask::~ProgramTask() {
    }
}

