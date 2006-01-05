/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  KernelInterfaces.cxx 

                        KernelInterfaces.cxx -  description
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
#include "control_kernel/KernelInterfaces.hpp"
#include "control_kernel/ComponentInterfaces.hpp"
#include "control_kernel/ComponentStateInterface.hpp"

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
#include "control_kernel/KernelConfig.hpp"
#endif

#include "corelib/PropertyComposition.hpp"
#include "corelib/CompletionProcessor.hpp"
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "control_kernel/ExecutionExtension.hpp"
#include "execution/TemplateFactories.hpp"
#endif
#include <corelib/Logger.hpp>

using namespace ORO_ControlKernel;
using namespace ORO_CoreLib;
using boost::bind;

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        using namespace ORO_Execution;
#endif

ControlKernelInterface::~ControlKernelInterface() {}

ControlKernelInterface::ControlKernelInterface(const std::string& kname)
    : name("name","The name of the kernel.", kname), base_(0)
{
}

const std::string& ControlKernelInterface::getKernelName() const
{
    return name.get();
}

KernelBaseFunction* ControlKernelInterface::base() const
{
    return base_;
}

void ControlKernelInterface::base(KernelBaseFunction* b) 
{
    base_ = b;
}

void ControlKernelInterface::setKernelName( const std::string& _name)
{
    name = _name;
}

bool ControlKernelInterface::updateKernelProperties(const PropertyBag& bag)
{
    return true;
}
        
KernelBaseFunction::KernelBaseFunction( ControlKernelInterface* ckip )
    : detail::ExtensionInterface( ckip, "Kernel"),
      frequency("frequency","The periodic execution frequency of this kernel",0),
      default_process(0),
      process_owner(false),
      startupSensor("Sensor", "!Deprecated: Use DefaultProcess!", "DefaultSensor"),
      startupEstimator("Estimator", "!Deprecated: Use DefaultProcess!", "DefaultEstimator"),
      startupGenerator("Generator", "!Deprecated: Use DefaultProcess!", "DefaultGenerator"),
      startupController("Controller", "!Deprecated: Use DefaultProcess!", "DefaultController"),
      startupEffector("Effector", "!Deprecated: Use DefaultProcess!", "DefaultEffector")
{
    // inform the ControlKernelInterface that we are the KernelBaseFunction.
    kernel()->base( this );
    abortKernelEvent.connect( bind( &KernelBaseFunction::abortHandler, this ), CompletionProcessor::Instance() );

}

KernelBaseFunction::~KernelBaseFunction() {}

TaskInterface* KernelBaseFunction::getTask() const
{
    return kernel()->getTask();
}
/*
const std::string& KernelBaseFunction::getKernelName() const
{
    return kernel()->getKernelName();
}*/

bool KernelBaseFunction::initialize() 
{ 
    // initial startup of all components
    this->kernelStarted.fire();

    // First, startup all the support components
    NameServer<ComponentBaseInterface*>::value_iterator itl = this->supports.getValueBegin();
    for( ; itl != this->supports.getValueEnd(); ++itl)
        this->startupComponent( *itl );

    // Deprecated ! Use DefaultProcess !
    if ( selectComponent(startupSensor) )
        if ( selectComponent(startupEstimator) )
            if ( selectComponent(startupGenerator) )
                if ( selectComponent(startupController) )
                    if (selectComponent(startupEffector) ) {
                        if ( default_process )
                            default_process->startup();
                        Logger::log() << Logger::Info << "Kernel "<< kernel()->getKernelName() << " started."<< Logger::endl;
                        return true;
                    }
    Logger::log() << Logger::Error << "KernelBaseFunction failed to select startup components."<< Logger::endl;
    KernelBaseFunction::finalize();
    return false;
}

void KernelBaseFunction::step() 
{ 
    // update the default process component 
    updateComponents();
}

void KernelBaseFunction::finalize() 
{ 
    selectComponent("DefaultSensor");
    selectComponent("DefaultEstimator");
    selectComponent("DefaultGenerator");
    selectComponent("DefaultController");
    selectComponent("DefaultEffector");

    default_process->shutdown();
    
    ComponentMap::iterator itl;
    for( itl = components.begin(); itl != components.end(); ++itl)
        itl->second->shutdown();

    Logger::log() << Logger::Info << "Kernel "<< kernel()->getKernelName() << " stopped."<< Logger::endl;
    
    this->kernelStopped.fire();
}

double KernelBaseFunction::getPeriod() const
{ return 1./frequency; }

void KernelBaseFunction::setPeriod( double p )
{ frequency = 1./p; }

bool KernelBaseFunction::updateProperties(const PropertyBag& bag)
{
    bool result(false);
    result = composeProperty(bag, startupSensor);
    result = composeProperty(bag, startupEstimator) || result;
    result = composeProperty(bag, startupGenerator) || result;
    result = composeProperty(bag, startupController) || result;
    result = composeProperty(bag, startupEffector) || result;
    composeProperty(bag, frequency);

    Logger::In("KernelBaseFunction::updateProperties");
    if (result) {
        Logger::log() << Logger::Warning << "Deprecated use detected !"<<Logger::nl;
        Logger::log() << Logger::Warning << "Set the startup components in the DefaultProcess component's Property file !"<<Logger::nl;
    }

    Logger::log() << Logger::Info << "Properties set to : " << Logger::nl
                  << frequency.getName()<<" : "<< frequency.get()<< Logger::endl;

    return frequency > 0;
}

