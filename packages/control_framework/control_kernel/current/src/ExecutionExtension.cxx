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
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "control_kernel/ExecutionExtension.hpp"
#include "control_kernel/DataObjectReporting.hpp"

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
#include "control_kernel/PropertyExtension.hpp"
#endif

#include <os/MutexLock.hpp>
#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/GlobalDataSourceFactory.hpp>
#include <execution/DataSourceFactoryInterface.hpp>
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
          running_progr(false),count(0),
          interval("Interval", "The relative interval of executing a program node \
with respect to the Kernels period. Should be strictly positive ( > 0).", 1),
          tc(_base->getKernelName() ), // pass task name
          initcommands(false)
    {
    }

    ExecutionExtension::~ExecutionExtension()
    {
        tc.engine()->setTask( 0 );
    }

    using std::cerr;
    using std::endl;

    bool ExecutionExtension::initialize()
    {
        Logger::In in("ExecutionExtension");
        initKernelCommands();
        tc.engine()->setTask( this->kernel()->getTask() );
        bool res = tc.engine()->initialize();
        if ( res == false ) {
            Logger::log() << Logger::Error
                          << "Could not start Kernel's ExecutionEngine."<< Logger::endl;
            return false;
        }
        if ( tc.engine()->states()->getStateMachine("Default") == 0 ) {
            Logger::log() << Logger::Info
                          << "No \"Default\" StateMachine present."<< Logger::endl;
        } else {
            if ( !tc.engine()->states()->getStateMachine("Default")->activate() )
                Logger::log() << Logger::Error
                              << "Could not activate \"Default\" StateMachine."<< Logger::endl;
            else if ( !tc.engine()->states()->getStateMachine("Default")->start() )
                Logger::log() << Logger::Error
                              << "Could not start \"Default\" StateMachine."<< Logger::endl;
        }
        return true;
    }

#if 0
    const StateMachinePtr ExecutionExtension::getStateMachine(const std::string& name)
    {
        return tc.engine()->states()->getStateMachine(name);
    }
    const ProgramInterfacePtr ExecutionExtension::getProgram(const std::string& name)
    {
        return tc.engine()->states()->getProgram(name);
    }
