/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ExecutionExtension.hpp

                        ExecutionExtension.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef KERNELSCRIPTABLEEXTENSION_HPP
#define KERNELSCRIPTABLEEXTENSION_HPP

#include "KernelInterfaces.hpp"
#include "ComponentInterfaces.hpp"
#include <corelib/Property.hpp>

#include <execution/Processor.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/TaskContext.hpp>

#include <map>
#include <string>

#pragma interface

namespace ORO_Execution {
    class ParsedStateMachine;
}

namespace ORO_ControlKernel
{
    using ORO_CoreLib::PropertyBag;
    using ORO_Execution::CommandFactoryInterface;
    using ORO_Execution::DataSourceFactoryInterface;
    using ORO_Execution::MethodFactoryInterface;
    using ORO_Execution::ProgramGraph;
    using ORO_Execution::Processor;
    using ORO_Execution::TaskContext;
    using ORO_Execution::ParsedStateMachine;
    using ORO_Execution::ProgramInterface;
    class ExecutionExtension;

    /**
     * @brief This is the default interface a kernel Component gets
     * when the kernel is extended with the ExecutionExtension.
     *
     * @see ExecutionExtension
     *
     */
    class ExecutionComponentInterface
        : public detail::ComponentFacetInterface<ExecutionExtension>
    {
        friend class ExecutionExtension;

    protected:
        std::string name;
        ExecutionExtension* master;

        using detail::ComponentFacetInterface<ExecutionExtension>::enableFacet;

        CommandFactoryInterface* _commandfactory;
        MethodFactoryInterface* _methodfactory;
        DataSourceFactoryInterface* _datasourcefactory;

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
        virtual DataSourceFactoryInterface* createDataSourceFactory();

        /**
         * The factory for creating the methods this Component
         * wants to export.
         */
        virtual MethodFactoryInterface* createMethodFactory();

        bool enableFacet( ExecutionExtension* ext );

        void disableFacet();

        ~ExecutionComponentInterface();
    };

    /**
     * @brief The ExecutionExtension provides the parser with access to
     * useful commands and data inside components.
     *
     * A component should
     * assign a useful command factory to commandFactory in its
     * constructor, and a useful DataSource factory to
     * dataSourceFactory.  Check out TemplateCommandFactory and
     * TemplateDataSourceFactory for a low-trouble way to build the
     * factories.
     * Furthermore, It contains a Processor which can then execute
     * the generated commands at runtime.
     */
    class ExecutionExtension
        : public detail::ExtensionInterface
    {
        ProgramGraph* program;

    public:
        typedef ExecutionComponentInterface CommonBase;

        ExecutionExtension( ControlKernelInterface* _base );

        virtual ~ExecutionExtension();

        /**
         * Set Programs to be used in the kernel.
         *
         * @param filename A file containing one or more programs.
         * @throw file_parse_exception ( defined in
         * <execution/parse_exception.hpp> ) if parsing fails
         * @throw program_load_exception ( defined in
         * this header ) if loading fails...
         */
        bool loadProgram( const std::string&  filename, const std::string& file = "" );

        /**
         * Load the StateMachines from the given file.
         *
         * @param filename The name of the StateMachine.
         * @throw file_parse_exception ( defined in
         * <execution/parse_exception.hpp> ) if parsing fails
         * @throw program_load_exception ( defined in
         * this header ) if loading fails...
         */
        void loadStateMachine( const std::string& filename, const std::string& file = "" );

        const ParsedStateMachine* getStateMachine(const std::string& name);
        const ProgramInterface* getProgram(const std::string& name);

        /**
         * Delete the given StateMachine...
         */
        bool deleteStateMachine( const std::string& name );

        /**
         * Start a previously loaded Program.
         * @param name The name of the Program.
         */
        bool startProgram(const std::string& name);

        /**
         * Returns true if the program was successfully started.
         * @param name The name of the Program.
         */
        bool isProgramRunning(const std::string& name) const;

        /**
         * Stops the execution of a program.
         * @param name The name of the Program.
         */
        bool stopProgram(const std::string& name);

        /**
         * Pauses the execution of a running program.
         * @param name The name of the Program.
         */
        bool pauseProgram(const std::string& name);

        /**
         * Steps a single instruction of a paused program.
         * @param name The name of the Program.
         */
        bool stepProgram(const std::string& name);

        /**
         * Activate a previously loaded StateMachine.
         * @param name The name of the StateMachine.
         */
        bool activateStateMachine(const std::string& name);

        /**
         * Deactivate a stopped StateMachine.
         * @param name The name of the StateMachine.
         */
        bool deactivateStateMachine(const std::string& name);

        /**
         * Start a previously activated StateMachine.
         * @param name The name of the StateMachine.
         */
        bool startStateMachine(const std::string& name);

        /**
         * Pause a previously activated StateMachine.
         * @param name The name of the StateMachine.
         */
        bool pauseStateMachine(const std::string& name);

        /**
         * Returns true if the program was successfully started.
         * @param name The name of the StateMachine.
         */
        bool isStateMachineRunning(const std::string& name) const;

        /**
         * Stops the execution of a StateMachine and enters the Final_State.
         * @param name The name of the StateMachine.
         */
        bool stopStateMachine(const std::string& name);

        /**
         * Resets the execution of a StateMachine and enters the Initial_State.
         * @param name The name of the StateMachine.
         */
        bool resetStateMachine(const std::string& name);

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

        bool steppedStateMachine(const std::string& name);

        bool continuousStateMachine(const std::string& name);

        virtual bool updateProperties( const PropertyBag& bag );

        virtual CommandFactoryInterface* createCommandFactory();

        virtual DataSourceFactoryInterface* createDataSourceFactory();

        TaskContext* getTaskContext()
        {
            return &tc;
        }

    protected:
        virtual bool initialize();

        virtual void step() ;

        virtual void finalize();

        void initKernelCommands();
    private:
        bool true_gen() const {return true;}
        bool running_progr;

        int count;

        Processor proc;

        ControlKernelInterface* base;

        Property<int> interval;

        std::map<std::string,ParsedStateMachine*> parsed_states;

        TaskContext tc;
    };

}

#endif
