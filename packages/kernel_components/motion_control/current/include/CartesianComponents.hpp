/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CartesianComponents.hpp 

                        CartesianComponents.hpp -  description
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
 
 
#ifndef CARTESIAN_COMPONENTS_HPP
#define CARTESIAN_COMPONENTS_HPP

#include <geometry/frames.h>
#include <geometry/trajectory.h>
#include <geometry/trajectory_segment.h>
#include <geometry/path_line.h>
#include <geometry/velocityprofile_trap.h>
#include <geometry/rotational_interpolation_sa.h>
#include <geometry/MotionProperties.hpp>
#include <corelib/HeartBeatGenerator.hpp>
#include <kindyn/KinematicsComponent.hpp>
#include <kindyn/KinematicsFactory.hpp>
#include <corelib/EventInterfaces.hpp>
#include <kernel_components/Simulator.hpp>

    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
    using namespace ORO_KinDyn;

    /**
     * The CartesianGenerator generates these SetPoints.
     */
    struct CartesianSetPoints { Frame mp_base_frame; Frame task_frame; Double6D q6; };

    /**
     * The CartesianGenerator receives these commands.
     */
    struct CartesianCommands  { Trajectory* trajectory; Frame task_frame; Frame tool_mp_frame; bool home; };
    
    /**
     * A Simple Cartesian Generator.
     */
    template <class Base>
    class CartesianGenerator 
         : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::SetPointType SetPointType;
            typedef typename Base::InputType InputType;
            typedef typename Base::CommandType CommandType;
            typedef typename Base::ModelType ModelType;
            
            /**
             * Constructor.
             */
            CartesianGenerator() 
                 :end_pos("End Position","One of many variables which can be reported."),
                 timestamp(0), time(0), tr(0), onetime(false), task_frame(Frame::Identity()), tool_mp_frame(Frame::Identity()), restart(false)
            {}

                 /**
                  * @see KernelReportingExtension.hpp class ReportingComponent
                  */
                 virtual void exportReports(PropertyBag& bag)
                 {
                     bag.add(&end_pos);
                 }
                
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Base::Input::dObj()->Get(input);
                Base::Command::dObj()->Get(command);
                Base::Model::dObj()->Get(model);
                if ( command.home == true  && tr == 0)
                {
                    //cout <<"Home : from "<<command.task_frame<<" to "<< model.mp_base_frame<<endl;
                    // command this functionality
                    tr = new Trajectory_Segment( new Path_Line(model.mp_base_frame, command.task_frame, new RotationalInterpolation_SingleAxis(),1.0 ), new VelocityProfile_Trap(1,10),10.0);
                    task_frame = Frame::Identity(); //only used for storing the homing pos
                    tool_mp_frame = Frame::Identity();
                    restart = true;
                }
                if (tr == 0)
                {
                    tr = command.trajectory;
                    task_frame = command.task_frame;
                    tool_mp_frame = command.tool_mp_frame;
                    restart = true;
                }
                if (restart)
                {
                    timestamp = HeartBeatGenerator::Instance()->ticksGet();
                    restart = false;
                }
                time = HeartBeatGenerator::Instance()->secondsSince(timestamp);
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                if ( trajectDone() )
                {
                    if ( command.trajectory !=0 && Equal( task_frame * tr->Pos(time) * tool_mp_frame.Inverse(), 
                                command.task_frame * command.trajectory->Pos(0) * tool_mp_frame.Inverse(),0.01) )
                    {
                        //cout << "loading..."<<endl;
                        tr = command.trajectory;
                        task_frame = command.task_frame;
                        timestamp = HeartBeatGenerator::Instance()->ticksGet();
                        time = 0;
                    }
                }
                result.task_frame = task_frame;
                end_pos = result.mp_base_frame = task_frame * tr->Pos(time) * tool_mp_frame.Inverse();
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Base::SetPoint::dObj()->Set(result);
            }
            
            bool trajectDone()
            {
                if (tr)
                    return time > tr->Duration();
                return true;
            }

            void setTrajectory(Trajectory* _t)
            {
                tr = _t;
            }
            
        protected:
            Property<Frame> end_pos;
            InputType    input;
            CommandType  command;
            ModelType    model;
            SetPointType result;
            HeartBeatGenerator::ticks timestamp;
            Seconds      time;
            Trajectory*  tr;
            bool onetime;
            Frame task_frame;
            Frame tool_mp_frame;
            bool restart;
    };
    
    // one Model :
    struct CartesianModel { Frame mp_base_frame; };
    
    /**
     * A Cartesian Estimator
     */
    template <class Base>
    class CartesianEstimator
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::InputType InputType;
            typedef typename Base::ModelType ModelType;
            
            CartesianEstimator() 
                : kineName("Kinematics","The name of the KinematicsStub to use","Kuka361"), kine(0),kineComp(0)
            {
                kine = KinematicsFactory::create( kineName );
                if (kine)
                    kineComp = new KinematicsComponent( kine );
            }

            virtual ~CartesianEstimator()
            {
                if (kine) 
                {
                    delete kine;
                    delete kineComp;
                }
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Base::Input::dObj()->Get(input);
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                if ( !kineComp->positionForward(input.q6, model.mp_base_frame) )
                {
                    EventOperationInterface* es = EventOperationInterface::nameserver.getObjectByName("SingularityDetected");
                    if ( es != 0 )
                        es->fire();
                }
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Base::Model::dObj()->Set(model);
            }
            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
            }

            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
                if ( composeProperty(bag, kineName ) )
                {
                    kine = KinematicsFactory::create( kineName );
                    if (kine)
                    {
                        kineComp = new KinematicsComponent( kine );
                        return true;
                    }
                }
                return false;
            }

        protected:
            Property<string> kineName;
            KinematicsInterface* kine;
            KinematicsComponent* kineComp;
            ModelType model;
            InputType input;
    };

    // Send velocities to the drives :
    struct CartesianDriveOutputs { Twist mp_base_twist; Double6D q_dot; };
    
    /**
     * A Cartesian Controller
     */
    template <class Base>
    class CartesianController
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::SetPointType SetPointType;
            typedef typename Base::InputType InputType;
            typedef typename Base::ModelType ModelType;
            typedef typename Base::OutputType OutputType;
            
            CartesianController(KinematicsInterface* k) 
                : gain("Gain","The error gain.",0),
                  end_twist("Result Twist",""), kineComp(k), q_err("Velocity Setpoints","")
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Base::Input::dObj()->Get(input);
                Base::Model::dObj()->Get(model);
                Base::SetPoint::dObj()->Get(setpoint);
                kineComp.stateSet( input.q6 );
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                // OK FOR ROTATIONAL PART OF TWIST :
                ee_base = model.mp_base_frame;
                //ee_base.p = Vector::Zero();
                
                kineComp.positionInverse(setpoint.mp_base_frame, q_des );
                q_err = q_des - input.q6; 
                output.q_dot = q_err.value() * gain ;
                // ( goal_frame, current_frame )
