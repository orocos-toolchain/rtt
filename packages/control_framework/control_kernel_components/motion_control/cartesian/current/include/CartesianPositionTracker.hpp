/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  CartesianPositionTracker.hpp 

                        CartesianPositionTracker.hpp -  description
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
 
#ifndef CARTESIAN_POSITION_TRACKER_HPP
#define CARTESIAN_POSITION_TRACKER_HPP

#include <geometry/frames.h>
#include <geometry/frames_io.h>
#include <geometry/trajectory.h>
#include <geometry/trajectory_segment.h>
#include <geometry/path_line.h>
#include <geometry/velocityprofile_trap.h>
#include <geometry/velocityprofile_rect.h>
#include <geometry/velocityprofile_dirac.h>
#include <geometry/rotational_interpolation_sa.h>
#include <corelib/TimeService.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <control_kernel/ExecutionExtension.hpp>
#include "execution/TemplateFactories.hpp"
#endif


/**
 * @file CartesianPositionTracker.hpp
 *
 * This file contains Cartesian Generator Trackers for
 * use in cartesian path planning. Only Nameserved
 * DataObjects are used.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif

    /**
     * @brief The Nameserved Command DataObjects.
     */
    struct CPTCommand
        : public ServedTypes< Frame, double >
    {
        CPTCommand()
        {
            // The numbers 0,1 is the type number as in
            // ServedTypes< Type0, Type1, Type2,... >
            this->insert( make_pair(0, "TaskFrame"));
            this->insert( make_pair(0, "ToolFrame"));
            this->insert( make_pair(0, "TrackFrame"));
            this->insert( make_pair(1, "TrackTime"));
        }
    };
 
    /**
     * The SetPoint is expressed in the robot base frame.
     */
    struct CPTSetPoint
        : public ServedTypes<Frame>
    {
        CPTSetPoint()
        {
            this->insert( make_pair(0, "EndEffectorFrame"));
        }
    };

    
    /**
     * The Estimator estimates the current cartesian position
     * and can provide a tracking position.
     */
    struct CPTModel
        : public ServedTypes<Frame, double>
    {
    };


    /**
     * Sensor tracking is only possible if the Sensor
     * provides a positional Frame.
     */
    struct CPTSensorInput
        : public ServedTypes<Frame, double>
    {
    };

#ifdef ORO_PRAGMA_INTERFACE
ORO_PRAGMA_INTERFACE
#endif
    
    /**
     * @brief A Cartesian Position Frame / TaskFrame Tracker.
     *
     * This Generator can track an externally generated Frame
     * and adapt to an externally generated TaskFrame.
     *
     * @ingroup kcomps kcomp_generator
     */
    class CartesianPositionTracker
        : public Generator< Expects<CPTSensorInput>,
                                      Expects<CPTModel>,
                                      Expects<CPTCommand>,
                                      Writes<CPTSetPoint>,
                                      MakeFacet<PropertyExtension,KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                    , ExecutionExtension