Event<void(void)>* KernelBaseFunction::eventGet(const std::string& name)
{
    if ( name == std::string("kernelStarted") )
        return &kernelStarted;
    if ( name == std::string("kernelStopped") )
        return &kernelStopped;
    if ( name == std::string("abortKernel") )
        return &abortKernelEvent;
    return &nullEvent;
}

bool KernelBaseFunction::addComponent(ComponentBaseInterface* comp)
{
    this->preLoad( comp );
    if (  comp->componentLoaded() )
        {
            if (comp->getName() == "DefaultProcess")
                this->default_process = comp;
            this->postLoad( comp );
            return true;
        }
    this->postLoad( comp );
    return false;
}

void KernelBaseFunction::removeComponent(ComponentBaseInterface* comp)
{
    ComponentMap::iterator itl 
        = components.find( comp->getName() );
    if (itl != components.end() ) {
        this->preUnload( comp );
        comp->componentUnloaded();
        //components.erase(itl);
        this->postUnload( comp );
    }
}

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
bool KernelBaseFunction::loadProperties( const std::string& filename )
{
    if (kernel()->getTask() && kernel()->getTask()->isRunning() )
        return false;

    KernelConfig config( (*this->kernel()), filename);
    return config.configure();
}
#endif

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

bool KernelBaseFunction::refreshProperties() {
    if ( kernel()->getExtension<ExecutionExtension>() == 0 
         ||
         kernel()->getExtension<ExecutionExtension>()->getTaskContext()->attributeRepository.properties() == 0 
         ||
         (kernel()->getTask() && kernel()->getTask()->isRunning() ) )
        return false;
    std::vector<detail::ExtensionInterface*>::const_iterator it = kernel()->getExtensions().begin();
    bool result = true;
    while ( it != kernel()->getExtensions().end() )
        {
            result = (*it)->updateProperties( *(kernel()->getExtension<ExecutionExtension>()->getTaskContext()->attributeRepository.properties())) && result;
            ++it;
        }
    return result;
}

bool KernelBaseFunction::exportProperties(AttributeRepository& bag)
{
    return bag.addConstant( frequency.getName(), frequency.get() ) &&
        bag.addProperty( &startupSensor) &&
        bag.addProperty( &startupEstimator) &&
        bag.addProperty( &startupGenerator) &&
        bag.addProperty( &startupController) &&
        bag.addProperty( &startupEffector);
}

MethodFactoryInterface* KernelBaseFunction::createMethodFactory()
{
    TemplateMethodFactory< KernelBaseFunction >* ret =
        newMethodFactory( this );
    ret->add( "start", 
              method
              ( &KernelBaseFunction::startKernel ,
                "Start the Kernel Task"  ) );
    ret->add( "stop", 
              method
              ( &KernelBaseFunction::stopKernel ,
                "Stop the Kernel Task"  ) );
    ret->add( "selectComponent", 
              method
              ( &KernelBaseFunction::selectComponent ,
                "Select a Component", "Name", "The name of the Component" ) );
    ret->add( "startComponent", 
              method
              ( &KernelBaseFunction::startComponent ,
                "Start a Component", "Name", "The name of the Component" ) );
    ret->add( "stopComponent", 
              method
              ( &KernelBaseFunction::stopComponent ,
                "Stop a Component", "Name", "The name of the Component" ) );
    ret->add( "loadProperties", 
              method
              ( &KernelBaseFunction::loadProperties ,
                "Load the Kernel's and Extensions Properties. Only works if this Kernel is not running.", "Filename", "The kernelconfig.xml file." ) );
    ret->add( "refreshProperties", 
              method
              ( &KernelBaseFunction::refreshProperties ,
                "Instruct the Kernel and its Extensions to re-read their Properties and reconfigure if necessary.\n Use this if you manually changed the properties through the kernel's task attributes.  Only works if this Kernel is not running." ) );
    return ret;
}


DataSourceFactoryInterface* KernelBaseFunction::createDataSourceFactory()
{
    TemplateDataSourceFactory< KernelBaseFunction >* ret =
        newDataSourceFactory( this );
    ret->add( "isSelected", 
              data( &KernelBaseFunction::isSelected, "Check if this Component is selected.",
                    "Name", "The name of the Component") );
    ret->add( "isStarted", 
              data( &KernelBaseFunction::isStarted, "Check if this Component is started.",
                    "Name", "The name of the Component") );
    ret->add( "isLoaded", 
              data( &KernelBaseFunction::isLoaded, "Check if this Component is loaded.",
                    "Name", "The name of the Component") );
    ret->add( "isRunning", 
              data( &KernelBaseFunction::isRunning, "Check if the Kernel is running."  ) );
    return ret;
}

        ComponentBaseInterface* KernelBaseFunction::switchComponent(ComponentBaseInterface* oldC, ComponentBaseInterface* newC ) const
        {
            shutdownComponent(oldC);
            if ( startupComponent( newC ) )
                return newC;
            else
                if ( startupComponent( oldC ) )
                    return oldC;
            return 0; // quite severe failure
        }

        bool KernelBaseFunction::startupComponent(ComponentBaseInterface* c ) const 
        {
            if ( c->componentStartup() )
                {
                    c->selected = true;
                    return true;
                }
            return false;
        }
        
        void KernelBaseFunction::shutdownComponent(ComponentBaseInterface* c ) const 
        {
            c->componentShutdown();
            c->selected = false;
        }



#endif

