#ifndef TRAJECTORYGENERATOR_HPP
#define TRAJECTORYGENERATOR_HPP

#include <geometry/trajectory.h>
#include <geometry/frames.h>
#include <corelib/PropertyComposition.hpp>

namespace ControlKernel
{
    using namespace ORO_Geometry;

    /**
     * This Generator reads a Trajectory in the Task Frame and converts
     * it to the robot frame. It can read a Trajectory from file or
     * can use an existing Trajectory object.
     *
     * It will only read a new trajectory when the previous is done.
     * 
     */
    template< class Base>
    class TrajectoryGenerator
        : public Base
    {
        public:
        typedef typename Base::Command Commands;
        typedef typename Base::SetPoint SetPoints;
        typedef typename Base::SetPointType SetPointType;
            TrajectoryGenerator() 
                : filename("FileName","Traject File Name"), 
                  repeat("CyclicTrajectory","True if current trajectory needs to be cyclicly repeated", false),
                  refreshFile("RefreshTrajectory", "True if trajectory needs to be re-read from file in each startup", true),
                  acceptCommands("AcceptCommands", "True if the Generator accepts trajectories through the Commands Data Object", true),
                  task_base_frame("TaskRobotFrame", "The Task Frame expressed in the Robot Base Frame"),
                  robot_world_frame("RobotWorldFrame", "The Robot Frame expressed in the World Frame"),
                  task_world_frame("TaskWorldFrame", "The Task Frame expressed in the World Frame"),
                  home_task_frame("HomeTaskFrame", "The Home Position of the Robot in the Task Frame"),
                  tr(0), time_stamp(0), time(0), run(false)
            {}

            void pull()
            {
                if (!run)
                    return;

                if ( acceptCommands && !repeat && trajectDone() )
                {
                    // Read new traject when done with old.
                    typename Base::CommandType new_com;
                    Commands::dObj()->Get(new_com);
                    tr = new_com.trajectory;
                    time_stamp = HeartBeatGenerator::Instance()->ticksGet();
                } else 
                    if (repeat && trajectDone() )
                {
                    // Repeat current traject (dangerous !)
                    time_stamp = HeartBeatGenerator::Instance()->ticksGet();
                }

                time = HeartBeatGenerator::Instance()->secondsGet(time_stamp);
            }

            void calculate()
            {
                if (run)
                {
                    fr = task_base_frame.get() * tr->Pos( time );
                    tw = task_base_frame.get() * tr->Vel( time );
                }
            }

            void push()
            {
                result.mp_base_frame = fr;
                result.mp_base_twist = tw;
                SetPoints::dObj()->Set(result);
            }

            bool updateProperties(PropertyBag& bag)
            {
                if (refreshFile)
                    if ( composeProperty(bag, filename) )
                    {
                        std::ifstream f( filename.c_str() );
                        tr = Trajectory::Read(f);
                        composeProperty(bag, repeat);
                        composeProperty(bag, acceptCommands);
                        composeProperty(bag, repeat);
                        if (!composeProperty(bag, refreshFile))
                            refreshFile = false;
                        return true;
                        fr = tr->Pos(0);
                        tw = tr->Vel(0);
                    }
                return false;
            }

            void setTaskInBaseFrame( const Frame& f )
            {
                task_base_frame = f;
                task_world_frame = robot_world_frame * task_base_frame;
            }

            void setTaskInWorldFrame( const Frame& f )
            {
                task_world_frame = f;
                task_base_frame  = robot_world_frame.Inverse() * task_world_frame;
            }

            void setRobotInWorldFrame( const Frame& f )
            {
                robot_world_frame = f;
                task_base_frame  = robot_world_frame.Inverse() * task_world_frame;
            }

            void resetTime( double newTime = 0)
            {
                time_stamp = HeartBeatGenerator::Instance()->ticksGet() - HeartBeatGenerator::Instance()->nano2ticks(newTime);
            }

            /**
             * Scales the execution time of the traject with X percent.
             * The scaling will be limited with the current trajectory's
             * limits. 
             * 
             * @param percent A percentage inidicating the relative new trajectory duration.
             */
            void timeScale( double percent )
            {
                MotionProfile* t_prof = t->GetProfile();
                double oldDur = t_prof->Duration();
                double newDur = percent / 100.0 * oldDur;
                
                t_prof->SetDuration(t_prof->Pos(0), t_prof->Pos(oldDur), newDur);
                
                fact  = t_prof->Duration() / oldDur;
                time *= fact;
                resetTime( time );
            }

            void start()
            {
                if (run)
                    return;
                
                if (acceptCommands)
                    tr = Commands::dObj->get().trajectory;
                if (tr)
                    run = true;
                
                time_stamp = HeartBeatGenerator::Instance()->ticksGet();
            }

            void stop()
            {
                run = false;
            }

            bool trajectDone()
            {
                if (tr)
                    return time > tr->Duration();
                return false;
            }

            bool setTrajectory(ORO_Geometry::Trajectory* _t)
            {
                if (!run)
                {
                    tr = _t;
                    return true;
                }
                return false;
            }
            
        protected:
            Property<std::string> filename;
            Property<bool>        repeat;
            Property<bool>        refreshFile;
            Property<bool>        acceptCommands;
            Property<Frame>       task_base_frame;
            Property<Frame>       robot_world_frame;
            Property<Frame>       task_world_frame;
            Property<Frame>       home_task_frame;

            SetPointType result;
            Frame fr;
            Twist tw;
            const Trajectory* tr;
            HeartBeatGenerator::ticks time_stamp;
            double time;
            bool run;
    };
}
#endif
