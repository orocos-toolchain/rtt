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


#include <os/MutexLock.hpp>
#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/GlobalDataSourceFactory.hpp>
#include <execution/DataSourceFactoryInterface.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/Parser.hpp>
#include <execution/parse_exception.hpp>
#include <execution/ParsedStateContext.hpp>

#include <corelib/PropertyComposition.hpp>

#include "execution/TemplateDataSourceFactory.hpp"
#include <execution/TemplateCommandFactory.hpp>

#include <functional>
#include <boost/function.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_Execution;
    using namespace ORO_OS;
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

    // is this still needed ???
//     TaskInterface* ExecutionExtension::getTask() const
//     {
//         return base->getTask();
//     }

//     void ExecutionExtension::setTask( TaskInterface* task )
//     {
//         base->setTask( task );
//     }

    bool ExecutionExtension::initialize()
    {
        initKernelCommands();
        if ( !proc.activateStateContext("Default") )
            cerr << "Warning : Processor could not activate \"Default\" StateContext."<<endl;
        else if ( !proc.startStateContext("Default") )
            cerr << "Warning : Processor could not start \"Default\" StateContext."<<endl;
        return true;
    }

    bool ExecutionExtension::startProgram(const std::string& name)
    {
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

    bool ExecutionExtension::pauseProgram(const std::string& name)
    {
        return proc.pauseProgram(name);
    }

    bool ExecutionExtension::stepProgram(const std::string& name)
    {
        return proc.stepProgram(name);
    }

    bool ExecutionExtension::loadProgram( std::istream& prog_stream )
    {
        initKernelCommands();
        Parser    parser;
        std::vector<ProgramGraph*> pg_list;
        try {
            pg_list = parser.parseProgram( prog_stream, &proc, this );
        }
        catch( const file_parse_exception& exc )
        {
          // no reason to catch this other than clarity
          throw;
        }
        if ( pg_list.empty() )
        {
          throw program_load_exception( "No Programs defined in inputfile." );
        }
        for_each(pg_list.begin(), pg_list.end(), boost::bind( &Processor::loadProgram, &proc, _1) );
        return true;
    }

    namespace {
        void recursiveRegister( std::map<std::string, ParsedStateContext*>& m, ParsedStateContext* parent ) {
            std::vector<StateContextTree*>::const_iterator chit;
            m[parent->getName()] = parent;
            for( chit = parent->getChildren().begin(); chit != parent->getChildren().end(); ++chit)
                recursiveRegister(m, dynamic_cast<ParsedStateContext*>(*chit) );
        }
    }

    void ExecutionExtension::loadStateContext( std::istream& state_stream, const std::string& filename )
    {
        initKernelCommands();
        Parser    parser;
        std::vector<ParsedStateContext*> contexts;
        try {
          contexts = parser.parseStateContext( state_stream, filename, &proc, this );
        }
        catch( const file_parse_exception& exc )
        {
          // no reason to catch this other than clarity
          throw;
        }
        if ( contexts.empty() )
        {
          throw program_load_exception( "No StateContexts instantiated in file \"" + filename + "\"." );
        }

        // this can throw a program_load_exception
        std::vector<ParsedStateContext*>::iterator it;
        for( it= contexts.begin(); it !=contexts.end(); ++it) {
            getProcessor()->loadStateContext( *it );
            recursiveRegister( parsed_states, *it );
        }
    }

    ParsedStateContext* ExecutionExtension::getStateContext(const std::string& name) {
        if ( parsed_states.count(name) == 0 )
            return 0;
        return parsed_states[ name ];
    }

    ProgramInterface* ExecutionExtension::getProgram(const std::string& name) {
        return getProcessor()->getProgram(name);
    }

    bool ExecutionExtension::activateStateContext(const std::string& name)
    {
        return proc.activateStateContext(name);
    }

    bool ExecutionExtension::deactivateStateContext(const std::string& name)
    {
        return proc.deactivateStateContext(name);
    }

    bool ExecutionExtension::startStateContext(const std::string& name)
    {
        return proc.startStateContext(name);
    }

    bool ExecutionExtension::pauseStateContext(const std::string& name)
    {
        return proc.pauseStateContext(name);
    }

    bool ExecutionExtension::deleteStateContext( const std::string& name )
    {
        parsed_states.erase( name );
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

    bool ExecutionExtension::executeCommand( CommandInterface* c)
    {
        if ( this->kernel()->getTask()->isRunning() )
            return proc.process( c );
        else {
            MutexLock lockit( execguard );
            c->execute();
        }
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
        proc.stopStateContext("Default");
        proc.deactivateStateContext("Default");
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
                CommandFactoryInterface* commandfactory;
                DataSourceFactoryInterface* datasourcefactory;
                MethodFactoryInterface* methodfactory;

                // Add the commands / data of the kernel :
                commandfactory = (*it)->createCommandFactory();
                if ( commandfactory )
                    commandFactory().registerObject( (*it)->getName(), commandfactory );

                datasourcefactory = (*it)->createDataSourceFactory();
                if ( datasourcefactory )
                    dataFactory().registerObject( (*it)->getName(), datasourcefactory );

                methodfactory = (*it)->createMethodFactory();
                if ( methodfactory ) {
                    methodFactory().registerObject( (*it)->getName(), methodfactory );
                }
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
        ret->add( "stepProgram",
                  command
                  ( &ExecutionExtension::stepProgram ,
                    &ExecutionExtension::true_gen ,
                    "Step a single program instruction", "Name", "The Name of the Paused Program" ) );
        ret->add( "pauseProgram",
                  command
                  ( &ExecutionExtension::pauseProgram ,
                    &ExecutionExtension::true_gen ,
                    "Pause a program", "Name", "The Name of the Started Program" ) );
        ret->add( "activateStateContext",
                  command
                  ( &ExecutionExtension::activateStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Activate a StateContext", "Name", "The Name of the Loaded StateContext" ) );
        ret->add( "deactivateStateContext",
                  command
                  ( &ExecutionExtension::deactivateStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Deactivate a StateContext", "Name", "The Name of the Stopped StateContext" ) );
        ret->add( "startStateContext",
                  command
                  ( &ExecutionExtension::startStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Start a StateContext", "Name", "The Name of the Activated/Paused StateContext" ) );
        ret->add( "pauseStateContext",
                  command
                  ( &ExecutionExtension::pauseStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Pause a StateContext", "Name", "The Name of a Started StateContext" ) );
        ret->add( "stopStateContext",
                  command
                  ( &ExecutionExtension::stopStateContext ,
                    &ExecutionExtension::true_gen ,
                    "Stop a StateContext", "Name", "The Name of the Started/Paused StateContext" ) );
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

    bool ExecutionExtension::updateProperties( const PropertyBag& bag )
    {
        // The interval for executing the program, relative to the
        // kernel period. This property is optional.
        composeProperty(bag, interval);
        // check for validity.
        return interval > 0;
    }

    ExecutionComponentInterface::ExecutionComponentInterface( const std::string& _name )
        : detail::ComponentAspectInterface<ExecutionExtension>( _name + std::string( "::Execution" ) ),
          name( _name ), master( 0 ),
          _commandfactory( 0 ), _methodfactory(0), _datasourcefactory( 0 )
    {
    }

    bool ExecutionComponentInterface::enableAspect( ExecutionExtension* ext )
    {
        master = ext;
        _commandfactory = this->createCommandFactory();
        if ( _commandfactory )
            master->commandFactory().registerObject( name, _commandfactory );
        _datasourcefactory = this->createDataSourceFactory();
        if ( _datasourcefactory )
            master->dataFactory().registerObject( name, _datasourcefactory );
        _methodfactory = this->createMethodFactory();
        if ( _methodfactory )
            master->methodFactory().registerObject( name, _methodfactory );
        return true;
    }

    void ExecutionComponentInterface::disableAspect()
    {
        if (master == 0 )
            return;
        master->commandFactory().unregisterObject( name );
        master->dataFactory().unregisterObject( name );
        master->methodFactory().unregisterObject( name );
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

    MethodFactoryInterface* ExecutionComponentInterface::createMethodFactory()
    {
        return 0;
    }
}

