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
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
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
CommandFactoryInterface* KernelBaseFunction::createCommandFactory()
{
    TemplateCommandFactory< KernelBaseFunction >* ret =
        newCommandFactory( this );
    ret->add( "selectController", 
              command
              ( &KernelBaseFunction::selectController ,
                &KernelBaseFunction::true_gen,
                "Select a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "selectGenerator", 
              command
              ( &KernelBaseFunction::selectGenerator ,
                &KernelBaseFunction::true_gen,
                "Select a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "selectEstimator", 
              command
              ( &KernelBaseFunction::selectEstimator ,
                &KernelBaseFunction::true_gen,
                "Select a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "selectSensor", 
              command
              ( &KernelBaseFunction::selectSensor ,
                &KernelBaseFunction::true_gen,
                "Select a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "selectEffector", 
              command
              ( &KernelBaseFunction::selectEffector ,
                &KernelBaseFunction::true_gen,
                "Select a Effector Component", "Name", "The name of the Effector" ) );
    ret->add( "startController", 
              command
              ( &KernelBaseFunction::startController ,
                &KernelBaseFunction::true_gen,
                "Start a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "startGenerator", 
              command
              ( &KernelBaseFunction::startGenerator ,
                &KernelBaseFunction::true_gen,
                "Start a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "startEstimator", 
              command
              ( &KernelBaseFunction::startEstimator ,
                &KernelBaseFunction::true_gen,
                "Start a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "startSensor", 
              command
              ( &KernelBaseFunction::startSensor ,
                &KernelBaseFunction::true_gen,
                "Start a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "stopEffector", 
              command
              ( &KernelBaseFunction::stopEffector ,
                &KernelBaseFunction::true_gen,
                "Stop a Effector Component", "Name", "The name of the Effector" ) );
    ret->add( "stopController", 
              command
              ( &KernelBaseFunction::stopController ,
                &KernelBaseFunction::true_gen,
                "Stop a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "stopGenerator", 
              command
              ( &KernelBaseFunction::stopGenerator ,
                &KernelBaseFunction::true_gen,
                "Stop a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "stopEstimator", 
              command
              ( &KernelBaseFunction::stopEstimator ,
                &KernelBaseFunction::true_gen,
                "Stop a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "stopSensor", 
              command
              ( &KernelBaseFunction::stopSensor ,
                &KernelBaseFunction::true_gen,
                "Stop a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "stopEffector", 
              command
              ( &KernelBaseFunction::stopEffector ,
                &KernelBaseFunction::true_gen,
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

