 
#ifndef CARTESIAN_NS_GENERATOR_HPP
#define CARTESIAN_NS_GENERATOR_HPP

#include <geometry/frames.h>
#include <geometry/frames_io.h>

#include <geometry/trajectory_segment.h>
#include <geometry/path_line.h>
#include <geometry/path_point.h>
#include <geometry/velocityprofile_trap.h>
#include <geometry/velocityprofile_rect.h>
#include <geometry/velocityprofile_dirac.h>
#include <geometry/rotational_interpolation_sa.h>
#include <geometry/MotionProperties.hpp>
#include <corelib/HeartBeatGenerator.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include "CartesianNSDataObjects.hpp"

/**
 * @file CartesianNSGenerator.hpp
 *
 * This file contains a Cartesian Generator doing interpolations.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif


    /**
     * An advanced Cartesian Trajectory Generator.
     */
    template <class Base>
    class CartesianGenerator 
        : public Base
    {
    public:
        typedef CartesianNSCommand CommandDataObject;
        typedef CartesianNSSetPoint SetPointDataObject;
        typedef CartesianNSSensorInput InputDataObject;
        typedef CartesianNSModel ModelDataObject;

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

        virtual bool componentStartup()
        {
            // startup the component, acquire all the data objects,
            // put sane defaults in the setpoint dataobject.
            if ( !Base::Command::dObj()->Get("Trajectory", traj_DObj) ||
                 !Base::Command::dObj()->Get("ToolFrame", tool_f_DObj) ||
                 !Base::Command::dObj()->Get("TaskFrame", task_f_DObj) ||
                 !Base::Model::dObj()->Get("EndEffPosition", mp_base_f_DObj) ||
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

        double time() const
        {
            return _time;
        }

        /**
         * Wait for a moment on the last visited position.
         */
        bool wait( double time )
        {
            if ( kernel()->isRunning() && trajectoryDone() )
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
            if ( kernel()->isRunning() && trajectoryDone() )
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
         */
        void safeStop()
        {
            if ( kernel()->isRunning && cur_tr )
                {
                    // assume vel and acc are positive...
                    double current_v = cur_tr->getProfile()->Vel( _time );
                    double needed_time = current_v / max_acc;
                    // we will travel this distance until stopped :
                    double needed_distance = current_v * needed_time
                        - max_acc*needed_time*needed_time;
                    VelocityProfile_TrapHalf* vth = new VelocityProfile_TrapHalf( current_v, max_acc, false );
                    vth->SetProfile(0, 1, needed_time); // this will stretch the '1' to the end position.
                    cur_tr =  new Trajectory_Segment( new Path_Twist( mp_base_frame, cur_tr->Vel( _time ),
                                                                     new RotationalInterpolation_SingleAxis(),
                                                                     1.0), 
                                                      interpol->Clone() );
                    
                
        }

        /**
         * Move with a certain velocity (translational and rotational)
         * for a period of time.
         */
        bool move( Twist end_twist, double time = 0 )
        {
            if ( kernel()->isRunning() && trajectoryDone() )
                {
                    Frame pos = targetPosition();
                    Trajectory* tr_copy = cur_tr;
                    // This is an approximation, should be delta_v / a_max.
                    if ( time == 0 )
                        time = max_vel.get() / max_acc.get();
                    Frame new_pos = pos + end_twist * time;
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
        void loadTrajectory()
        {
            if ( kernel()->isRunning() && trajectoryDone() )
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
                        }
                    else cout << "Trajectory not loaded"<<endl;
                }
            cout << "exit loadTrajectory()"<<endl;
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

#ifdef OROPKG_EXECUTION_PROGRAM_PARSER

        DataSourceFactory* createDataSourceFactory()
        {
            TemplateDataSourceFactory< CartesianGenerator<Base> >* ret =
                newDataSourceFactory( this );
            ret->add( "currentPosition", 
                      data( &CartesianGenerator<Base>::currentPosition, "The current position "
                            "of the robot." ) );
            ret->add( "targetPosition", 
                      data( &CartesianGenerator<Base>::targetPosition, "The target position "
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

        template< class T >
        bool true_gen( T t ) const { return true; }

        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< CartesianGenerator<Base> >* ret =
                newCommandFactory( this );
            ret->add( "moveTo", 
                      command( &CartesianGenerator<Base>::moveTo,
                               &CartesianGenerator<Base>::trajectoryDone,
                               "Move the robot to a position",
                               "Frame", "The Frame to move to.",
                               "Time", "The time the movement must take.") );
            ret->add( "wait", 
                      command( &CartesianGenerator<Base>::wait,
                               &CartesianGenerator<Base>::trajectoryDone,
                               "Wait on a certain position",
                               "Time", "The time that must be waited.") );
            ret->add( "home", 
                      command( &CartesianGenerator<Base>::home,
                               &CartesianGenerator<Base>::isHomed,
                               "Move the robot to its home position" ) );
            ret->add( "setHomeFrame", 
                      command( &CartesianGenerator<Base>::setHomeFrame,
                               &CartesianGenerator<Base>::true_gen,
                            "The home position of the robot.", "Frame", "Homing End Frame" ) );
            ret->add( "loadTrajectory",
                      command( &CartesianGenerator<Base>::loadTrajectory,
                               &CartesianGenerator<Base>::isTrajectoryLoaded,
                               "Load a new trajectory." ) );
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

