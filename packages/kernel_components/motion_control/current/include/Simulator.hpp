/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Simulator.hpp 

                        Simulator.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <kindyn/KinematicsComponent.hpp>

namespace ORO_ControlKernel
{
    using ORO_CoreLib::Double6D;
    using ORO_Geometry::Twist;
    using ORO_Geometry::Frame;
    using ORO_KinDyn::KinematicsComponent;
    using ORO_KinDyn::KinematicsInterface;
    using ORO_KinDyn::Configuration;

    /**
     * A simulator for a 6DOF robot. One can set the twist or joint
     * velocity which must be integrated over a certain time. The resulting
     * joint positions can then be read out.
     *
     */
    class SimulatorInterface
    {
        public:
            virtual ~SimulatorInterface() {}
            /**
             * Sets the joint velocity over a certain time.
             *
             * @param qdot6 The 6 joint velocities.
             * @param time  The time period these must be applied.
             */
            virtual void setQDots(const Double6D& qdot6, double time) = 0;
            /**
             * Sets the end effector Twist over a certain time.
             *
             * @param t     The end effector twist.
             * @param time  The time period these must be applied.
             */
            virtual void setTwist(const Twist& t, double time) = 0;
            
            /**
             * Read the resulting joint positions.
             *
             * @return the resulting joint positions.
             */
            virtual Double6D& getJointPositions() = 0;
    };
            
    /**
     * A simple implementation of an integrating SimulatorInterface.
     */
    class Simulator
        :public SimulatorInterface
    {
        public:
            /**
             * Create a Simulator which uses a specified KinematicsInterface
             * implementation.
             */
            Simulator( KinematicsInterface* klib)
                : kineComp(klib) 
                {}
            
            void setStartFrame( const Frame& f, const Configuration& c )
            {
                kineComp.stateSet(f, c);
                frame = f;
                kineComp.jointsGet(q6);
                cout <<"Joints are now "<< q6[0]<<" " <<q6[1]<<endl;
            }
            
            void setStartPosition( const Double6D& q )
            {
                kineComp.stateSet(q);
                kineComp.jointsGet(q6);
            }
            
            virtual void setQDots(const Double6D& qdot6, double time)
            {
                q6 += qdot6 * time;
                kineComp.stateSet(q6);
            }
            
            virtual void setTwist(const Twist& t, double time)
            {
                //cout << t<<endl;
                if (kineComp.positionForward(q6, frame) )
                {
                    frame.Integrate(t, (1.0/time) );
                    //cout << frame <<endl;
                    if ( kineComp.positionInverse(frame, q6) )
                        return;
                    cout << "Inverse failed... ";
                }
                cout <<" Wrong setTwist"<<endl;
            }
            
            virtual Double6D& getJointPositions()
            { 
                kineComp.jointsGet(q6);
                return q6;
            }
        protected:
            Frame frame;
            Double6D q6;
            KinematicsComponent kineComp;
    };
}
#endif
