#ifndef KINEMATIC_ESTIMATOR_HPP
#define KINEMATIC_ESTIMATOR_HPP

#include "KinematicsInterface.hpp"
#include <lrl/frames.h>
#include <corelib/PropertyComposition.hpp>
#include "KinematicsFactory.hpp"

namespace ControlKernel
{
    /**
     * This Estimator converts joint positions and velocities to
     * Cartesian positions and velocities for a certain kind of robot.
     * 
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