#if 0
                output.mp_base_twist.vel = FrameDifference(setpoint.mp_base_frame, model.mp_base_frame).vel*gain;
                output.mp_base_twist.rot = (ee_base * FrameDifference(setpoint.mp_base_frame, model.mp_base_frame)).rot*gain;
                end_twist = output.mp_base_twist;
#endif
                //output.mp_base_twist.vel = Vector::Zero();
                //output.mp_base_twist = ee_base * output.mp_base_twist;
                //output.mp_base_twist.vel += FrameDifference(setpoint.mp_base_frame, model.mp_base_frame).vel;
                //output.mp_base_twist = ee_base * FrameDifference(setpoint.mp_base_frame, model.mp_base_frame);
                
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Base::Output::dObj()->Set(output);
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
            ModelType model;
            InputType input;
            SetPointType setpoint;
            OutputType output;
            Frame ee_base;
            Property<Twist> end_twist;
            Double6D q_des;
            KinematicsComponent kineComp;
            Property<Double6D> q_err;
    };

    /**
     * A Cartesian Effector, can be used by the CartesianSensor for 'simulation' purposes.
     */
    template <class Base>
    class CartesianEffector
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::OutputType OutputType;
            
            CartesianEffector(SimulatorInterface* _sim) 
                : endTwist("Twist","The End Effector twist"), sim(_sim)
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Base::Output::dObj()->Get(output);
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
                    sim->setQDots(output.q_dot, 0.05);
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
            OutputType output;
            SimulatorInterface* sim;
    };

    // Sensor measures positions :
    struct CartesianSensorInputs { Double6D q6; };

    /**
     * A Fake Cartesian Sensor measuring all data sent by the CartesianEffector.
     */
    template <class Base>
    class CartesianSensor
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::InputType InputType;
            
            CartesianSensor(SimulatorInterface* _sim = 0) 
                 :q6("JointPositions",""), sim(_sim)
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                /*
                 * Fake physical sensor data.
                 */
                if (sim != 0)
                    input.q6 = sim->getJointPositions();
                else
                    input.q6 = 0;
            }

            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Base::Input::dObj()->Set(input);
                q6 = input.q6;
            }

            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
                bag.add(&q6);
            }

            
        protected:
            Property<Double6D> q6;
            InputType input;
            SimulatorInterface* sim;
    };

}
#endif

