#ifndef KERNELSCRIPTABLEEXTENSION_HPP
#define KERNELSCRIPTABLEEXTENSION_HPP

#include "KernelInterfaces.hpp"

#include <execution/Processor.hpp>
#include <execution/SystemContext.hpp>
#include <execution/SystemState.hpp>
#include <execution/Program.hpp>
#include <execution/Parser.hpp>
#include <execution/GlobalFactory.hpp>
#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/DataSourceFactory.hpp>

namespace ORO_ControlKernel
{
    using ORO_CoreLib::PropertyBag;
    using ORO_Execution::CommandFactoryInterface;
    using ORO_Execution::DataSourceFactory;
    using ORO_Execution::Program;
    using ORO_Execution::Parser;
    using ORO_Execution::SystemContext;
    using ORO_Execution::SystemState;
    using ORO_Execution::Processor;
    using ORO_Execution::GlobalFactory;

    class ExecutionExtension;

    struct ExecutionComponentInterface
        : detail::ComponentAspectInterface<ExecutionExtension>
    {
        std::string name;
        ExecutionExtension* master;
    protected:
        CommandFactoryInterface* commandfactory;
        DataSourceFactory* dataSourceFactory;
    public:
        ExecutionComponentInterface( const std::string& _name )
            : detail::ComponentAspectInterface<ExecutionExtension>( _name + std::string( "::Execution" ) ),
              name( _name ), master( 0 ),
              commandfactory( 0 ), dataSourceFactory( 0 )
        {
        }

        virtual CommandFactoryInterface* createCommandFactory();
        virtual DataSourceFactory* createDataSourceFactory();

        bool enableAspect( ExecutionExtension* ext );
        void disableAspect();
        ~ExecutionComponentInterface();
    };

    /**
     * The ExecutionExtension: provides the parser with access to
     * useful commands and data inside components.  A component should
     * assign a useful command factory to commandFactory in its
     * constructor, and a useful DataSource factory to
     * dataSourceFactory.  Check out TemplateCommandFactory and
     * TemplateDataSourceFactory for a low-trouble way to build the
     * factories.
     * Furthermore, It contains a Processor which can then execute
     * the generated commands at runtime.
     */
    class ExecutionExtension
        : public detail::ExtensionInterface,
        public GlobalFactory
    {
        Program* program;
    public:
        typedef ExecutionComponentInterface CommonBase;

        ExecutionExtension( KernelBaseFunction* _base=0 )
            : detail::ExtensionInterface( "Execution" ), program(0)
        {
        }

        virtual ~ExecutionExtension()
        {
        }

        bool initialize() 
        { 
            return proc.startConfiguration() &&
                proc.loadSystemContext( new SystemContext(  new SystemState ) ) &&
                proc.endConfiguration() &&
                proc.loadProgram(program) && // pass ownership to the processor
                proc.startExecution();
        }

        /**
         * Set a Program to be used the next time the kernel is started.
         *
         * @param p A stream containing the program script to be executed.
         */
        bool loadProgram( std::istream& prog_stream )
        {
            program = parser.parseProgram( prog_stream, &proc, this );
            if (program == 0) 
                return false;
            return true;
        }

        /**
         * The Processor is needed during program construction,
         * so that the program can instruct the Processor if needed.
         *
         * @return The Program Processor of this extension.
         */
        Processor* getProcessor()
        {
            return &proc;
        }

        void step() { proc.doStep(); }

        void finalize() 
        {
            proc.stopExecution();
        }

        bool updateProperties( const PropertyBag& bag );

    private:
        Processor proc;
        Parser    parser;
    };
}

#endif
