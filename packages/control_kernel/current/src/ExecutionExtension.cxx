/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:38 CEST 2004  ExecutionExtension.cxx

                        ExecutionExtension.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#pragma implementation
#include "control_kernel/ExecutionExtension.hpp"

#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/GlobalDataSourceFactory.hpp>
#include <execution/DataSourceFactoryInterface.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/Parser.hpp>
#include <execution/ParsedStateContext.hpp>

#include <corelib/PropertyComposition.hpp>

#include "execution/TemplateDataSourceFactory.hpp"
#include <execution/TemplateCommandFactory.hpp>

#include <functional>
#include <boost/function.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_Execution;
    using namespace boost;

    ExecutionExtension::ExecutionExtension( ControlKernelInterface* _base )
        : detail::ExtensionInterface( _base, "Execution" ), program(0),
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

    bool ExecutionExtension::initialize()
    {
        initKernelCommands();
        if ( !proc.startStateContext("Default") )
            cerr << "Warning : Processor could not start \"Default\" StateContext."<<endl;
        return true;
    }

    bool ExecutionExtension::startProgram(const std::string& name)
    {
        proc.resetProgram(name);
        return proc.startProgram(name);
    }

    bool ExecutionExtension::isProgramRunning(const std::string& name) const
    {
        return proc.isProgramRunning(name);
    }

    bool ExecutionExtension::stopProgram(const std::string& name)
    {
        return proc.stopProgram(name);
    }

    bool ExecutionExtension::resetProgram(const std::string& name)
    {
        return proc.resetProgram(name);
    }

    bool ExecutionExtension::loadProgram( std::istream& prog_stream )
    {
        initKernelCommands();
        Parser    parser;
        program = parser.parseProgram( prog_stream, &proc, this );
        if (program == 0)
            return false;
        return proc.loadProgram( program );
    }

    std::pair<bool,std::string> ExecutionExtension::loadStateContexts( std::istream& state_stream, const std::string& filename )
    {
        initKernelCommands();
        Parser    parser;
        typedef std::vector<ParsedStateContext*> contexts_t;
        std::ostringstream errorstream;
        contexts_t contexts = parser.parseStateContext( state_stream, filename, &proc, this, errorstream );
        std::string error = errorstream.str();
        if ( contexts.empty() )
        {
            if ( error.empty() )
                error = "No StateContexts instantiated in file \"" + filename + "\".";
            return make_pair( false, error );
        }
        contexts_t::iterator i;
        for ( i = contexts.begin(); i != contexts.end(); ++i )
        {
            if ( !( *i )->registerWithProcessor( getProcessor() ) )
                break;
        }
        if ( i != contexts.end() )
        {
            std::string error(
                "Could not register StateContext \"" + ( *i )->getName() +
                "\" with the processor.  Probably the name is already used." );
            // something went wrong while registering *i -> unregister
            // the previous contexts.
            for ( contexts_t::iterator j = contexts.begin(); j != i; ++j )
                ( *j )->unregisterFromProcessor( getProcessor () );
            return make_pair( false, error );
        }
        return make_pair( true, std::string() );
    }

    bool ExecutionExtension::startStateContext(const std::string& name)
    {
        proc.resetStateContext(name);
        return proc.startStateContext(name);
    }

    bool ExecutionExtension::deleteStateContext( const std::string& name )
    {
        proc.stopStateContext( name );
        return proc.deleteStateContext( name );
    }

    bool ExecutionExtension::stopStateContext(const std::string& name)
    {
        return proc.stopStateContext(name);
    }

    bool ExecutionExtension::steppedStateContext(const std::string& name)
    {
        return proc.steppedStateContext(name);
    }

    bool ExecutionExtension::continuousStateContext(const std::string& name)
    {
        return proc.continuousStateContext(name);
    }

    bool ExecutionExtension::isStateContextRunning(const std::string& name) const
    {
        return proc.isStateContextRunning(name);
    }

    bool ExecutionExtension::resetStateContext(const std::string& name)
    {
        return proc.resetStateContext(name);
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
        proc.stopStateContext("Default");
    }

    void ExecutionExtension::initKernelCommands()
    {
        // I wish I could do this cleaner, but I can not do it
        // in the constructor (to early) and not in initialize (to late).
        static bool is_called = false;
        if ( is_called )
            return;
        is_called = true;

        std::vector<detail::ExtensionInterface*>::const_iterator it = base->getExtensions().begin();
        while ( it != base->getExtensions().end() )
            {
                // Add the commands / data of the kernel :
                commandfactory = (*it)->createCommandFactory();
                if ( commandfactory )
                    commandFactory().registerObject( (*it)->getName(), commandfactory );

                dataSourceFactory = (*it)->createDataSourceFactory();
                if ( dataSourceFactory )
                    dataFactory().registerObject( (*it)->getName(), dataSourceFactory );
                ++it;
            }
    }

    DataSourceFactoryInterface* ExecutionExtension::createDataSourceFactory()
    {
        // Add the data of the EE:
        TemplateDataSourceFactory< ExecutionExtension >* dat =
            newDataSourceFactory( this );

        dat->add( "isProgramRunning", data( &ExecutionExtension::isProgramRunning,
                                            "Is a program running ?", "Name", "The Name of the Loaded Program" ) );
        dat->add( "isStateContextRunning", data( &ExecutionExtension::isStateContextRunning,
                                            "Is a state context running ?", "Name", "The Name of the Loaded StateContext" ) );
        return dat;
    }

    CommandFactoryInterface* ExecutionExtension::createCommandFactory()
    {
        // Add the commands of the EE:
        TemplateCommandFactory< ExecutionExtension  >* ret =
            newCommandFactory( this );
        ret->add( "startProgram",
                  command
                  ( &ExecutionExtension::startProgram ,
                    &ExecutionExtension::true_gen ,
                    "Start a program", "Name", "The Name of the Loaded Program" ) );
        ret->add( "stopProgram",
                  command
                  ( &ExecutionExtension::stopProgram ,
                    //bind(&ExecutionExtension::foo, _1, mem_fn(&ExecutionExtension::isProgramRunning), std::logical_not<bool>() ),
                    &ExecutionExtension::true_gen ,
                    "Stop a program", "Name", "The Name of the Started Program" ) ); // true ==  invert the result.
        ret->add( "resetProgram",
                  command
                  ( &ExecutionExtension::resetProgram ,
                    &ExecutionExtension::true_gen ,
                    "Reset a program", "Name", "The Name of the Stopped Program" ) );
        ret->add( "startStateContext",
                  command
                  ( &ExecutionExtension::startStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Start a program", "Name", "The Name of the Loaded StateContext" ) );
        ret->add( "stopStateContext",
                  command
                  ( &ExecutionExtension::stopStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Stop a program", "Name", "The Name of the Started StateContext" ) );
        ret->add( "resetStateContext",
                  command
                  ( &ExecutionExtension::resetStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Reset a stateContext", "Name", "The Name of the Stopped StateContext") );
        ret->add( "steppedStateContext",
                  command
                  ( &ExecutionExtension::steppedStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Set a stateContext in step-at-a-time mode", "Name", "The Name of the StateContext") );
        ret->add( "continuousStateContext",
                  command
                  ( &ExecutionExtension::continuousStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Set a stateContext in continuous mode", "Name", "The Name of the StateContext") );
        return ret;
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
        if (master == 0 )
            return;
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

    DataSourceFactoryInterface* ExecutionComponentInterface::createDataSourceFactory()
    {
        return 0;
    }
}