#endif

    bool ExecutionExtension::unloadProgram( const std::string& name)
    {
        ProgramLoader loader;
        return loader.unloadProgram(name, &tc);
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

    bool ExecutionExtension::unloadStateMachine( const std::string& name)
    {
        ProgramLoader loader;
        return loader.unloadStateMachine(name, &tc);
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

    void ExecutionExtension::step() {
        if ( count % interval == 0 )
            {
                count = 0;
                tc.engine()->step();
            }
        ++count;
    }

    void ExecutionExtension::finalize()
    {
        StateMachinePtr def = tc.engine()->states()->getStateMachine("Default");
        if (def ) {
            def->stop();
            def->deactivate();
        }
        //tc.engine()->setTask(0);
        tc.engine()->finalize();
    }

    void ExecutionExtension::finishConstruction()
    {
        std::vector<detail::ExtensionInterface*>::const_iterator it = base()->getExtensions().begin();
        while ( it != base()->getExtensions().end() )
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
                    Logger::log() << Logger::Error << "ExecutionExtension: Could not load Properties of "+ base()->getKernelName()+"::"+(*it)->getName()+" as Task Attributes because of duplicate entry."<<Logger::endl;
                ++it;
            }
    }

    void ExecutionExtension::initKernelCommands()
    {
        // I wish I could do this cleaner, but I can not do it
        // in the constructor (to early) and not in initialize (to late).
        if ( initcommands )
            return;
        initcommands = true;

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
                boost::shared_ptr<DataObjectReporting> drobjects( DataObjectReporting::nameserver.getObject( base()->getKernelName() + "::" + *it ) );
                if ( drobjects == 0 )
                    Logger::log() << Logger::Debug << " Could not load "+ base()->getKernelName()+"::"+*it+" as DataSources."<<Logger::endl;
                else {
                    dsf = new MapDataSourceFactory( *drobjects, *it + " Data Object" );
                    tc.dataFactory.registerObject( *it, dsf );
                }
            }
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
          name( _name ), master( 0 ), componentTask(_name)
    {
    }

    TaskContext* ExecutionComponentInterface::getKernelContext() const {
        return master ? master->getTaskContext() : 0;
    }

    bool ExecutionComponentInterface::enableFacet( ExecutionExtension* ext )
    {
        master = ext;
        componentTask.engine()->reparent( getKernelContext()->engine() );

        getKernelContext()->addPeer( &componentTask );

        Logger::In in("ExecutionExtension");
        if ( this->component() == 0 || this->component()->kernel() == 0 ) {
            Logger::log() << Logger::Error
                          << "Can not initialise ExecutionExtension Facet before KernelBaseFunction Facet for Component "
                          << name <<Logger::nl;
            Logger::log() << "Reorder you Component's Facets."<<Logger::endl;
            return false;
        }

        // Create the default start/stop methods.
        TemplateMethodFactory< ComponentBaseInterface >* methods =
            newMethodFactory( this->component() );
        methods->add( "select", 
                      method
                      ( &ComponentBaseInterface::select ,
                        "Select this Component.(note: only valid for Data Flow Components )" ) );
        methods->add( "startup", 
                      method
                      ( &ComponentBaseInterface::startup ,
                        "Startup this Component." ) );
        methods->add( "restart", 
                      method
                      ( &ComponentBaseInterface::restart ,
                        "Restart (shutdown+startup) this Component." ) );
        methods->add( "shutdown", 
                      method
                      ( &ComponentBaseInterface::shutdown ,
                        "Shutdown this Component." ) );
        methods->add( "update", 
                      method
                      ( &ComponentBaseInterface::update ,
                        "Execute the update() method of this Component." ) );
        componentTask.methodFactory.registerObject("this", methods );

        // Create the default query methods.
        TemplateDataSourceFactory< ComponentBaseInterface >* datasources =
            newDataSourceFactory( this->component() );
        datasources->add( "isSelected", 
                          data( &ComponentBaseInterface::isSelected, "Check if this Component is selected.") );
        datasources->add( "isRunning", 
                          data( &ComponentBaseInterface::isRunning, "Check if this Component is started.") );
        componentTask.dataFactory.registerObject("this", datasources );

        CommandFactoryInterface* _commandfactory;
        MethodFactoryInterface* _methodfactory;
        DataSourceFactoryInterface* _datasourcefactory;

        _commandfactory = this->createCommandFactory();
        if ( _commandfactory )
            componentTask.commandFactory.registerObject( "this", _commandfactory );
        _datasourcefactory = this->createDataSourceFactory();
        if ( _datasourcefactory )
            componentTask.dataFactory.registerObject( "this", _datasourcefactory );
        _methodfactory = this->createMethodFactory();
        if ( _methodfactory )
            componentTask.methodFactory.registerObject( "this", _methodfactory );

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
        // get Properties
        PropertyExtension* pe = component()->kernel()->base()->getExtension<PropertyExtension>();
        if ( pe != 0 ) {
            PropertyBag cprops( pe->getComponentProperties( componentTask.getName() ) );
            for ( PropertyBag::iterator it=cprops.begin(); it!=cprops.end(); ++it) {
                // Add the PropertyBase to the repos.
                bool res = componentTask.attributeRepository.addProperty( *it );
                if (res == false ) {
                    Logger::log()<<Logger::Error <<"Failed to add Property "+(*it)->getName()+" of "
                                 <<componentTask.getName() << ". Possible duplicate entry in AttributeRepository."<<Logger::endl;
                }
            }
        } else {
            Logger::log() << Logger::Info << "Property Facet for Component "<<name
                          <<" not present or ordered after Execution Facet: No properties will be visible."<< Logger::endl;
        }
#endif
        return true;
    }

    void ExecutionComponentInterface::disableFacet()
    {
        if (master == 0 )
            return;
        componentTask.commandFactory.unregisterObject( "this" );
        componentTask.dataFactory.unregisterObject( "this" );
        componentTask.methodFactory.unregisterObject( "this" );

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
        // remove propbag from the the component task.
        PropertyExtension* pe = master->base()->getExtension<PropertyExtension>();
        if ( pe != 0 ) {
            PropertyBag cprops( pe->getComponentProperties( componentTask.getName() ) );
            for ( PropertyBag::iterator it=cprops.begin(); it!=cprops.end(); ++it) {
                // Add the PropertyBase to the repos.
                bool res = componentTask.attributeRepository.removeProperty( *it );
                if (res == false ) {
                    Logger::log()<<Logger::Warning <<"PropertyExtension: Failed to remove Property "+(*it)->getName() +" of "
                                 <<componentTask.getName() << ". Seems to be removed previously."<<Logger::endl;
                }
            }
        }
#endif

        componentTask.engine()->reparent( 0 );

        getKernelContext()->removePeer( componentTask.getName() );

        master = 0;
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

