 
#ifndef CARTESIAN_NS_SENSOR_HPP
#define CARTESIAN_NS_SENSOR_HPP

#include <corelib/EventInterfaces.hpp>
#include <kernel_components/Simulator.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include "CartesianNSDataObjects.hpp"


/**
 * @file CartesianNSComponents.hpp
 *
 * This file contains components for
 * use in cartesian path planning. Only Nameserved
 * DataObjects are used.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;

#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif

    /**
     * A Fake Cartesian Sensor measuring all data sent by the CartesianEffector.
     */
    template <class Base>
    class CartesianSensor
        : public Base
    {
    public:
        typedef CartesianNSSensorInput InputDataObject;
        /**
         * Necessary typedefs.
         */
        typedef typename Base::InputType InputType;
            
        CartesianSensor(SimulatorInterface* _sim = 0) 
            : Base("CartesianSensor"),
              sensorError(Event::SYNASYN, "CartesianSensor::SensorError"),
              q6("JointPositions",""), sim(_sim)
        {}
            
        virtual bool componentStartup()
        {
            if ( !Base::Input::dObj()->Get("JointPositions", jpos_DObj ) )
                return false;
            pull();
            push();
            return true;
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()      
        {
            /*
             * Fake physical sensor data.
             */
            if (sim != 0)
                q6 = sim->getJointPositions();
            else
                q6 = 0;
        }

        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            jpos_DObj->Set( q6 );
        }

            
        /**
         * @see KernelReportingExtension.hpp class ReportingComponent
         */
        virtual void exportReports(PropertyBag& bag)
        {
            bag.add(&q6);
        }

            
    protected:
        Event sensorError;

        Property<Double6D> q6;
        DataObjectInterface<Double6D>* jpos_DObj;

        SimulatorInterface* sim;
    };

}
#endif

