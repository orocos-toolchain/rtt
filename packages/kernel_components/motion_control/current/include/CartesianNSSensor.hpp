/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  CartesianNSSensor.hpp 

                        CartesianNSSensor.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
 
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
     * @brief A Fake Cartesian Sensor measuring all data sent by the CartesianEffector.
     * @ingroup kcomps kcomp_sensor
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

