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

namespace ORO_ControlKernel
{
    namespace detail
    {
        NameServer<ExtensionInterface*> ExtensionInterface::nameserver;
    }
}

KernelBaseFunction::KernelBaseFunction( KernelBaseFunction* _base )
    : running(false), 
      name("name","The name of the kernel.", "Default"),
      //              priority("priority","The priority of the kernel."),
      frequency("frequency","The periodic execution frequency of this kernel",0), mytask(0),
      kernelStarted(Event::SYNASYN), kernelStopped(Event::SYNASYN), nullEvent(Event::SYNASYN),
      startupSensor("Sensor", "", "DefaultSensor"),
      startupEstimator("Estimator", "", "DefaultEstimator"),
      startupGenerator("Generator", "", "DefaultGenerator"),
      startupController("Controller", "", "DefaultController"),
      startupEffector("Effector", "", "DefaultEffector")
{}

KernelBaseFunction::~KernelBaseFunction() {}

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

TaskInterface* KernelBaseFunction::getTask() const
{
    return mytask;
}

void KernelBaseFunction::setTask( TaskInterface* task )
{
    mytask = task;
}

const std::string& KernelBaseFunction::getKernelName() const
{
    return name.get();
}

double KernelBaseFunction::getPeriod() const
{ return 1./frequency; }

void KernelBaseFunction::setPeriod( double p )
{ frequency = 1./p; }

bool KernelBaseFunction::updateProperties(const PropertyBag& bag)
{
    return composeProperty(bag, frequency);
}

void KernelBaseFunction::startupComponents(const PropertyBag& bag)
{
    composeProperty(bag, startupSensor);
    composeProperty(bag, startupEstimator);
    composeProperty(bag, startupGenerator);
    composeProperty(bag, startupController);
    composeProperty(bag, startupEffector);
}

bool KernelBaseFunction::updateKernelProperties(const PropertyBag& bag)
{
    return true;
}
        
HandlerRegistrationInterface* KernelBaseFunction::eventGet(const std::string& name)
{
    if ( name == std::string("kernelStarted") )
        return &kernelStarted;
    if ( name == std::string("kernelStopped") )
        return &kernelStopped;
    return &nullEvent;
}

void KernelBaseFunction::setKernelName( const std::string& _name)
{
    name = _name;
}

void KernelBaseFunction::addComponent(ComponentBaseInterface* comp)
{
    components.push_back(comp);
}

void KernelBaseFunction::removeComponent(ComponentBaseInterface* comp)
{
    std::vector<ComponentBaseInterface*>::iterator itl 
        = std::find(components.begin(), components.end(), comp);
    if (itl != components.end() )
        components.erase(itl);
}


#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
CommandFactoryInterface* KernelBaseFunction::createCommandFactory()
{
    TemplateCommandFactory< KernelBaseFunction  >* ret =
        newCommandFactory( this );
    ret->add( "selectController", 
              command
              ( &KernelBaseFunction::selectController ,
                &KernelBaseFunction::isSelectedController,
                "Select a Controller Component", "Name", "The name of the Controller" ) );
    ret->add( "selectGenerator", 
              command
              ( &KernelBaseFunction::selectGenerator ,
                &KernelBaseFunction::isSelectedGenerator,
                "Select a Generator Component", "Name", "The name of the Generator" ) );
    ret->add( "selectEstimator", 
              command
              ( &KernelBaseFunction::selectEstimator ,
                &KernelBaseFunction::isSelectedEstimator,
                "Select a Estimator Component", "Name", "The name of the Estimator" ) );
    ret->add( "selectSensor", 
              command
              ( &KernelBaseFunction::selectSensor ,
                &KernelBaseFunction::isSelectedSensor,
                "Select a Sensor Component", "Name", "The name of the Sensor" ) );
    ret->add( "selectEffector", 
              command
              ( &KernelBaseFunction::selectEffector ,
                &KernelBaseFunction::isSelectedEffector,
                "Select a Effector Component", "Name", "The name of the Effector" ) );
    return ret;
}

DataSourceFactoryInterface* KernelBaseFunction::createDataSourceFactory()
{
    TemplateDataSourceFactory< KernelBaseFunction >* ret =
        newDataSourceFactory( this );
    ret->add( "usingGenerator", 
              data( &KernelBaseFunction::isSelectedGenerator, "Check if this generator is used.",
                    "Name", "The name of the Generator") );
    ret->add( "usingController", 
              data( &KernelBaseFunction::isSelectedController, "Check if this controller is used.",
                    "Name", "The name of the Controller") );
    ret->add( "usingEstimator", 
              data( &KernelBaseFunction::isSelectedEstimator, "Check if this estimator is used.",
                    "Name", "The name of the Estimator") );
    ret->add( "usingEffector", 
              data( &KernelBaseFunction::isSelectedEffector, "Check if this effector is used.",
                    "Name", "The name of the Effector") );
    ret->add( "usingSensor", 
              data( &KernelBaseFunction::isSelectedSensor, "Check if this sensor is used.",
                    "Name", "The name of the Sensor") );
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

