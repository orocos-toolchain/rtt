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

#include <execution/TaskContext.hpp>

#include <map>
#include <string>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_Execution {
    class ParsedStateMachine;
}

namespace ORO_ControlKernel
{
    using ORO_CoreLib::PropertyBag;
    using ORO_Execution::AttributeRepository;
    using ORO_Execution::CommandFactoryInterface;
    using ORO_Execution::DataSourceFactoryInterface;
    using ORO_Execution::MethodFactoryInterface;
    using ORO_Execution::TaskContext;
    using ORO_Execution::StateMachine;
    using ORO_Execution::ProgramInterface;
    using ORO_Execution::StateMachinePtr;
    using ORO_Execution::ProgramInterfacePtr;
    class ExecutionExtension;

    /**
     * @brief This is the default interface a kernel Component gets
     * when the kernel is extended with the ExecutionExtension.
     *
     * A component should
     * assign a useful command factory to commandFactory in its
     * constructor, and a useful DataSource factory to
     * dataSourceFactory.  Check out TemplateCommandFactory and
     * TemplateDataSourceFactory for a low-trouble way to build the
     * factories.
     * @see ExecutionExtension
     * @see ProgramLoader for loading program/state machine scripts into
     * the Component's or Kernel's TaskContext.
     * @ingroup compinterf
     *
     */
    class ExecutionComponentInterface
        : public detail::ComponentFacetInterface<ExecutionExtension>
    {
        friend class ExecutionExtension;

    public:
        /**
         * Get the Component's TaskContext.
         * @return The exclusive TaskContext of this Component.
         */
        TaskContext* getComponentContext();
    protected:
        /**
         * Get the Kernel's TaskContext.  
         * @return null if component
         * not loaded or ExecutionExtension not present, the kernel's
         * TaskContext otherwise.
         */
        TaskContext* getKernelContext();

    protected:
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

        ~ExecutionComponentInterface();

        /**
         * The name of this component.
         * @deprecated, use getComponentContext()->getName().
         */
        std::string name;

        ExecutionExtension* master;

        TaskContext componentTask;

        using detail::ComponentFacetInterface<ExecutionExtension>::enableFacet;

        bool enableFacet( ExecutionExtension* ext );

        void disableFacet();
    };

    /**
     * @brief The ExecutionExtension provides the parser with access to
     * useful commands and data inside components.
     *
     * Furthermore, It contains a Processor which can then execute
     * the generated commands at runtime.
     * @see ExecutionComponentInterface
     * @ingroup kernelint
     */
    class ExecutionExtension
        : public detail::ExtensionInterface
    {
    public:
        typedef ExecutionComponentInterface CommonBase;

        ExecutionExtension( ControlKernelInterface* _base );

        virtual ~ExecutionExtension();

        /**
         * Set Programs to be used in the kernel.
         *
         * @param filename A file containing one or more programs.
         */
        bool loadProgram( const std::string&  filename, const std::string& file = "" );

        /**
         * Load the StateMachines from the given file.
         *
         * @param filename A file containing one or more StateMachines.
         */
        bool loadStateMachine( const std::string& filename, const std::string& file = "" );

        StateMachinePtr getStateMachine(const std::string& name);
        ProgramInterfacePtr getProgram(const std::string& name);

        /**
         * Unload the given StateMachine...
         */
        bool unloadStateMachine( const std::string& name );

        /**
         * Unload the given Program...
         */
        bool unloadProgram( const std::string& name );

        virtual bool updateProperties( const PropertyBag& bag );

        virtual bool exportProperties( AttributeRepository& bag );

        virtual void finishConstruction();

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

        Property<int> interval;

        TaskContext tc;

        bool initcommands;
    };

}

#endif
