/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  CartesianNSGenerator.hpp 

                        CartesianNSGenerator.hpp -  description
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
 
 
#ifndef CARTESIAN_NS_GENERATOR_HPP
#define CARTESIAN_NS_GENERATOR_HPP

#include <geometry/frames.h>
#include <geometry/frames_io.h>

#include <geometry/trajectory_segment.h>
#include <geometry/path_line.h>
#include <geometry/path_point.h>
#include <geometry/velocityprofile_trap.h>
#include <geometry/velocityprofile_traphalf.h>
#include <geometry/velocityprofile_rect.h>
#include <geometry/velocityprofile_dirac.h>
#include <geometry/rotational_interpolation_sa.h>
#include <geometry/MotionProperties.hpp>
#include <corelib/HeartBeatGenerator.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/TemplateFactories.hpp"
#include <control_kernel/ExecutionExtension.hpp>
#endif

#include "CartesianNSDataObjects.hpp"

#pragma interface

/**
 * @file CartesianNSGenerator.hpp
 *
 * This file contains a Cartesian Generator doing interpolations.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif


    /**
     * @brief An advanced Cartesian Trajectory Generator.
     * @ingroup kcomps kcomp_generator
     */
    class CartesianGenerator 
        : public Generator<Expects<CartesianNSSensorInput>,
                                     Expects<CartesianNSModel>,
                                     Expects<CartesianNSCommand>,
                                     Writes<CartesianNSSetPoint>,
                                     MakeAspect<PropertyExtension, KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                   , ExecutionExtension
