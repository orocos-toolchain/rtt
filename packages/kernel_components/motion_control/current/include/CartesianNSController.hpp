/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  CartesianNSController.hpp 

                        CartesianNSController.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef CARTESIAN_NS_CONTROLLER_HPP
#define CARTESIAN_NS_CONTROLLER_HPP

#include <geometry/frames.h>
#include <geometry/frames_io.h>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include "CartesianNSDataObjects.hpp"

/**
 * @file CartesianNSController.hpp
 *
 * This file contains a Controller calculating
 * the reference velocities for 6 axes.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
    using namespace ORO_KinDyn;
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif

    /**
     * @brief A Cartesian Controller which converts a Frame setpoint
     * to joint positions and does the control on joint level.
     * @ingroup kcomps kcomp_controller
     */
    template <class Base>
    class CartesianController
        : public Base
    {
    public:
        typedef CartesianNSDriveOutput OutputDataObject;
        typedef CartesianNSSetPoint SetPointDataObject;
        typedef CartesianNSSensorInput InputDataObject;
        typedef CartesianNSModel ModelDataObject;

        /**
         * Necessary typedefs.
         */
        typedef typename Base::SetPointType SetPointType;
        typedef typename Base::InputType InputType;
        typedef typename Base::ModelType ModelType;
        typedef typename Base::OutputType OutputType;
            
        CartesianController(KinematicsInterface* k) 
            :  Base("CartesianController"),gain("Gain","The error gain.",0),
              end_twist("Result Twist",""), kineComp(k), q_err("Velocity Setpoints","")
        {}

        virtual bool componentStartup()
        {
            // Resolve the DataObjects :
            if ( !Base::Input::dObj()->Get("JointPositions", jpos_DObj) ||
                 !Base::Model::dObj()->Get("EndEffPosition", curFrame_DObj) ||
                 !Base::SetPoint::dObj()->Get("EndEffectorFrame", desFrame_DObj) ||
                 !Base::Output::dObj()->Get("JointVelocities", jvel_DObj) )
                return false;
            return true;
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()      
        {
            jpos_DObj->Get( q6 );
            curFrame_DObj->Get( mp_base_frame );
            desFrame_DObj->Get( setpoint_frame );
                
            kineComp.stateSet( q6 );
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void calculate() 
        {
            // OK FOR ROTATIONAL PART OF TWIST :
            ee_base = mp_base_frame;
            //ee_base.p = Vector::Zero();
                
            kineComp.positionInverse(setpoint_frame, q_des );
            q_err = q_des - q6; 
            q_dot = q_err.value() * gain ;
            // ( goal_frame, current_frame )
#if 0
            mp_base_twist.vel = FrameDifference(setpoint_frame, mp_base_frame).vel*gain;
            mp_base_twist.rot = (ee_base * FrameDifference(setpoint_frame, mp_base_frame)).rot*gain;
#endif
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            jvel_DObj->Set( q_dot );
        }
            
        /**
         * @see KernelReportingExtension.hpp class ReportingComponent
         */
        virtual void exportReports(PropertyBag& bag)
        {
            bag.add(&q_err);
        }

        /**
         * @see KernelPropertyExtension.hpp class PropertyComponentInterface
         */
        virtual bool updateProperties( const PropertyBag& bag)
        {
            return composeProperty(bag, gain );
        }
            
        /**
         * @see KernelPropertyExtension.hpp class PropertyComponentInterface
         */
        virtual void exportProperties( PropertyBag& bag )
        {
            bag.add(&gain);
        }
            
    protected:
        Property<double> gain;
        Frame ee_base;
        Property<Twist> end_twist;
        Double6D q_des;
        KinematicsComponent kineComp;
        Property<Double6D> q_err;

        Double6D q6;
        DataObjectInterface<Double6D>* jpos_DObj;

        Frame mp_base_frame;
        DataObjectInterface<Frame>* curFrame_DObj;

        Frame setpoint_frame;
        DataObjectInterface<Frame>* desFrame_DObj;

        Double6D q_dot;
        DataObjectInterface<Double6D>* jvel_DObj;
    };

}
#endif

