#include "control_kernel/ExecutionExtension.hpp"

#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/DataSourceFactory.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/Parser.hpp>

namespace ORO_ControlKernel
{
    using ORO_Execution::Parser;

    ExecutionExtension::ExecutionExtension( KernelBaseFunction* _base )
        : detail::ExtensionInterface( "Execution" ), program(0), count(0),
          interval("Interval", "The relative interval of executing a program node \
with respect to the Kernels period. Should be strictly positive ( > 0).", 1)
    {
    }

    ExecutionExtension::~ExecutionExtension()
    {
    }

    bool ExecutionExtension::initialize() 
    { 
        return proc.startConfiguration() &&
            proc.loadSystemContext( new SystemContext(  new SystemState ) ) &&
            proc.endConfiguration() &&
            proc.loadProgram(program) && // pass ownership to the processor
            proc.startExecution();
    }

    bool ExecutionExtension::loadProgram( std::istream& prog_stream )
    {
        Parser    parser;
        program = parser.parseProgram( prog_stream, &proc, this );
        if (program == 0) 
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
        proc.stopExecution();
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

