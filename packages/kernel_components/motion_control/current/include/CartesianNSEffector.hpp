/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  CartesianNSEffector.hpp 

                        CartesianNSEffector.hpp -  description
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
 
#ifndef CARTESIAN_NS_EFFECTOR_HPP
#define CARTESIAN_NS_EFFECTOR_HPP

#include <geometry/frames.h>
#include <geometry/MotionProperties.hpp>
#include <kernel_components/Simulator.hpp>
#include <corelib/MultiVector.hpp>


#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include "CartesianNSDataObjects.hpp"


/**
 * @file CartesianNSEffector.hpp
 * Contains a dummy Effector which uses a 
 * simulated robot.
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
     * @brief A Cartesian Effector, can be used by the CartesianSensor for 'simulation' purposes.
     * @ingroup kcomps kcomp_effector
     */
    template <class Base>
    class CartesianEffector
        : public Base
    {
    public:
        typedef CartesianNSDriveOutput OutputDataObject;

        /**
         * Necessary typedefs.
         */
        typedef typename Base::OutputType OutputType;
            
        CartesianEffector(SimulatorInterface* _sim) 
            :  Base("CartesianEffector"),endTwist("Twist","The End Effector twist"), sim(_sim)
        {}

        virtual bool componentStartup()
        {
            if ( !Base::Output::dObj()->Get("JointVelocities",qdot_DObj) )
                return false;
            return true;
        }

        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()      
        {
            qdot_DObj->Get( q_dot );
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            /*
             * Acces device drivers
             */
            if (sim !=0)
                sim->setQDots(q_dot, 0.05);
            //endTwist = output.mp_base_twist;
        }
            
        /**
         * @see KernelReportingExtension.hpp class ReportingComponent
         */
        virtual void exportReports(PropertyBag& bag)
        {
            bag.add(&endTwist);
        }

    protected:
        Property<Twist> endTwist;
        SimulatorInterface* sim;

        Double6D q_dot;
        DataObjectInterface<Double6D>* qdot_DObj;
    };

}
#endif

