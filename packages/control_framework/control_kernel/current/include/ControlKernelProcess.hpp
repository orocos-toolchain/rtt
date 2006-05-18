/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ControlKernelProcess.hpp 

                        ControlKernelProcess.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_CONTROL_KERNEL_PROCESS_HPP
#define ORO_CONTROL_KERNEL_PROCESS_HPP

#include <corelib/PropertyComposition.hpp>
#include "control_kernel/BaseComponents.hpp"
#include "control_kernel/DataServer.hpp"

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
#include <control_kernel/PropertyExtension.hpp>
#include <corelib/PropertyComposition.hpp>
#endif
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/TemplateFactories.hpp"
#include <control_kernel/ExecutionExtension.hpp>
#endif

#include "control_kernel/ExtensionComposition.hpp"

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    /**
     * This Process executes (when running) all started Data Flow
     * Components in each control kernel step.  In addition you can
     * specify through its properties which data flow components to
     * start and stop by default, although only one instance per
     * component type.
     */
    class ControlKernelProcess
        : public Process<Expects<NoCommand>, 
                         Expects<NoInput>,
                         Expects<NoModel>,
                         Expects<NoSetPoint>,
                         Expects<NoOutput>,
                         MakeFacet<
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
        PropertyExtension, 
#endif
        KernelBaseFunction 
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        ,ExecutionExtension
#endif
        >::CommonBase >
    {
        typedef Process<Expects<NoCommand>, 
                        Expects<NoInput>,
                        Expects<NoModel>,
                        Expects<NoSetPoint>,
                        Expects<NoOutput>,
                        MakeFacet<
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
            PropertyExtension, 
#endif
            KernelBaseFunction 
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
            ,ExecutionExtension
#endif
            >::CommonBase > Base;

    public:
        ControlKernelProcess( const std::string& name )
            : Base(name),
              frequency("frequency","The periodic execution frequency of the control kernel process (unused)",0.0),
              startupSensor("Sensor", "", "DefaultSensor"),
              startupEstimator("Estimator", "", "DefaultEstimator"),
              startupGenerator("Generator", "", "DefaultGenerator"),
              startupController("Controller", "", "DefaultController"),
              startupEffector("Effector", "", "DefaultEffector"),
              startupComponents("StartupComponents","The Components to be Selected when the kernel is started")
        {
            startupComponents.value().add( &startupSensor );
            startupComponents.value().add( &startupEstimator );
            startupComponents.value().add( &startupGenerator );
            startupComponents.value().add( &startupController );
            startupComponents.value().add( &startupEffector );
        }

        /**
         * Upon startup, select user defined components.
         */
        bool componentStartup() 
        { 
            if ( kernel()->selectComponent(startupSensor) )
                if ( kernel()->selectComponent(startupEstimator) )
                    if ( kernel()->selectComponent(startupGenerator) )
                        if ( kernel()->selectComponent(startupController) )
                            if (kernel()->selectComponent(startupEffector) ) {
                                Logger::log() << Logger::Info << "Control Kernel Process "<< kernel()->base()->getKernelName() << " started."<< Logger::endl;
                                return true;
                            }
            Logger::log() << Logger::Error << "Control Kernel Process failed to select startup components."<< Logger::endl;
            componentShutdown();
            return false;
        }

        void componentShutdown() 
        { 
            kernel()->selectComponent("DefaultSensor");
            kernel()->selectComponent("DefaultEstimator");
            kernel()->selectComponent("DefaultGenerator");
            kernel()->selectComponent("DefaultController");
            kernel()->selectComponent("DefaultEffector");
    
            Logger::log() << Logger::Info << "Control Kernel Process "<< kernel()->base()->getKernelName() << " stopped."<< Logger::endl;
        }


        /**
         * Update all Data Flow Components which are started.
         * Updates are in the following order : 
         * Sensors, Estimators, Generators, Controllers, Effectors.
         */
        virtual void update()
        {
            // This is called from the StandardControlKernel.
            // one update is one control cycle
            // The figure is a unidirectional graph
            std::for_each(kernel()->sensors.getValueBegin(), kernel()->sensors.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(kernel()->estimators.getValueBegin(), kernel()->estimators.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(kernel()->generators.getValueBegin(), kernel()->generators.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(kernel()->controllers.getValueBegin(), kernel()->controllers.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(kernel()->effectors.getValueBegin(), kernel()->effectors.getValueEnd(), Updater<ComponentBaseInterface>() );
        }

    protected:

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY

        bool updateProperties(const PropertyBag& bag)
        {
            composeProperty(bag, startupComponents);
            composeProperty(bag, frequency);

            Logger::log() << Logger::Info << "Control Kernel Process Properties : " << Logger::nl
                          << frequency.getName()<<" : "<< frequency.get()<< Logger::nl
                          << startupEffector.getName()<<" : "<< startupEffector.get()<< Logger::nl
                          << startupEstimator.getName()<<" : "<< startupEstimator.get()<< Logger::nl
                          << startupGenerator.getName()<<" : "<< startupGenerator.get()<< Logger::nl
                          << startupController.getName()<<" : "<< startupController.get()<< Logger::nl
                          << startupSensor.getName()<<" : "<< startupSensor.get()<< Logger::endl;
            
            //return frequency > 0;
            return true;
        }

        void exportProperties(PropertyBag& bag)
        {
            bag.add( &frequency );
            bag.add( &startupComponents );
        }

#endif
        template<class T>
        struct Updater {
            void operator() (T* t) {
                if ( t->isSelected() )
                    t->update();
            }
        };

        Property<double> frequency;
        Property<std::string> startupSensor;
        Property<std::string> startupEstimator;
        Property<std::string> startupGenerator;
        Property<std::string> startupController;
        Property<std::string> startupEffector;
        Property<PropertyBag> startupComponents;
    };
}

#endif
