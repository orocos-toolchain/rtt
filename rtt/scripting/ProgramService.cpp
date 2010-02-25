/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ProgramService.cpp

                        ProgramService.cpp -  description
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



#include "ProgramService.hpp"

#include "../Method.hpp"
#include "../interface/FactoryExceptions.hpp"
#include "../ExecutionEngine.hpp"
#include "../internal/DataSources.hpp"

namespace RTT
{

    using namespace detail;

    ProgramService::ProgramService(FunctionGraphPtr prog, TaskContext* tc)
        : ServiceProvider( prog->getName(), tc),
          program( new ValueDataSource<ProgramInterfacePtr>(prog) ),
          function(prog)
    {
        this->doc("Orocos Program Script");

        // We need a weak pointer here in order to be able to unload programs that
        // reference self. The only way we can use weak_ptr with Method/Operation is by putting it in the data source
        // of the first argument. We can not 'boost::bind' to a weak pointer, only to a shared_ptr.
        DataSource<ProgramInterfacePtr>* ptr = program.get();
        // Methods :
        addOperationDS("start", &ProgramInterface::start,ptr).doc("Start or continue this program.");
        addOperationDS("pause", &ProgramInterface::pause,ptr).doc("Pause this program.");
        addOperationDS("step", &ProgramInterface::step,ptr).doc("Step a paused program.");
        addOperationDS("stop", &ProgramInterface::stop,ptr).doc("Stop and reset this program.");

        // DataSources:

        addOperationDS("isRunning", &ProgramInterface::isRunning,ptr).doc("Is this program being executed and not paused ?");
        addOperationDS("inError", &ProgramInterface::inError,ptr).doc("Has this program executed an erroneous method ?");
        addOperationDS("isPaused", &ProgramInterface::isPaused,ptr).doc("Is this program running but paused ?");
    }

    ProgramService::~ProgramService() {
        // When the this ServiceProvider is deleted, make sure the program does not reference us.
        FunctionGraphPtr prog = function;
        if ( prog ) {
            prog->setProgramService( ProgramServicePtr() );
        }
    }
}