#endif
                                                    >::CommonBase >
    {
        typedef Generator< Expects<CPTSensorInput>,
                           Expects<CPTModel>,
                           Expects<CPTCommand>,
                           Writes<CPTSetPoint>,
                           MakeFacet<PropertyExtension,KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                      , ExecutionExtension
#endif
                                      >::Result > Base;
    public:
        /**
         * Constructor.
         */
        CartesianPositionTracker(const std::string& name = string("CartesianPositionTracker") ) 
            : Base( name ),
              tracking(false),
              task_tracking(false),
              timestamp(0), _time(0),
              interpol(0), cur_tr(0),
              tool_mp_frame(Frame::Identity()),
              task_frame(Frame::Identity()),
              closeness("Closeness", "The minimal difference in tracking setpoints to trigger a new trajectory generation.", 0.001 ),
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
            if ( !Model->dObj()->Get("EndEffPosition", mp_base_f_DObj) ||
                 !SetPoint->dObj()->Get("EndEffectorFrame", end_f_DObj) )
                return false;

            // stay as is.
            mp_base_f_DObj->Get( mp_base_frame );
            end_f_DObj->Set( mp_base_frame );
            end_pos = mp_base_frame;

            // record the startup time. 
            timestamp = TimeService::Instance()->getTicks();
            return true;
        }            
                
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()
        {
            // Always read the current position
            mp_base_f_DObj->Get( mp_base_frame );

            // Track the Task Frame ?
            if (task_tracking)
                task_f_DObj->Get( task_frame );

            // Track a DataObject ?
            if (tracking && this->trajectoryDone() )
                {
                    Frame track_peeker;
                    track_f_DObj->Get( track_peeker );
                    // If the difference is big enough between old and new track Frame,
                    // create a new trajectory. Otherwise, remain on the same spot.
                    if ( !Equal(track_peeker, track_frame, closeness ) )
                        {
                            track_frame = track_peeker;
                            track_t_DObj->Get( track_time  );
                            delete cur_tr;
                            cout <<"Tracking : "<< track_frame << endl;
                            cur_tr = new Trajectory_Segment( new Path_Line(mp_base_frame, track_frame,
                                                                           new RotationalInterpolation_SingleAxis(),1.0 ),
                                                             interpol->Clone(), track_time );
                            timestamp = TimeService::Instance()->getTicks();
                            _time = 0;
                        }
                }

            _time = TimeService::Instance()->secondsSince(timestamp);
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

        /**
         * @name Cartesian Position Tracking commands.
         * @{
         */
            
        /** 
         * @brief Inspect if the current trajectory is finished.
         * 
         * @return true if so, false otherwise
         */
        bool trajectoryDone() const
        {
            if (cur_tr)
                return _time > cur_tr->Duration();
            return true;
        }
        
        /** 
         * @brief Return the current Cartesian position.
         * 
         * 
         * @return The current position.
         */
        Frame position() const
        {
            return mp_base_frame;
        }

        /** 
         * @brief Return the current elapsed movement time
         * 
         * 
         * @return The time passed.
         */
        double time() const
        {
            return _time;
        }


        /** 
         * @brief Select an interpolation algorithm
         * 
         * @param algorithm One of "Trapezoidal, "Linear" or "Step"
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
         * @brief Track the TaskFrame in the Command DataObject.
         * 
         * @param do_name The name of the taskframe DataObject
         * 
         * @return true if acceptable.
         */
        bool trackTaskFrameCommand( const std::string& do_name )
        {
            return trackTaskFrame( Command->dObj(), do_name );
        }

        /** 
         * @brief Track the TaskFrame in the Input DataObject.
         * 
         * @param do_name The name of the taskframe DataObject
         * 
         * @return true if acceptable.
         */
        bool trackTaskFrameInput( const std::string& do_name )
        {
            return trackTaskFrame( Input->dObj(), do_name );
        }

        /** 
         * @brief Track the TaskFrame in the Model DataObject.
         * 
         * @param do_name The name of the taskframe DataObject
         * 
         * @return true if acceptable.
         */
        bool trackTaskFrameModel( const std::string& do_name )
        {
            return trackTaskFrame( Model->dObj(), do_name );
        }

        /** 
         * @brief Track the Position in the Command DataObject.
         * 
         * @param do_name The name of the DataObject which contains the position
         * @param do_time The refresh time to read a new position.
         * 
         * @return true if acceptable.
         */
        bool trackPositionCommand(const std::string& do_name, const std::string& do_time)
        {
            return trackPosition( Command->dObj(), do_name, do_time );
        }

        /** 
         * @brief Track the Position in the Model DataObject.
         * 
         * @param do_name The name of the DataObject which contains the position
         * @param do_time The refresh time to read a new position.
         * 
         * @return true if acceptable.
         */
        bool trackPositionModel(const std::string& do_name, const std::string& do_time)
        {
            return trackPosition( Model->dObj(), do_name, do_time );
        }

        /** 
         * @brief Track the Position in the Input DataObject.
         * 
         * @param do_name The name of the DataObject which contains the position
         * @param do_time The refresh time to read a new position.
         * 
         * @return true if acceptable.
         */
        bool trackPositionInput(const std::string& do_name, const std::string& do_time)
        {
            return trackPosition( Input->dObj(), do_name, do_time );
        }

        /** 
         * @brief Inspect if a tracking operation is succeeding.
         * 
         * @return True if succesfull
         */
        bool isTracking() const
        {
            return tracking;
        }

        /** 
         * @brief Inspect if a TaskFrame is being tracked.
         * 
         * @return  True if successful.
         */
        bool isTaskTracking( ) const
        {
            return task_tracking;
        }
        /**
         * @}
         */

        virtual bool updateProperties( const PropertyBag& bag)
        {
            composeProperty( bag, closeness );
            composeProperty( bag, max_vel );
            composeProperty( bag, max_acc );
            composeProperty( bag, interpol_prop);

            interpolate( interpol_prop.get() ) ; // re-init 
            return true;
        }

        virtual void exportProperties( PropertyBag & bag )
        {
            bag.add(&closeness);
            bag.add(&max_vel);
            bag.add(&max_acc);
            bag.add(&interpol_prop);
        }

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< CartesianPositionTracker >* ret =
                newDataSourceFactory( this );
            ret->add( "position", 
                      data( &CartesianPositionTracker::position, "The current position "
                            "of the robot." ) );
            ret->add( "time",
                      data( &CartesianPositionTracker::time, 
                            "The current time in the movement "
                            ) );
            ret->add( "isTracking",
                      data( &CartesianPositionTracker::isTracking, 
                            "Return true if the Generator is tracking an object."
                            ) );
            return ret;
        }

        MethodFactoryInterface* createMethodFactory()
        {
            TemplateMethodFactory< CartesianPositionTracker >* ret =
                newMethodFactory( this );
            ret->add( "trackPositionInput",
                      method( &CartesianPositionTracker::trackPositionInput,
                               "Track the Frame in the Input DataObject",
                               "FrameName","The DataObject Name of the Frame to track",
                               "TimeName", "The DataObject Name of the duration of one setpoint."
                               ) ); 
            ret->add( "trackTaskFrameInput",
                      method( &CartesianPositionTracker::trackTaskFrameInput,
                               "Use the Task Frame in the Input DataObject",
                               "FrameName","The DataObject Name of the Task Frame"
                               ) ); 
            return ret;
        }
