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
#include <execution/GlobalFactory.hpp>

#pragma interface

namespace ORO_ControlKernel
{
    using ORO_CoreLib::PropertyBag;
    using ORO_Execution::CommandFactoryInterface;
    using ORO_Execution::DataSourceFactoryInterface;
    using ORO_Execution::ProgramGraph;
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
    class ExecutionComponentInterface
        : public detail::ComponentAspectInterface<ExecutionExtension>
    {
        friend class ExecutionExtension;

    protected:
        std::string name;
        ExecutionExtension* master;

        using detail::ComponentAspectInterface<ExecutionExtension>::enableAspect;

        CommandFactoryInterface* commandfactory;
        DataSourceFactoryInterface* dataSourceFactory;

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

        bool enableAspect( ExecutionExtension* ext );

        void disableAspect();

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
        : public detail::ExtensionInterface,
        public GlobalFactory
    {
        ProgramGraph* program;

    protected:
        CommandFactoryInterface* commandfactory;
        DataSourceFactoryInterface* dataSourceFactory;

    public:
        typedef ExecutionComponentInterface CommonBase;

        ExecutionExtension( ControlKernelInterface* _base=0 );

        virtual ~ExecutionExtension();

        virtual bool initialize();

        /**
         * Set a ProgramGraph to be used the next time the kernel is started.
         *
         * @param prog_stream A stream containing a program.
         */
        bool loadProgram( std::istream& prog_stream );

        /**
         * Set a StateGraph to be used the next time the kernel is started.
         *
         * @param state_stream A stream containing the definitions of
         *     the statecontexts script to be executed.  All root
         *     contexts will be loaded.
         * @param name The name of the StateContext.
         */
        bool loadStateContexts( std::istream& state_stream );

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
         * Resets the execution of a stopped program to the first instruction.
         * @param name The name of the Program.
         */
        bool resetProgram(const std::string& name);

        /**
         * Start a previously loaded StateContext.
         * @param name The name of the StateContext.
         */
        bool startStateContext(const std::string& name);

        /**
         * Returns true if the program was successfully started.
         * @param name The name of the StateContext.
         */
        bool isStateContextRunning(const std::string& name) const;

        /**
         * Stops the execution of a StateContext and enters the Final_State.
         * @param name The name of the StateContext.
         */
        bool stopStateContext(const std::string& name);

        /**
         * Resets the execution of a StateContext and enters the Initial_State.
         * @param name The name of the StateContext.
         */
        bool resetStateContext(const std::string& name);

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

        bool steppedStateContext(const std::string& name);

        bool continuousStateContext(const std::string& name);


        virtual TaskInterface* getTask() const;

        virtual void setTask( TaskInterface* task );

        virtual void step() ;

        virtual void finalize();

        virtual bool updateProperties( const PropertyBag& bag );

        virtual CommandFactoryInterface* createCommandFactory();

        virtual DataSourceFactoryInterface* createDataSourceFactory();

    protected:
        void initKernelCommands();
        //bool foo( bool( ExecutionExtension::*cond)() const, bool (*adapter)(const bool&) ) const;
    private:
        bool true_gen() const { return true; }
        bool running_progr;

        Processor proc;
        int count;

        ControlKernelInterface* base;

        Property<int> interval;
    };
}

#endif
