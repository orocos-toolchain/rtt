/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  AxisPositionGenerator.hpp 

                        AxisPositionGenerator.hpp -  description
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
 
#ifndef AXISPOSITION_GENERATOR_HPP
#define AXISPOSITION_GENERATOR_HPP

#include <geometry/velocityprofile_trap.h>
#include <geometry/velocityprofile_traphalf.h>
#include <corelib/Property.hpp>
#include <corelib/HeartBeatGenerator.hpp>
#include <corelib/PropertyBag.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ComponentInterfaces.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <control_kernel/ExecutionExtension.hpp>
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#pragma interface
    
namespace ORO_ControlKernel
{
    using namespace ORO_ControlKernel;
    using namespace ORO_CoreLib;
    using namespace ORO_Geometry;

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif

    struct AxisPositionGeneratorSetPoint
        : public ServedTypes< std::vector<double> >
    {

        AxisPositionGeneratorSetPoint()
        {
            this->insert(std::make_pair(0, "ChannelValues"));
        }
    };

    struct AxisPositionGeneratorInput
        : public ServedTypes< std::vector<double> >
    {};

    /**
     * @brief An Axis Position Generator. Axes are numbered from 1 .. N.
     * @ingroup kcomps kcomp_generator
     */
    class AxisPositionGenerator 
        : public Generator< Expects<AxisPositionGeneratorInput>,
                                      Expects<NoModel>,
                                      Expects<NoCommand>,
                                      Writes<AxisPositionGeneratorSetPoint>,
                                      MakeAspect<PropertyExtension,KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                    ,ExecutionExtension
#endif
                                                    >::Result >
    {
        typedef Generator< Expects<AxisPositionGeneratorInput>,
                           Expects<NoModel>,
                           Expects<NoCommand>,
                           Writes<AxisPositionGeneratorSetPoint>,
                           MakeAspect<PropertyExtension,KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                      ,ExecutionExtension
#endif
                                      >::Result > Base;
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
         * @brief Create a generator for interpolating N axes.
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

        void componentShutdown()
        {
            cout << "Shutdown APG" <<endl;
        }

        bool componentStartup()
        {
            cout << "Startup APG"<<endl;
            // we need the inputs for the stand-still setpoint
            if ( Base::Input::dObj()->Get("ChannelValues", inp_dObj ) )
                {
                    inputs = inp_dObj->Get();
                    setpoints = inputs;
                    push();
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
            for ( std::vector<AxisInfo>::iterator it = axes.begin(); it != axes.end(); ++it)
                if ( it->traj_ptr )
                    {
                        double t =  hbg->secondsSince( it->timestamp );
                        setpoints[ it - axes.begin() ] = it->traj_ptr->Pos( t );
                        if ( it->traj_ptr->Duration() < t )
                            it->traj_ptr = 0; //indicates we are ready for next command.
                    }
        }

        virtual void push()      
        {
            setp_dObj->Set(setpoints);
        }

        /**
         * @name AxisPositionGenerator commands.
         */
        // @{
        /** 
         * @brief Inspect if an Axis is ready for the next command. 
         * 
         * @param nr The Axis number
         * 
         * @return true if so.
         */
        bool isReady( int nr ) const
        {
            if ( nr < 1 || nr > num_axes )
                return false;
            --nr;
            //ready if no current traj or current traj done.
            if (axes[nr].traj_ptr == 0 )
                return true;
            return false;
        }

        double position( int nr ) const
        {
            if ( nr < 1 || nr > num_axes )
                return 0;
            --nr;
            return setpoints[nr];
        }

        /** 
         * @brief Instruct an axis to move to a position with a velocity.
         * 
         * @param axis_nr  The number of the axis to move
         * @param velocity The desired velocity
         * @param position The endposition
         * 
         * @return true if acceptable, false otherwise
         */
        bool move( int axis_nr, double velocity, double position)
        {
            if ( !isReady(axis_nr) )
                return false;

            --axis_nr;
            axes[axis_nr].traj_planner.SetMax( velocity, maxAcc );
            axes[axis_nr].traj_planner.SetProfile( setpoints[axis_nr], position );
            axes[axis_nr].timestamp = hbg->ticksGet();
            axes[axis_nr].traj_ptr  = &axes[axis_nr].traj_planner;
            return true;
        }

        /**
         * @brief Wait for a specified time (in seconds).
         */
        bool wait( int axis_nr, double time )
        {
            if ( isReady(axis_nr) )
                return false;
            --axis_nr;
            axes[axis_nr].traj_planner.SetMax( 0 , maxAcc );
            axes[axis_nr].traj_planner.SetProfileDuration( setpoints[axis_nr], setpoints[axis_nr], time );
            axes[axis_nr].timestamp = hbg->ticksGet();
            axes[axis_nr].traj_ptr  = &axes[axis_nr].traj_planner;
            return true;
        }
        
        // @}

        virtual bool updateProperties( const PropertyBag& bag )
        {
            bool status = composeProperty(bag, maxVel ) &&
                composeProperty(bag, maxAcc );
            return status;
        }
            
    protected:
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< AxisPositionGenerator >* ret =
                newCommandFactory( this );
            ret->add( "move",
                      command( &AxisPositionGenerator::move,
                               &AxisPositionGenerator::isReady,
                               "Move an axis to a position with a given velocity",
                               "AxisNr","The Axis number (starting from 1).",
                               "Velocity","The maximum velocity of the movement.",
                               "Position","The end position of the movement."
                               ) ); 
            ret->add( "wait",
                      command( &AxisPositionGenerator::wait,
                               &AxisPositionGenerator::isReady,
                               "Hold the axis still for an amount of time",
                               "AxisNr","The Axis number (starting from 1).",
                               "Time", "The time to wait, in seconds."
                               ) ); 
            return ret;
        }

        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< AxisPositionGenerator >* ret =
                newDataSourceFactory( this );
            ret->add( "position",
                      data( &AxisPositionGenerator::position,
                            "Get the axis position",
                            "AxisNr","The Axis number (starting from 1)."
                               ) ); 
            ret->add( "isReady",
                      data( &AxisPositionGenerator::isReady,
                            "Inspect if an Axis is ready for a next command.",
                            "AxisNr","The Axis number (starting from 1)."
                            ) ); 
            return ret;
        }
#endif
            
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
