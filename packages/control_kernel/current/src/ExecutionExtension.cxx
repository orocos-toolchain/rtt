#include "control_kernel/ExecutionExtension.hpp"

#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/DataSourceFactory.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/Parser.hpp>

#include <execution/TemplateCommandFactory.hpp>

#include <functional>
#include <boost/function.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_Execution;
    using namespace boost;

    ExecutionExtension::ExecutionExtension( KernelBaseFunction* _base )
        : detail::ExtensionInterface( "Execution" ), program(0), context(0),
          running_progr(false),count(0), base( _base ),
          interval("Interval", "The relative interval of executing a program node \
with respect to the Kernels period. Should be strictly positive ( > 0).", 1)
    {
    }

    ExecutionExtension::~ExecutionExtension()
    {
    }

    using std::cerr;
    using std::endl;

    TaskInterface* ExecutionExtension::getTask() const
    {
        return base->getTask();
    }
    
    void ExecutionExtension::setTask( TaskInterface* task )
    {
        base->setTask( task );
    }

    bool ExecutionExtension::initialize() 
    { 
        class DummyState : public StateInterface
        { void onEntry() {} void onExit() {} void handle() {} };

//         if ( !proc.startConfiguration() )
//             cerr << "Configuration of Processor failed !"<<endl;
        if ( !proc.loadStateContext( "Default", context == 0 ? new StateContext( new DummyState, new DummyState ) : context ) )
            cerr << "Failed to load Processor State Context !"<<endl;
//         if ( !proc.endConfiguration() )
//             cerr << "Could not end Processor Configuration !"<<endl;
        if ( program !=0 && !proc.loadProgram("Default", program) ) // pass ownership to the processor
            cerr << "Program present but could not be loaded in Processor !" <<endl;

        if ( !proc.startStateContext("Default") )
            cerr << "Processor could not start State Execution !"<<endl;
        return true;
    }

    void ExecutionExtension::startProgram()
    {
        proc.resetProgram("Default");
        running_progr = proc.startProgram("Default");
    }

    bool ExecutionExtension::isProgramRunning() const
    {
        return running_progr;
    }

    void ExecutionExtension::stopProgram()
    {
        proc.stopProgram("Default");
        running_progr  = false;
    }

    bool ExecutionExtension::loadProgram( std::istream& prog_stream )
    {
        initKernelCommands();
        Parser    parser;
        program = parser.parseProgram( prog_stream, &proc, this );
        if (program == 0) 
            return false;
        return true;
    }

    bool ExecutionExtension::loadStateContext( std::istream& state_stream )
    {
        initKernelCommands();
        Parser    parser;
        context = parser.parseStateContext( state_stream, &proc, this );
        if (context == 0) 
            return false;
        return true;
    }

    void ExecutionExtension::step() {
        if ( count % interval == 0 )
            {
                count = 0;
                proc.doStep();
            }
        ++count;
    }

    void ExecutionExtension::finalize() 
    {
        proc.stopProgram("Default");
    }

    CommandFactoryInterface* ExecutionExtension::createCommandFactory()
    {
        return 0;
    }

    DataSourceFactory* ExecutionExtension::createDataSourceFactory()
    {
        return 0;
    }

    void ExecutionExtension::initKernelCommands()
    {
        // I wish I could do this cleaner, but I can not do it
        // in the constructor (to early) and not in initialize (to late).
        static bool is_called = false;
        if ( is_called )
            return;
        is_called = true;

        // Add the commands / data of the kernel :
        commandfactory = createCommandFactory();
        if ( commandfactory )
            commandFactory().registerObject( "kernel", commandfactory );

        dataSourceFactory = createDataSourceFactory();
        if ( dataSourceFactory )
            dataFactory().registerObject( "kernel", dataSourceFactory );

        // Add the commands/ data of the EE:
        TemplateCommandFactory< ExecutionExtension  >* ret =
            newCommandFactory( this );
        ret->add( "startProgram", 
                  command
                  ( &ExecutionExtension::startProgram ,
                    &ExecutionExtension::isProgramRunning ,
                    "Start a program" ) );
        ret->add( "stopProgram", 
                  command
                  ( &ExecutionExtension::stopProgram ,
                    //bind(&ExecutionExtension::foo, _1, mem_fn(&ExecutionExtension::isProgramRunning), std::logical_not<bool>() ),
                    &ExecutionExtension::isProgramRunning ,
                    "Stop a program", true ) ); // true ==  invert the result.
        commandFactory().registerObject( "engine", ret );
    }

    ExecutionComponentInterface::ExecutionComponentInterface( const std::string& _name )
        : detail::ComponentAspectInterface<ExecutionExtension>( _name + std::string( "::Execution" ) ),
          name( _name ), master( 0 ),
          commandfactory( 0 ), dataSourceFactory( 0 )
    {
    }

    bool ExecutionComponentInterface::enableAspect( ExecutionExtension* ext )
    {
        master = ext;
        commandfactory = createCommandFactory();
        if ( commandfactory )
            master->commandFactory().registerObject( name, commandfactory );
        dataSourceFactory = createDataSourceFactory();
        if ( dataSourceFactory )
            master->dataFactory().registerObject( name, dataSourceFactory );
        return true;
    }

    bool ExecutionExtension::updateProperties( const PropertyBag& bag )
    {
        // The interval for executing the program, relative to the
        // kernel period. This property is optional.
        composeProperty(bag, interval);
        // check for validity.
        return interval > 0;
    }

    void ExecutionComponentInterface::disableAspect()
    {
        master->commandFactory().unregisterObject( name );
        master->dataFactory().unregisterObject( name );
    }

    ExecutionComponentInterface::~ExecutionComponentInterface()
    {
    }

    CommandFactoryInterface* ExecutionComponentInterface::createCommandFactory()
    {
        return 0;
    }

    DataSourceFactory* ExecutionComponentInterface::createDataSourceFactory()
    {
        return 0;
    }
}

