/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  KinematicEstimator.hpp 

                        KinematicEstimator.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
 
#ifndef KINEMATIC_ESTIMATOR_HPP
#define KINEMATIC_ESTIMATOR_HPP

#include "KinematicsInterface.hpp"
#include <geometry/frames.h>
#include <corelib/PropertyComposition.hpp>
#include "kindyn/KinematicsFactory.hpp"

namespace ORO_ControlKernel
{
    using ORO_CoreLib::Double6D;
    using ORO_Geometry::Twist;
    using ORO_Geometry::Frame;
    using ORO_KinDyn::KinematicsComponent;
    using ORO_KinDyn::KinematicsInterface;

    /**
     * @brief This Estimator converts joint positions and velocities to
     * Cartesian positions and velocities for a certain kind of robot. (EXP)
     *
     * It reads joint positions from the inputs and puts a Frame in the
     * model. EXPERIMENTAL
     *
     * @param BaseComponent The DefaultEstimator class of your kernel.
     */
    template< class BaseComponent>
    class KinematicEstimator
        : public BaseComponent
    {
        public:
            /**
             * Constructor.
             * 
             * @param _k The kinematic algorithm to use.
             */
            KinematicEstimator(KinematicsInterface* _k = 0) 
                :kine(_k), kineName("KineType","The type of Robot to use for kinematics calculations")
            {}

            void pull()
            {
                if (kine == 0)
                    return;

                q6 = BaseComponent::Inputs::dObj->Get().q6;
                q_dot6 = BaseComponent::Inputs::dObj->Get().q_dot6;
            }

            void calculate()
            {
                if (kine)
                {
                    f = kine->velocityForward(q6, q_dot6, f, t);
                }
            }

            void push()
            {
                BaseComponent::Model::dObj->Set().base_frame = f;
                BaseComponent::Model::dObj->Set().base_twist = t;
            }

            bool updateProperties(PropertyBag& bag)
            {
                 return composeProperty(bag, kineName);
            }

            void setKinematics( const std::string& name )
            {
                KinematicsInterface* _k;
                if ( (_k = KinematicsFactory::create(name)) != 0 )
                    kine = k;
            }
            
        protected:
            Property<std::string> kineName;

            Frame f;
            Twist t;
            KinematicsInterface* kine;
    };
}
#endif