#endif
                                                   >::Result >
    {
        typedef Generator<Expects<CartesianNSSensorInput>,
                          Expects<CartesianNSModel>,
                          Expects<CartesianNSCommand>,
                          Writes<CartesianNSSetPoint>,
                          MakeAspect<PropertyExtension, KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                     , ExecutionExtension
#endif
                                     >::Result > Base;
    public:
        /**
         * Constructor.
         */
        CartesianGenerator(const std::string& name = string("CartesianGenerator") ) 
            : Base( name ),
              homepos("HomingPosition","The Homing Position.", Frame::Identity()),
              hometime("HomingTime","The Time in seconds a homing movement must take.", 10),
              timestamp(0), _time(0), interpol(0), cur_tr(0),
              task_frame(Frame::Identity()),
              tool_mp_frame(Frame::Identity()),
              max_vel("MaximumVelocity", "The Maximum Cartesian Velocity", 0 ),
              max_acc("MaximumAcceleration", "The Maximum Cartesian Acceleration", 0 ),
              interpol_prop("Interpolation", "One of Step, Linear, Trapezoidal.","Trapezoidal")
        {
            // Default Behaviour.
            interpolate( interpol_prop.get() );
        }

        ~CartesianGenerator()
        {
            delete interpol;
            delete cur_tr;
        }

        virtual bool componentStartup()
        {
            // startup the component, acquire all the data objects,
            // put sane defaults in the setpoint dataobject.
            // If no commands are defined, trajectories can not be loaded.
            // then only moveTo is functional.
            if ( Base::Command::dObj()->Get("Trajectory", traj_DObj) ) {
                Base::Command::dObj()->Get("ToolFrame", tool_f_DObj);
                Base::Command::dObj()->Get("TaskFrame", task_f_DObj);
            }
            if ( !Base::Model::dObj()->Get("EndEffPosition", mp_base_f_DObj) ||
                 !Base::SetPoint::dObj()->Get("EndEffectorFrame", end_f_DObj) )
                return false;

            // stay as is.
            mp_base_f_DObj->Get( mp_base_frame );
            end_f_DObj->Set( mp_base_frame );
            end_pos = mp_base_frame;

            // record the startup time. 
            timestamp = HeartBeatGenerator::Instance()->ticksGet();
            return true;
        }            
                
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()
        {
            // Always read the current position
            mp_base_f_DObj->Get( mp_base_frame );
            _time = HeartBeatGenerator::Instance()->secondsSince(timestamp);
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void calculate() 
        {
            if ( cur_tr )
                {
                    end_pos = task_frame * cur_tr->Pos(_time) * tool_mp_frame.Inverse();
                }
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()    
        {
            end_f_DObj->Set( end_pos );
        }
            
        bool trajectoryDone() const
        {
            if (cur_tr)
                return _time > cur_tr->Duration();
            return true;
        }
        
        /**
         * Returns the current position on the trajectory.
         */
        Frame currentPosition() const
        {
            return mp_base_frame;
        }

        /**
         * Returns the target position of the trajectory.
         * or the currentPosition() if no trajectory is present.
         */
        Frame targetPosition() const
        {
            if ( cur_tr )
                return cur_tr->Pos( cur_tr->Duration() + 1.0 ); // this +1.0 is for when Duration() == 0 (dirac)
            else 
                return currentPosition();
        }

        double currentTime() const
        {
            return _time;
        }

        /**
         * Wait for a moment on the last visited position.
         */
        bool wait( double time )
        {
            if ( this->kernel()->isRunning() && trajectoryDone() )
                {
                    Frame pos = targetPosition();
                    cout <<"Waiting for "<< time<<" seconds."<< endl;
                    delete cur_tr;
                    cur_tr = new Trajectory_Segment( new Path_Point( pos ),
                                                     interpol->Clone(), time );
                    timestamp = HeartBeatGenerator::Instance()->ticksGet();
                    _time = 0;
                    return true;
                }
            return false;
        }

        bool isAtEndPosition( Frame , double ) const
        {
            return trajectoryDone();
        }

        /**
         * Move from the last visited (or current if first)
         * position to a new position within a time.
         */
        bool moveTo( Frame new_pos, double time = 0 )
        {
            if ( this->kernel()->isRunning() && trajectoryDone() )
                {
                    Frame pos = targetPosition();
                    cout <<"MoveTo : from "<< pos <<" to "<<endl<< new_pos <<endl;
                    delete cur_tr;
                    cur_tr = new Trajectory_Segment( new Path_Line( pos, new_pos,
                                                                   new RotationalInterpolation_SingleAxis(),1.0 ),
                                                     interpol->Clone(), time );
                    timestamp = HeartBeatGenerator::Instance()->ticksGet();
                    _time = 0;
                    return true;
                }
            return false;
        }

        /**
         * Select the interpolation algorithm.
         */
        void interpolate(const std::string& algorithm )
        {
            if ( algorithm == "Trapezoidal" ) {
                delete interpol;
                interpol = new VelocityProfile_Trap( max_vel, max_acc);
            }
            else if ( algorithm == "Linear" ) {
                delete interpol;
                interpol =  new VelocityProfile_Rectangular( max_vel );
            }
            else if ( algorithm == "Step" ) {
                delete interpol;
                interpol =  new VelocityProfile_Dirac( );
            }
        }

        /**
         * Stop as fast as possible (with a_max).
         * EXPERIMENTAL
         */
        bool safeStop()
        {
            if ( this->kernel()->isRunning() && cur_tr )
                {
                    // assume vel and acc are positive...
                    //double current_v = cur_tr->GetProfile()->Vel( _time );
                    //double needed_time = current_v / max_acc;
                    // we will travel this distance until stopped :
                    // to be implemented :
                    //double needed_distance = current_v * needed_time
                    //    - max_acc*needed_time*needed_time;
                    //VelocityProfile_TrapHalf* vth = new VelocityProfile_TrapHalf( current_v, max_acc, false );
                    //vth->SetProfile(0, 1, needed_time); // this will stretch the '1' to the end position.
                    cur_tr =  new Trajectory_Segment( new Path_Line( mp_base_frame, cur_tr->Vel( _time ),
                                                                     new RotationalInterpolation_SingleAxis(),
                                                                     1.0), 
                                                      interpol->Clone() );
                    return true;
                }
            return false;
        }

        /**
         * Move with a certain velocity (translational and rotational)
         * for a period of time. EXPERIMENTAL
         */
        bool move( Twist end_twist, double time = 0 )
        {
            if ( this->kernel()->isRunning() && trajectoryDone() )
                {
                    Frame pos = targetPosition();
                    Trajectory* tr_copy = cur_tr;
                    // This is an approximation, should be delta_v / a_max.
                    if ( time == 0 )
                        time = max_vel.get() / max_acc.get();
                    Frame new_pos = pos;
                    new_pos.Integrate(end_twist, 1./time);
                    cout <<"Move : with twist "<< end_twist <<endl;
                    cur_tr = new Trajectory_Segment( new Path_Line(mp_base_frame, new_pos,
                                                                   new RotationalInterpolation_SingleAxis(),1.0 ),
                                                     interpol->Clone(), time );
                    delete tr_copy;
                    timestamp = HeartBeatGenerator::Instance()->ticksGet();
                    _time = 0;
                    return true;
                }
            return false;
        }

        /**
         * Read the trajectory stored in the command and 
         * use it for interpolation.
         */
        bool loadTrajectory()
        {
            if ( this->kernel()->isRunning() && trajectoryDone() && traj_DObj != 0 )
                {
                    /**
                     * First, be sure the given traj is not zero.
                     * Then, check if we are interpolating a traj or not.
                     * In wichever case we are, the new traj should start within
                     * a margin from the current generated position.
                     */
                    if (  traj_DObj->Get() != 0 && 
                          (( cur_tr !=0 &&
                             Equal( task_frame * cur_tr->Pos(_time) * tool_mp_frame.Inverse(), 
                                    task_f_DObj->Get() * traj_DObj->Get()->Pos(0) * tool_mp_frame.Inverse(),0.01) )
                           ||
                           ( cur_tr == 0 &&
                             Equal( mp_base_frame,
                                    task_f_DObj->Get() * traj_DObj->Get()->Pos(0) * tool_mp_frame.Inverse(),0.01))))
                        {
                            cout << "Load Trajectory"<<endl;
                            delete cur_tr;
                            // get a clone of the new trajectory (so we become the owner).
                            cur_tr = traj_DObj->Get()->Clone();
                            task_f_DObj->Get(task_frame);
                            tool_f_DObj->Get(tool_mp_frame);
                            timestamp = HeartBeatGenerator::Instance()->ticksGet();
                            _time = 0;
                            return true;
                        }
                    else
                        cout << "Trajectory not loaded"<<endl;
                }
            cout << "exit loadTrajectory()"<<endl;
            return false;
        }

        /**
         * Home to a predefined position.
         */
        bool home()
        {
            return this->moveTo( homepos, hometime);
        }

        bool isHomed() const
        {
            return trajectoryDone();
        }

        bool isTrajectoryLoaded() const
        {
            return cur_tr != 0;
        }

        void setHomeFrame( Frame _homepos )
        {
            homepos = _homepos;
        }

        virtual bool updateProperties( const PropertyBag& bag)
        {
            composeProperty( bag, max_vel );
            composeProperty( bag, max_acc );
            composeProperty( bag, interpol_prop);

            interpolate( interpol_prop.get() ) ; // re-init 

            composeProperty( bag, homepos );
            composeProperty( bag, hometime );
            return true;
        }

        virtual void exportProperties( PropertyBag& bag)
        {
            bag.add( &max_vel );
            bag.add( &max_vel );
            bag.add( &interpol_prop );
            bag.add( &homepos );
            bag.add( &hometime );
        }

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< CartesianGenerator >* ret =
                newDataSourceFactory( this );
            ret->add( "currentPosition", 
                      data( &CartesianGenerator::currentPosition, "The current position "
                            "of the robot." ) );
            ret->add( "targetPosition", 
                      data( &CartesianGenerator::targetPosition, "The target position "
                            "of the robot." ) );
            ret->add( "currentTime",
                      data( &CartesianGenerator::currentTime, 
                            "The current time in the movement "
                            ) );
            ret->add( "trajectoryDone",
                      data( &CartesianGenerator::trajectoryDone,
                            "The state of the current trajectory "
                            ) ); 
            return ret;
        }

        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< CartesianGenerator >* ret =
                newCommandFactory( this );
            ret->add( "moveTo", 
                      command( &CartesianGenerator::moveTo,
                               &CartesianGenerator::trajectoryDone,
                               "Move the robot to a position",
                               "Frame", "The Frame to move to.",
                               "Time", "The time the movement must take.") );
            ret->add( "wait", 
                      command( &CartesianGenerator::wait,
                               &CartesianGenerator::trajectoryDone,
                               "Wait on a certain position",
                               "Time", "The time that must be waited.") );
            ret->add( "home", 
                      command( &CartesianGenerator::home,
                               &CartesianGenerator::isHomed,
                               "Move the robot to its home position" ) );
            ret->add( "loadTrajectory",
                      command( &CartesianGenerator::loadTrajectory,
                               &CartesianGenerator::trajectoryDone,
                               "Load a new trajectory." ) );
            return ret;
        }

        MethodFactoryInterface* createMethodFactory()
        {
            TemplateMethodFactory< CartesianGenerator >* ret =
                newMethodFactory( this );
            ret->add( "setHomeFrame", 
                      method( &CartesianGenerator::setHomeFrame,
                            "The home position of the robot.", "Frame", "Homing End Frame" ) );
            return ret;
        }
#endif
    protected:
        Property<Frame> homepos;
        Property<double> hometime;
        HeartBeatGenerator::ticks timestamp;
        Seconds      _time;

        VelocityProfile* interpol;
        Trajectory*  cur_tr;
        DataObjectInterface<Trajectory*>* traj_DObj;

        Frame task_frame;
        DataObjectInterface<Frame>* task_f_DObj;

        Frame tool_mp_frame;
        DataObjectInterface<Frame>* tool_f_DObj;

        Frame end_pos;
        DataObjectInterface<Frame>* end_f_DObj;

        Frame mp_base_frame;
        DataObjectInterface<Frame>* mp_base_f_DObj;

        Property<double> max_vel;
        Property<double> max_acc;
        Property<std::string> interpol_prop;
    };

}
#endif

