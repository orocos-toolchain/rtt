
#include "execution/ProgramTask.hpp"
#include "execution/TaskAttribute.hpp"
#include "execution/TemplateFactories.hpp"
#include "execution/TaskContext.hpp"
#include "execution/FactoryExceptions.hpp"

namespace ORO_Execution
{

    ProgramTask::ProgramTask(ProgramInterfacePtr prog, ExecutionEngine* ee)
        : TaskContext( prog->getName(), ee ), program( new VariableDataSource<ProgramInterfaceWPtr>(prog) )
    {
        // Commands :
        TemplateCommandFactory< DataSource<ProgramInterfaceWPtr> >* fact =
            newCommandFactory( static_cast< DataSource<ProgramInterfaceWPtr>* >( program.get() ) );

        fact->add("start", command_ds(&ProgramInterface::start, &ProgramInterface::isRunning,"Start or continue this program.") );
        fact->add("pause", command_ds(&ProgramInterface::pause, &ProgramInterface::isPaused,"Pause this program.") );
        fact->add("step", command_ds(&ProgramInterface::step, &ProgramInterface::stepDone,"Step a paused program.") );
        fact->add("stop", command_ds(&ProgramInterface::stop, &ProgramInterface::isStopped,"Stop and reset this program.") );

        this->commandFactory.registerObject("this", fact);

        // DataSources:
        TemplateDataSourceFactory< DataSource<ProgramInterfaceWPtr> >* dfact =
            newDataSourceFactory( static_cast< DataSource<ProgramInterfaceWPtr>* >( program.get() ) );

        dfact->add("isRunning", data_ds( &ProgramInterface::isRunning, "Is this program being executed and not paused ?") );
        dfact->add("inError", data_ds(&ProgramInterface::inError,"Has this program executed an erroneous command ?") );
        dfact->add("isPaused", data_ds(&ProgramInterface::isPaused,"Is this program running but paused ?") );

        this->dataFactory.registerObject("this", dfact);
    }

    ProgramTask::~ProgramTask() {
    }
}

