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
#include <kindyn/KinematicsComponent.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/KinematicProcess.hpp>
#include <corelib/PropertyComposition.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <control_kernel/ExecutionExtension.hpp>
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include "CartesianDataObjects.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

/**
 * @file CartesianController.hpp
 *
 * This file contains a Controller calculating
 * the reference velocities for 6 axes.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
    using namespace ORO_KinDyn;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif

    /**
     * @brief A Cartesian Controller which converts a Frame setpoint
     * to joint positions and does the control on joint level.
     * @ingroup kcomps kcomp_controller
     */
    class CartesianController
        : public Controller<Expects<CartesianSensorInput>,
                            Expects<CartesianModel>,
                            Expects<CartesianSetPoint>,
                            Writes<CartesianDriveOutput>,
                            MakeFacet<PropertyExtension, KernelBaseFunction,
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                      ExecutionExtension,
#endif
                                      ReportingExtension>::Result >
    {
        typedef Controller<Expects<CartesianSensorInput>,
                           Expects<CartesianModel>,
                           Expects<CartesianSetPoint>,
                           Writes<CartesianDriveOutput>,
                           MakeFacet<PropertyExtension,
                                     KernelBaseFunction,
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                     ExecutionExtension,
#endif
                                     ReportingExtension>::Result > Base;
    public:
            
        CartesianController(std::string name = "CartesianController") 
            :  Base( name ),gain("Gain","The error gain.",0),
               end_twist("Result Twist",""), q_err("Velocity Setpoints","")
        {
        }

        virtual bool componentLoaded() {
            Logger::In in("CartesianController");
            KinematicProcess* kProc = kernel()->getProcess<KinematicProcess>("Kinematics");
            if ( kProc == 0 ) {
                Logger::log() << Logger::Error << "Could not find KinematicProcess with name \"Kinematics\" for kinematic algorithms."<<Logger::nl;
                Logger::log() << "Load the KinematicProcess Component first."<<Logger::endl;
                return false;
            }
            Logger::log() << Logger::Info << "Found KinematicProcess for kinematic algorithms."<<Logger::endl;
            kineComp = kProc->getKinematics();
            if ( kineComp ) {
                Logger::log() << Logger::Info << "Using kinematic algorithms for "<< kProc->getArchitecture() <<Logger::endl;
                jpos.resize(kineComp->getNumberOfJoints(),0.0);
                q_err.value().resize(kineComp->getNumberOfJoints(), 0.0);
                q_dot.resize(kineComp->getNumberOfJoints(), 0.0);
                q_des.resize(kineComp->getNumberOfJoints(), 0.0);
                return true;
            }
            Logger::log() << Logger::Error << "KinematicProcess not properly configured. Aborting." <<Logger::endl;
            return false;
        }

        virtual bool componentStartup()
        {
            // Resolve the DataObjects :
            if ( !Input->dObj()->Get("JointPositions", jpos_DObj) ||
                 !Model->dObj()->Get("EndEffPosition", curFrame_DObj) ||
                 !SetPoint->dObj()->Get("EndEffectorFrame", desFrame_DObj) ||
                 !Output->dObj()->Get("JointVelocities", jvel_DObj) )
                return false;
            return true;
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()      
        {
            jpos_DObj->Get( jpos );
            curFrame_DObj->Get( mp_base_frame );
            desFrame_DObj->Get( setpoint_frame );
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void calculate() 
        {
            // OK FOR ROTATIONAL PART OF TWIST :
            ee_base = mp_base_frame;
            //ee_base.p = Vector::Zero();
                
            if (kineComp->positionInverse(setpoint_frame, q_des ) == false) {
                for (unsigned int i = 0; i < jpos.size(); ++i) {
                    q_dot[i] = 0.0;
                }
                return; // emitting an event would be far more sane here.
            }
                
            for (unsigned int i = 0; i < jpos.size(); ++i) {
                q_err.value()[i] = q_des[i] - jpos[i]; 
                q_dot[i] = q_err.value()[i] * gain;
            }
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
        ORO_KinDyn::JointPositions q_des;
        KinematicsComponent* kineComp;
        Property<ORO_KinDyn::JointPositions> q_err;

        ORO_KinDyn::JointPositions jpos;
        DataObjectInterface<ORO_KinDyn::JointPositions>* jpos_DObj;

        Frame mp_base_frame;
        DataObjectInterface<Frame>* curFrame_DObj;

        Frame setpoint_frame;
        DataObjectInterface<Frame>* desFrame_DObj;

        ORO_KinDyn::JointVelocities q_dot;
        DataObjectInterface<ORO_KinDyn::JointVelocities>* jvel_DObj;

    };

}
#endif

