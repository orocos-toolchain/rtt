#ifndef KERNELSCRIPTABLEEXTENSION_HPP
#define KERNELSCRIPTABLEEXTENSION_HPP

#include "KernelInterfaces.hpp"

#include <execution/Processor.hpp>
#include <execution/SystemContext.hpp>
#include <execution/SystemState.hpp>
#include <execution/GlobalFactory.hpp>

namespace ORO_ControlKernel
{
    using ORO_CoreLib::PropertyBag;
    using ORO_Execution::CommandFactoryInterface;
    using ORO_Execution::DataSourceFactory;
    using ORO_Execution::ProgramGraph;
    using ORO_Execution::SystemContext;
    using ORO_Execution::SystemState;
    using ORO_Execution::Processor;
    using ORO_Execution::GlobalFactory;

    class ExecutionExtension;

    /**
     * @brief This is the default interface a kernel Component gets
     * when the kernel is extended with the ExecutionExtension.
     *
     * @see ExecutionExtension
     *
     */
    struct ExecutionComponentInterface
        : detail::ComponentAspectInterface<ExecutionExtension>
    {
        std::string name;
        ExecutionExtension* master;
    protected:
        CommandFactoryInterface* commandfactory;
        DataSourceFactory* dataSourceFactory;
    public:
        ExecutionComponentInterface( const std::string& _name );

        /**
         * The factory for creating the commands this Component
         * wants to export.
         */
        virtual CommandFactoryInterface* createCommandFactory();

        /**
         * The factory for creating the data this Component
         * wants to export.
         */
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
        ProgramGraph* program;
    public:
        typedef ExecutionComponentInterface CommonBase;

        ExecutionExtension( KernelBaseFunction* _base=0 );

        virtual ~ExecutionExtension();

        virtual bool initialize();

        /**
         * Set a ProgramGraph to be used the next time the kernel is started.
         *
         * @param p A stream containing the program script to be executed.
         */
        bool loadProgram( std::istream& prog_stream );

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

        virtual void step() ;

        virtual void finalize();

        virtual bool updateProperties( const PropertyBag& bag );

    private:
        Processor proc;
        int count;
        Property<int> interval;
    };
}

#endif
