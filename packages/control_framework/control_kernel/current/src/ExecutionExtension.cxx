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
#include "control_kernel/DataObjectReporting.hpp"


#include <os/MutexLock.hpp>
#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/GlobalDataSourceFactory.hpp>
#include <execution/DataSourceFactoryInterface.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/parse_exception.hpp>
#include <execution/ParsedStateMachine.hpp>
#include <execution/MapDataSourceFactory.hpp>
#include <execution/ProgramLoader.hpp>

#include <corelib/PropertyComposition.hpp>
#include <corelib/Logger.hpp>

#include "execution/TemplateDataSourceFactory.hpp"
#include <execution/TemplateCommandFactory.hpp>

#include <functional>
#include <boost/function.hpp>
#include <fstream>

namespace ORO_ControlKernel
{
    using namespace detail;
    using namespace ORO_Execution;
    using namespace ORO_OS;
    using namespace boost;
    using namespace std;

    ExecutionExtension::ExecutionExtension( ControlKernelInterface* _base )
        : detail::ExtensionInterface( _base, "Execution" ),
          program(0),
          running_progr(false),count(0), base( _base ),
          interval("Interval", "The relative interval of executing a program node \
with respect to the Kernels period. Should be strictly positive ( > 0).", 1),
          tc(_base->getKernelName(), &proc) // pass task name, runnableinterface and processor.
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
        if ( !proc.activateStateMachine("Default") )
            Logger::log() << Logger::Info << "ExecutionExtension : "
                          << "Processor could not activate \"Default\" StateMachine."<< Logger::endl;
        else if ( !proc.startStateMachine("Default") )
            Logger::log() << Logger::Info << "ExecutionExtension : "
                          << "Processor could not start \"Default\" StateMachine."<< Logger::endl;
        proc.setTask( this->kernel()->getTask() );
        proc.initialize();
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

    bool ExecutionExtension::loadProgram( const std::string& filename, const std::string& file )
    {
        initKernelCommands();
        std::ifstream file_stream;
        std::stringstream text_stream( file );
        std::istream *prog_stream;
        if ( file.empty() ) {
            file_stream.open(filename.c_str());
            prog_stream = &file_stream;
        } else
            prog_stream = &text_stream;

        ProgramLoader loader;
        return loader.loadProgram( *prog_stream, &tc, filename );
    }

    bool ExecutionExtension::loadStateMachine( const std::string& filename, const std::string& file )
    {
        initKernelCommands();
        std::ifstream file_stream;
        std::stringstream text_stream( file );
        std::istream *prog_stream;
        if ( file.empty() ) {
            file_stream.open(filename.c_str());
            prog_stream = &file_stream;
        } else
            prog_stream = &text_stream;

        ProgramLoader loader;
        return loader.loadStateMachine( *prog_stream, &tc, filename );
    }

    const StateMachine* ExecutionExtension::getStateMachine(const std::string& name) {
        return getProcessor()->getStateMachine( name );
    }

    const ProgramInterface* ExecutionExtension::getProgram(const std::string& name) {
        return getProcessor()->getProgram(name);
    }

    bool ExecutionExtension::activateStateMachine(const std::string& name)
    {
        return proc.activateStateMachine(name);
    }

    bool ExecutionExtension::deactivateStateMachine(const std::string& name)
    {
        return proc.deactivateStateMachine(name);
    }

    bool ExecutionExtension::startStateMachine(const std::string& name)
    {
        return proc.startStateMachine(name);
    }

    bool ExecutionExtension::pauseStateMachine(const std::string& name)
    {
        return proc.pauseStateMachine(name);
    }

    bool ExecutionExtension::deleteStateMachine( const std::string& name )
    {
        return proc.deleteStateMachine( name );
    }

    bool ExecutionExtension::stopStateMachine(const std::string& name)
    {
        return proc.stopStateMachine(name);
    }

    bool ExecutionExtension::steppedStateMachine(const std::string& name)
    {
        return proc.steppedModeStateMachine(name);
    }

    bool ExecutionExtension::continuousStateMachine(const std::string& name)
    {
        return proc.continuousModeStateMachine(name);
    }

    bool ExecutionExtension::isStateMachineRunning(const std::string& name) const
    {
        return proc.isStateMachineRunning(name);
    }

    bool ExecutionExtension::resetStateMachine(const std::string& name)
    {
        return proc.resetStateMachine(name);
    }

    void ExecutionExtension::step() {
        if ( count % interval == 0 )
            {
                count = 0;
                proc.step();
            }
        ++count;
    }

    void ExecutionExtension::finalize()
    {
        proc.stopStateMachine("Default");
        proc.deactivateStateMachine("Default");
        proc.setTask(0);
        proc.finalize();
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
                    tc.commandFactory.registerObject( (*it)->getName(), commandfactory );

                datasourcefactory = (*it)->createDataSourceFactory();
                if ( datasourcefactory )
                    tc.dataFactory.registerObject( (*it)->getName(), datasourcefactory );

                methodfactory = (*it)->createMethodFactory();
                if ( methodfactory ) {
                    tc.methodFactory.registerObject( (*it)->getName(), methodfactory );
                }

                // Load the Extension Properties in the TaskContext :
                if ( (*it)->exportProperties( tc.attributeRepository ) == false ) 
                    Logger::log() << Logger::Error << "ExecutionExtension: Could not load Properties of "+ base->getKernelName()+"::"+(*it)->getName()+" as Task Attributes because of duplicate entry."<<Logger::endl;
                ++it;
            }
        
