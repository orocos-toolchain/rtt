/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  AxisPositionGenerator.hpp 

                        AxisPositionGenerator.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef AXISPOSITION_GENERATOR_HPP
#define AXISPOSITION_GENERATOR_HPP

#include <geometry/velocityprofile_trap.h>
#include <geometry/velocityprofile_traphalf.h>
#include <corelib/Property.hpp>
#include <corelib/HeartBeatGenerator.hpp>
#include <corelib/PropertyBag.hpp>
    
namespace ORO_ControlKernel
{
    //using namespace ORO_ControlKernel;
    using namespace ORO_CoreLib;
    using namespace ORO_Geometry;
    using namespace ORO_DeviceInterface;
    using namespace ORO_DeviceDriver;

    struct AxisPositionGeneratorSetPoint
        : public ServedTypes< std::vector<double> >
    {

        AxisPositionGeneratorSetPoint()
        {
            this->insert(std::make_pair(0, "ChannelValues"));
        }
    };


    /**
     * A AxisPosition Generator. Axes are numbered from 1 .. N.
     */
    template <class Base>
    class AxisPositionGenerator 
        : public Base
    {
        typedef std::vector<double> ChannelType;
        struct AxisInfo
        {
            AxisInfo() : traj_planner(1.,1.), traj_ptr(0), timestamp(0) {}
            VelocityProfile_Trap traj_planner;
            VelocityProfile*     traj_ptr;
            HeartBeatGenerator::ticks timestamp;
        };            
    public:
        /**
         * Constructor.
         */
        AxisPositionGenerator( int _num_axes ) 
            : Base("AxisPositionGenerator"),
              maxVel("MaxVel","",0),
              maxAcc("MaxAcc","",0),
              num_axes(_num_axes),
              axes(_num_axes),
              hbg( HeartBeatGenerator::Instance() )
        {
            setpoints.resize(num_axes,0);
            axes.resize( num_axes );
        }

        bool componentLoaded()
        {
            return Base::SetPoint::dObj()->Get("ChannelValues", setp_dObj );
        }

        bool componentStartup()
        {
            // we need the inputs for the stand-still setpoint
            if ( Base::Input::dObj()->Get("ChannelMeasurements", inp_dObj ) )
                {
                    setpoints = inp_dObj->Get();
                    return true;
                }
            return false;
        }

        virtual void pull()      
        {
            setp_dObj->Get(inputs);
        }

        virtual void calculate()
        {
            for (typename std::vector<AxisInfo>::iterator it = axes.begin(); it != axes.end(); ++it)
                if ( it->traj_ptr )
                    setpoints[ it - axes.begin() ] = it->traj_ptr->Pos( hbg->secondsSince( it->timestamp ) );
        }

        virtual void push()      
        {
            setp_dObj->Set(setpoints);
        }

        bool isReady( int nr )
        {
            --nr;
            if ( nr < 1 || nr > num_axes )
                return false;
            //ready if no current traj or current traj done.
            if (axes[nr].traj_ptr == 0 )
                return true;
            if (axes[nr]->traj_ptr->Duration() < hbg->secondsSince(axes[nr]->timestamp) )
                return true;
            return false;
        }

        bool move( int axis_nr, double velocity, double position)
        {
            if ( !isReady(axis_nr) )
                return false;

            --axis_nr;
            axes[axis_nr].traj_ptr = 0;
            axes[axis_nr].traj_planner->SetMax( velocity, maxAcc );
            axes[axis_nr].SetProfile( inputs[axis_nr], position );
            axes[axis_nr].timestamp = hbg->ticksGet();
            axes[axis_nr].traj_ptr  = axes[axis_nr].traj_planner;
        } 

        virtual bool updateProperties( const PropertyBag& bag )
        {
            bool status = composeProperty(bag, maxVel ) &&
                composeProperty(bag, maxAcc );
            return status;
        }
            
    protected:

        Property<double> maxVel;
        Property<double> maxAcc;
        int num_axes;
        std::vector< AxisInfo > axes;

        DataObjectInterface<ChannelType>* inp_dObj;
        DataObjectInterface<ChannelType>* setp_dObj;
        ChannelType inputs;
        ChannelType setpoints;
        HeartBeatGenerator* hbg;
    };
}

#endif
