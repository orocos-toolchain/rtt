/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CartesianNSComponents.hpp 

                        CartesianNSComponents.hpp -  description
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
#include <geometry/frames_io.h>
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

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif


/**
 * @file CartesianNSComponents.hpp
 *
 * This file contains some 'example' components for
 * use in cartesian path planning. Mainly Nameserved
 * DataObjects are used, but it demonstrates that
 * they can be mixed with classical DataObjects.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
    using namespace ORO_KinDyn;
    using namespace ORO_Execution;

    struct CartesianCommands     { Trajectory* trajectory; Frame task_frame; Frame tool_mp_frame; };

    /**
     * This class represents twice the same data types, once nameserved
     * and once not nameserved. It is intended to show how an equivalent functionality
     * can be reached in the NSKernel. (ServedTypes and UnServedTypes are
     * representing different data, they are not copies or links to each other.)
     */
    struct CartNSCommands
        : public ServedTypes< Trajectory*, Frame>, public UnServedType< CartesianCommands >
    {
        CartNSCommands()
        {
            // The numbers 0,1 is the type number as in
            // ServedTypes< Type0, Type1, Type2,... >
            this->insert( make_pair(0, "Trajectory"));
            this->insert( make_pair(1, "TaskFrame"));
            this->insert( make_pair(1, "ToolFrame"));
        }
    };
 
    struct CartesianSetPoints { Frame mp_base_frame; Frame task_frame; Double6D q6; };

    /**
     * Specifying an empty ServedTypes allows you to be compatible
     * with components written for the DefaultControlKernel . No nameserving is done.
     * This still can only used in the NSControlKernel.
     */
    struct CartNSSetPoints
        : public ServedTypes<>, public UnServedType< CartesianSetPoints >
    {
    };

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
             timestamp(0), _time(0), cur_tr(0), task_frame(Frame::Identity()), tool_mp_frame(Frame::Identity())
        {}

        /**
         * @see KernelReportingExtension.hpp class ReportingComponent
         */
        virtual void exportReports(PropertyBag& bag)
        {
            bag.add(&end_pos);
        }

        virtual bool componentStartup()
        {
            // startup the component, acquire all the data objects,
            // put sane defaults in the setpoint dataobject.
            if ( !Base::Command::dObj()->Get(string("Trajectory"), com_tr) )
                return false;
            Base::Input::dObj()->Get(input);
            Base::Command::dObj()->Get(_command);
            Base::Model::dObj()->Get(model);

            // stay as is.
            result.mp_base_frame = model.mp_base_frame;
            //cout << "Result :"<< result.mp_base_frame<<endl;
            Base::SetPoint::dObj()->Set(result);

            // record the startup time. 
            timestamp = HeartBeatGenerator::Instance()->ticksGet();
            return true;
        }            
                
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()
        {
            Base::Command::dObj()->Get( _command );
            Base::Model::dObj()->Get(model);
            mp_base_frame = model.mp_base_frame;
            _time = HeartBeatGenerator::Instance()->secondsSince(timestamp);
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void calculate() 
        {
            if ( cur_tr )
                {
                    result.task_frame = task_frame;
                    end_pos = result.mp_base_frame = task_frame * cur_tr->Pos(_time) * tool_mp_frame.Inverse();
                }
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            Base::SetPoint::dObj()->Set(result);
        }
            
        bool trajectoryDone()
        {
            if (cur_tr)
                return _time > cur_tr->Duration();
            return true;
        }
        
        Frame position()
        {
            return result.mp_base_frame;
        }

        double time()
        {
            return _time;
        }

        /**
         * Read the trajectory stored in the command and 
         * use it for interpolation.
         */
        void loadTrajectory()
        {
            if ( kernel()->isRunning() && trajectoryDone() )
                if ( _command.trajectory !=0 && 
                     Equal( task_frame * cur_tr->Pos(_time) * tool_mp_frame.Inverse(), 
                            _command.task_frame * _command.trajectory->Pos(0) * tool_mp_frame.Inverse(),0.01) )
                    {
                        cout << "Load Trajectory"<<endl;
                        // get new trajectory
                        cur_tr = _command.trajectory;
                        task_frame = _command.task_frame;
                        tool_mp_frame = _command.tool_mp_frame;
                        timestamp = HeartBeatGenerator::Instance()->ticksGet();
                        _time = 0;
                    }
                else cout << "Trajectory not loaded"<<endl;
            cout << "exit loadTrajectory()"<<endl;
        }

        /**
         * Home to a predefined position.
         */
        void home()
        {
            // XXX insert proper delete code.
            if ( kernel()->isRunning() && trajectoryDone() )
                {
                    cout <<"Home : from "<< model.mp_base_frame <<" to "<<_command.task_frame<<endl;
                    _time = 0;
                    cur_tr = new Trajectory_Segment( new Path_Line(mp_base_frame, _command.task_frame, new RotationalInterpolation_SingleAxis(),1.0 ), new VelocityProfile_Trap(1,10),10.0);
                    task_frame = Frame::Identity(); //only used for storing the homing pos
                    tool_mp_frame = Frame::Identity();
                    timestamp = HeartBeatGenerator::Instance()->ticksGet();
                }
        }

        bool isHomed()
        {
            return trajectoryDone();
        }

        bool isTrajectoryLoaded()
        {
            return cur_tr != 0;
        }

        DataSourceFactory* createDataSourceFactory()
        {
            TemplateDataSourceFactory< CartesianGenerator<Base> >* ret =
                newDataSourceFactory( this );
            ret->add( "position", 
                      data( &CartesianGenerator<Base>::position, "The current position "
                            "of the robot." ) );
            ret->add( "time",
                      data( &CartesianGenerator<Base>::time, 
                            "The current time in the movement "
                            ) );
            ret->add( "trajectoryDone",
                      data( &CartesianGenerator<Base>::trajectoryDone,
                            "The state of the current trajectory "
                            ) ); 
            return ret;
        }

        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< CartesianGenerator<Base> >* ret =
                newCommandFactory( this );
            ret->add( "home", 
                      command( &CartesianGenerator<Base>::home,
                               &CartesianGenerator<Base>::isHomed,
                               "Move the robot to its home position" ) );
            ret->add( "loadTrajectory",
                      command( &CartesianGenerator<Base>::loadTrajectory,
                               &CartesianGenerator<Base>::isTrajectoryLoaded,
                               "Load a new trajectory." ) );
            return ret;
        }
    protected:
        Property<Frame> end_pos;
        InputType    input;
        CommandType  _command;
        ModelType    model;
        SetPointType result;

        HeartBeatGenerator::ticks timestamp;
        Seconds      _time;

        Trajectory*  cur_tr;
        Trajectory*  com_tr;

        Frame task_frame;
        Frame tool_mp_frame;
        Frame mp_base_frame;
    };
    
    // one Model :
    struct CartesianModel { Frame mp_base_frame; };

    /**
     * Specifying an empty ServedTypes allows you to be compatible
     * with the DefaultControlKernel implementation. No nameserving is done.
     */
    struct CartNSModel
        : public ServedTypes<>, public UnServedType< CartesianModel >
    {};

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

        virtual bool componentStartup()
        {
            pull();
            if ( !kineComp->positionForward(input.q6, model.mp_base_frame) )
                return false;
            push();
            return true;
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
 * Specifying an empty ServedTypes allows you to be compatible
 * with the DefaultControlKernel implementation. No nameserving is done.
 */
struct CartNSDriveOutputs
    : public ServedTypes<>, public UnServedType< CartesianDriveOutputs >
{};

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
     * Specifying an empty ServedTypes allows you to be compatible
     * with the DefaultControlKernel implementation. No nameserving is done.
     */
    struct CartNSSensorInputs
        : public ServedTypes<>, public UnServedType< CartesianSensorInputs >
    {};

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
            
        virtual bool componentStartup()
        {
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