        // Now load in the DataObjects as DataSources
        std::vector<std::string> objnames;
        objnames.push_back("Inputs");
        objnames.push_back("Models");
        objnames.push_back("Commands");
        objnames.push_back("SetPoints");
        objnames.push_back("Outputs");
        for ( vector<string>::iterator it = objnames.begin(); it != objnames.end(); ++it )
            {
                DataSourceFactoryInterface* dsf;
                boost::shared_ptr<DataObjectReporting> drobjects( DataObjectReporting::nameserver.getObject( base->getKernelName() + "::" + *it ) );
                if ( drobjects == 0 )
                    Logger::log() << Logger::Debug << " Could not load "+ base->getKernelName()+"::"+*it+" as DataSources."<<Logger::endl;
                else {
                    dsf = new MapDataSourceFactory( *drobjects, *it + " Data Object" );
                    tc.dataFactory.registerObject( *it, dsf );
                }
            }
    }

    DataSourceFactoryInterface* ExecutionExtension::createDataSourceFactory()
    {
        // Add the data of the EE:
        TemplateDataSourceFactory< ExecutionExtension >* dat =
            newDataSourceFactory( this );

        dat->add( "isProgramRunning", data( &ExecutionExtension::isProgramRunning,
                                            "Is a program running ?", "Name", "The Name of the Loaded Program" ) );
        dat->add( "isStateMachineRunning", data( &ExecutionExtension::isStateMachineRunning,
                                            "Is a state context running ?", "Name", "The Name of the Loaded StateMachine" ) );
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
        ret->add( "activateStateMachine",
                  command
                  ( &ExecutionExtension::activateStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Activate a StateMachine", "Name", "The Name of the Loaded StateMachine" ) );
        ret->add( "deactivateStateMachine",
                  command
                  ( &ExecutionExtension::deactivateStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Deactivate a StateMachine", "Name", "The Name of the Stopped StateMachine" ) );
        ret->add( "startStateMachine",
                  command
                  ( &ExecutionExtension::startStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Start a StateMachine", "Name", "The Name of the Activated/Paused StateMachine" ) );
        ret->add( "pauseStateMachine",
                  command
                  ( &ExecutionExtension::pauseStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Pause a StateMachine", "Name", "The Name of a Started StateMachine" ) );
        ret->add( "stopStateMachine",
                  command
                  ( &ExecutionExtension::stopStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Stop a StateMachine", "Name", "The Name of the Started/Paused StateMachine" ) );
        ret->add( "resetStateMachine",
                  command
                  ( &ExecutionExtension::resetStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Reset a stateContext", "Name", "The Name of the Stopped StateMachine") );
        ret->add( "steppedStateMachine",
                  command
                  ( &ExecutionExtension::steppedStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Set a stateContext in step-at-a-time mode", "Name", "The Name of the StateMachine") );
        ret->add( "continuousStateMachine",
                  command
                  ( &ExecutionExtension::continuousStateMachine ,
                    &ExecutionExtension::true_gen ,
                    "Set a stateContext in continuous mode", "Name", "The Name of the StateMachine") );
        return ret;
    }

    bool ExecutionExtension::updateProperties( const PropertyBag& bag )
    {
        // The interval for executing the program, relative to the
        // kernel period. This property is optional.
        composeProperty(bag, interval);
        Logger::log() << Logger::Info << "ExecutionExtension Properties : " << Logger::nl
                      << interval.getName()<<" : "<< interval.get()<< Logger::endl;
        // check for validity.
        return interval > 0;
    }

    bool ExecutionExtension::exportProperties( AttributeRepository& bag )
    {
        return bag.addProperty( &interval );
    }

    ExecutionComponentInterface::ExecutionComponentInterface( const std::string& _name )
        : detail::ComponentFacetInterface<ExecutionExtension>( _name + std::string( "::Execution" ) ),
          name( _name ), master( 0 ),
          _commandfactory( 0 ), _methodfactory(0), _datasourcefactory( 0 )
    {
    }

    bool ExecutionComponentInterface::enableFacet( ExecutionExtension* ext )
    {
        master = ext;
        _commandfactory = this->createCommandFactory();
        if ( _commandfactory )
            master->getTaskContext()->commandFactory.registerObject( name, _commandfactory );
        _datasourcefactory = this->createDataSourceFactory();
        if ( _datasourcefactory )
            master->getTaskContext()->dataFactory.registerObject( name, _datasourcefactory );
        _methodfactory = this->createMethodFactory();
        if ( _methodfactory )
            master->getTaskContext()->methodFactory.registerObject( name, _methodfactory );
        return true;
    }

    void ExecutionComponentInterface::disableFacet()
    {
        if (master == 0 )
            return;
        master->getTaskContext()->commandFactory.unregisterObject( name );
        master->getTaskContext()->dataFactory.unregisterObject( name );
        master->getTaskContext()->methodFactory.unregisterObject( name );
        _commandfactory = 0;
        _datasourcefactory = 0;
        _methodfactory = 0;
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