#endif
    protected:
        template < class DOS >
        bool trackTaskFrame( DOS* dObj, const std::string& do_name )
        {
            if (!dObj->has(do_name, Frame() ) )
                task_tracking = false;
            else
                {
                    dObj->Get(do_name, task_f_DObj);
                    task_tracking = true;
                }
            return task_tracking;
        } 

        template< class DOS >
        bool trackPosition(const DOS* dObj, const std::string& do_name, const std::string& do_time) {
            if (!dObj->has(do_name, Frame() ) ||
                !dObj->has(do_time, double() ) )
                tracking = false;
            else
                {
                    dObj->Get(do_name, track_f_DObj);
                    dObj->Get(do_time, track_t_DObj);
                    tracking = true;
                }
            return tracking;
        }

        bool tracking;
        bool task_tracking;

        TimeService::ticks timestamp;
        Seconds      _time;

        VelocityProfile* interpol;
        Trajectory*  cur_tr;

        Frame tool_mp_frame;

        Frame task_frame;
        DataObjectInterface<Frame>* task_f_DObj;

        Frame track_frame;
        DataObjectInterface<Frame>* track_f_DObj;

        double track_time;
        DataObjectInterface<double>* track_t_DObj;

        Frame end_pos;
        DataObjectInterface<Frame>* end_f_DObj;

        Frame mp_base_frame;
        DataObjectInterface<Frame>* mp_base_f_DObj;

        Property<double> closeness;
        Property<double> max_vel;
        Property<double> max_acc;
        Property<std::string> interpol_prop;
    };

}
#endif

