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

#pragma implementation
#include "control_kernel/KernelInterfaces.hpp"
#include "control_kernel/ComponentInterfaces.hpp"
#include "corelib/PropertyComposition.hpp"
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/TemplateFactories.hpp"
#endif

using namespace ORO_ControlKernel;
using namespace ORO_CoreLib;

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        using namespace ORO_Execution;
#endif


const std::string& ControlKernelInterface::getKernelName() const
{
    return name.get();
}

void ControlKernelInterface::setKernelName( const std::string& _name)
{
    name = _name;
}

bool ControlKernelInterface::updateKernelProperties(const PropertyBag& bag)
{
    return true;
}
        
KernelBaseFunction::KernelBaseFunction( ControlKernelInterface* _base )
    : detail::ExtensionInterface( _base, "Kernel"),
      running(false), 
      //              priority("priority","The priority of the kernel."),
      frequency("frequency","The periodic execution frequency of this kernel",0),
      //kernelStarted(Event::SYNASYN), kernelStopped(Event::SYNASYN), nullEvent(Event::SYNASYN),
      startupSensor("Sensor", "", "DefaultSensor"),
      startupEstimator("Estimator", "", "DefaultEstimator"),
      startupGenerator("Generator", "", "DefaultGenerator"),
      startupController("Controller", "", "DefaultController"),
      startupEffector("Effector", "", "DefaultEffector"),
      cki( _base )
{}

KernelBaseFunction::~KernelBaseFunction() {}

TaskInterface* KernelBaseFunction::getTask() const
{
    return cki->getTask();
}
    
const std::string& KernelBaseFunction::getKernelName() const
{
    return cki->getKernelName();
}

bool KernelBaseFunction::initialize() 
{ 
    running = true;
    if ( selectSensor(startupSensor) )
        if ( selectEstimator(startupEstimator) )
            if ( selectGenerator(startupGenerator) )
                if ( selectController(startupController) )
                    if (selectEffector(startupEffector) )
                        return true;
    KernelBaseFunction::finalize();
    return false;
}

void KernelBaseFunction::step() 
{ 
    // update the components 
    updateComponents();
}

void KernelBaseFunction::finalize() 
{ 
    if (running == false )
        return; // detect abort condition
    running = false;
    selectSensor("DefaultSensor");
    selectEstimator("DefaultEstimator");
    selectGenerator("DefaultGenerator");
    selectController("DefaultController");
    selectEffector("DefaultEffector");
    // stop all other components in kernel implementation class.
}

double KernelBaseFunction::getPeriod() const
{ return 1./frequency; }

void KernelBaseFunction::setPeriod( double p )
{ frequency = 1./p; }

bool KernelBaseFunction::updateProperties(const PropertyBag& bag)
{
    composeProperty(bag, startupSensor);
    composeProperty(bag, startupEstimator);
    composeProperty(bag, startupGenerator);
    composeProperty(bag, startupController);
    composeProperty(bag, startupEffector);
    return composeProperty(bag, frequency);
}

Event<void(void)>* KernelBaseFunction::eventGet(const std::string& name)
{
    if ( name == std::string("kernelStarted") )
        return &kernelStarted;
    if ( name == std::string("kernelStopped") )
        return &kernelStopped;
    return &nullEvent;
}

bool KernelBaseFunction::addComponent(ComponentBaseInterface* comp)
{
    this->preLoad( comp );
    if (  comp->componentLoaded() )
        {
            components.insert( std::make_pair(comp->getName(), comp) );
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
        components.erase(itl);
        this->postUnload( comp );
    }
}


#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

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
    ret->add( "selectController", 
              method
              ( &KernelBaseFunction::selectController ,
                "Select a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "selectGenerator", 
              method
              ( &KernelBaseFunction::selectGenerator ,
                "Select a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "selectEstimator", 
              method
              ( &KernelBaseFunction::selectEstimator ,
                "Select a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "selectSensor", 
              method
              ( &KernelBaseFunction::selectSensor ,
                "Select a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "selectEffector", 
              method
              ( &KernelBaseFunction::selectEffector ,
                "Select a Effector Component", "Name", "The name of the Effector" ) );
    ret->add( "startController", 
              method
              ( &KernelBaseFunction::startController ,
                "Start a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "startGenerator", 
              method
              ( &KernelBaseFunction::startGenerator ,
                "Start a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "startEstimator", 
              method
              ( &KernelBaseFunction::startEstimator ,
                "Start a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "startSensor", 
              method
              ( &KernelBaseFunction::startSensor ,
                "Start a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "stopEffector", 
              method
              ( &KernelBaseFunction::stopEffector ,
                "Stop a Effector Component", "Name", "The name of the Effector" ) );
    ret->add( "stopController", 
              method
              ( &KernelBaseFunction::stopController ,
                "Stop a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "stopGenerator", 
              method
              ( &KernelBaseFunction::stopGenerator ,
                "Stop a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "stopEstimator", 
              method
              ( &KernelBaseFunction::stopEstimator ,
                "Stop a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "stopSensor", 
              method
              ( &KernelBaseFunction::stopSensor ,
                "Stop a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "stopEffector", 
              method
              ( &KernelBaseFunction::stopEffector ,
                "Stop a Effector Component", "Name", "The name of the Effector" ) );
    return ret;
}


DataSourceFactoryInterface* KernelBaseFunction::createDataSourceFactory()
{
    TemplateDataSourceFactory< KernelBaseFunction >* ret =
        newDataSourceFactory( this );
    ret->add( "isSelectedGenerator", 
              data( &KernelBaseFunction::isSelectedGenerator, "Check if this generator is selected.",
                    "Name", "The name of the Generator") );
    ret->add( "isSelectedController", 
              data( &KernelBaseFunction::isSelectedController, "Check if this controller is selected.",
                    "Name", "The name of the Controller") );
    ret->add( "isSelectedEstimator", 
              data( &KernelBaseFunction::isSelectedEstimator, "Check if this estimator is selected.",
                    "Name", "The name of the Estimator") );
    ret->add( "isSelectedEffector", 
              data( &KernelBaseFunction::isSelectedEffector, "Check if this effector is selected.",
                    "Name", "The name of the Effector") );
    ret->add( "isSelectedSensor", 
              data( &KernelBaseFunction::isSelectedSensor, "Check if this sensor is selected.",
                    "Name", "The name of the Sensor") );
    ret->add( "isStarted", 
              data( &KernelBaseFunction::isStarted, "Check if this Component is started.",
                    "Name", "The name of the Component") );
    ret->add( "isLoaded", 
              data( &KernelBaseFunction::isLoaded, "Check if this Component is loaded.",
                    "Name", "The name of the Component") );
    return ret;
}

        ComponentBaseInterface* KernelBaseFunction::switchComponent(ComponentBaseInterface* oldC, ComponentBaseInterface* newC ) const
        {
            stopComponent(oldC);
            if ( startComponent( newC ) )
                return newC;
            else
                if ( startComponent( oldC ) )
                    return oldC;
            return 0; // quite severe failure
        }

        bool KernelBaseFunction::startComponent(ComponentBaseInterface* c ) const 
        {
            if ( c->componentStartup() )
                {
                    c->selected = true;
                    return true;
                }
            return false;
        }
        
        void KernelBaseFunction::stopComponent(ComponentBaseInterface* c ) const 
        {
            c->componentShutdown();
            c->selected = false;
        }



#endif

