/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  AxisSensor.hpp 

                        AxisSensor.hpp -  description
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
 
#ifndef AXIS_SENSOR_HPP
#define AXIS_SENSOR_HPP

#include <device_drivers/Axis.hpp>
#include <device_drivers/AnalogDrive.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include <map>
#include <utility>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <control_kernel/DataServer.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExecutionExtension.hpp>

#pragma interface

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif
    using namespace ORO_DeviceInterface;
    using namespace ORO_DeviceDriver;
    using boost::bind;

    /**
     * @brief An AxisSensor can write doubles and produce Channel Measurements.
     */
    struct AxisInput
        : public ServedTypes<double, std::vector<double> >
    {
        AxisInput()
        {
            this->insert( std::make_pair(1,"ChannelMeasurements") );
        }
    };

    /**
     * @brief An Axis Reading Sensor for a Motion Control Kernel
     *
     * Each added Axis introduces DataObjects (due to its sensors)
     * and Digital Inputs/Outputs which you can use to read the
     * status.
     *
     * @todo : Better channel<->sensor assignment
     * @ingroup kcomps kcomp_sensor
     */
    template <class Base = Sensor< Writes<AxisInput>, MakeExtension<KernelBaseFunction, ExecutionExtension>::CommonBase > >
    class AxisSensor
        : public Base
    {
    public:
        typedef AxisInput InputDataObject;
        typedef std::vector<double> ChannelType;

        /**
         * @brief Create a Sensor with maximum <max_chan> virtual channels in "ChannelMeasurements" and
         * an unlimited number of DataObjects representing analog/digital channels
         */
        AxisSensor( int max_chan = 1) 
            :  Base("AxisSensor"),
               max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan)
        {
            Axis* _a = 0;
            SensorInterface<double>* _d = 0;
            channels.resize(max_chan, make_pair(_d,_a) );
            chan_meas.resize(max_chan, 0.0);
        }

        bool componentLoaded()
        {
            if ( !Base::Input::dObj()->Get("ChannelMeasurements",chan_DObj) )
                return false;
            // kind-of resize of the vector in the dataobject:
            chan_DObj->Set(chan_meas); 
            return true;
        }

        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            /*
             * Acces Sensor device drivers
             */
            std::for_each( drive.begin(), drive.end(), bind( &AxisSensor<Base>::drive_to_do, this, _1 ) );
            std::for_each( sensor.begin(), sensor.end(), bind( &AxisSensor<Base>::sensor_to_do, this, _1 ) );

            // gather results.
            for (unsigned int i=0; i < channels.size(); ++i)
                chan_meas[i] = channels[i].first ? channels[i].first->readSensor() : 0 ;

            // writeout.
            chan_DObj->Set( chan_meas );
        }

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Input and
         * all Axis status info (homed, enabled, breaked, ...)
         *
         */
        bool addAxis( const std::string& name, Axis* ax )
        {
            if ( axes.count(name) != 0 || this->kernel()->isRunning() )
                return false;

            // no channel tied == -1
            axes[name] = ax;

            d_out[ name + ".Drive" ] = ax->driveGet()->enableGet();
            if ( ax->breakGet() )
            d_out[ name + ".Break" ] = ax->breakGet();
            if ( ax->homeswitchGet() )
                d_in[ name + ".Home" ] = ax->homeswitchGet();

            // Create the dataobjects in the Input DO.
            typedef typename Base::Input::template DataObject<double>::type doubleType;
            drive[ name ] = make_pair( ax->driveGet(), new doubleType(name+".Velocity") );
            Base::Input::dObj()->reg( drive[ name ].second );

            // Repeat for each additional sensor...
            std::vector<std::string> res( ax->sensorList() );
            for ( std::vector<std::string>::iterator it = res.begin(); it != res.end(); ++it)
                {
                    std::string sname( name+"."+*it );
                    sensor[ sname ] = make_pair( ax->sensorGet( *it ), new doubleType( sname ));
                    Base::Input::dObj()->reg( sensor[ sname ].second );
                }
            return true;
        }

        /** 
         * @brief Add a sensor of an Axis object on a Channel.
         *
         * A Channel is added representing the Sensor on channel \a virtual_channel,
         * 
         * @param axis_name        The name of the previously added Axis
         * @param sensor_name      The name of a Sensor of the Axis
         * @param virtual_channel  The channel number where the Sensor must be added.
         * 
         * @return true if successfull, false otherwise.
         */
        bool addSensorOnChannel(const std::string& axis_name, const std::string& sensor_name, int virtual_channel )
        {
            if ( virtual_channel >= max_channels ||
                 channels[virtual_channel].first != 0 ||
                 axes.count(axis_name) != 1 ||
                 axes[axis_name]->sensorGet( sensor_name ) == 0 ||
                 this->kernel()->isRunning() )
               return false;

            // The owner Axis is stored in the channel.
            channels[virtual_channel] = make_pair( axes[axis_name]->sensorGet( sensor_name ), axes[axis_name] );
            return true;
        }

        /**
         * @brief Remove a previously added Axis.
         */
        bool removeAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 || this->kernel()->isRunning() )
                return false;

            for ( std::vector< pair< const SensorInterface<double>*, Axis* > >::iterator it = channels.begin();
                  it != channels.end();
                  ++it )
                if ( it->second == axes[name] )
                    {
                        it->first = 0; // clear the channel occupied by an axis sensor
                        it->second = 0;
                    }

            drive.erase( name );

            // remove all sensors.
            std::vector<std::string> res( axes[name]->sensorList() );
            for ( std::vector<std::string>::iterator it = res.begin(); it != res.end(); ++it)
                {
                    std::string sname( name+"."+*it );
                    sensor.erase( sname );
                }
            d_out.erase( name + ".Drive" );
            d_out.erase( name + ".Break" );
            d_in.erase( name + ".Home" );

            axes.erase(name);
            return true;
        }

        /**
         * @name AxisSensor Commands.
         * Runtime commands for the AxisSensor.
         * @{
         */
        /**
         * @brief Inspect if an axis is enabled.
         */
        bool isEnabled( const std::string& name ) const
        {
            AxisMap::const_iterator it = axes.find(name);
            if ( it != axes.end() )
                return it->second->isEnabled();
            return false;
        }

        /**
         * @brief Inspect the position of an Axis.
         */
        double position( const std::string& name ) const
        {
            SensorMap::const_iterator it = sensor.find(name+".Position");
            if ( it != sensor.end() )
                return it->second.first->readSensor();
            return 0;
        }

        /**
         * @brief Inspect the status of a Digital Input or
         * Digital Output.
         */
        bool isOn( const std::string& name ) const
        {
            if ( d_in.count(name) == 1 )
                return d_in.find(name)->second->isOn();
            else if (d_out.count(name) == 1 )
                return d_out.find(name)->second->isOn();
            return false;
        }
        /**
         * @}
         */

    protected:
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER

        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< AxisSensor<Base> >* ret =
                newDataSourceFactory( this );
            ret->add( "isOn", 
                      data( &AxisSensor<Base>::isOn,
                            "Inspect the status of a Digital Input or Output.",
                            "Name", "The Name of the Digital IO."
                            ) );
            ret->add( "position", 
                      data( &AxisSensor<Base>::position,
                            "Inspect the status of the Position of an Axis.",
                            "Name", "The Name of the Axis."
                            ) );
            ret->add( "isEnabled", 
                      data( &AxisSensor<Base>::isEnabled,
                            "Inspect the status of an Axis.",
                            "Name", "The Name of the Axis."
                            ) );
            return ret;
        }
#endif

        /**
         * Write Analog input to DataObject.
         */
        void drive_to_do( std::pair<std::string,std::pair<AnalogDrive*,
                          DataObjectInterface<double>* > > dd )
        {
            dd.second.second->Set( dd.second.first->driveGet() );
        }
            
        void sensor_to_do( std::pair<std::string,std::pair< const SensorInterface<double>*,
                           DataObjectInterface<double>* > > dd )
        {
            dd.second.second->Set( dd.second.first->readSensor() );
        }
            
        Property<int> max_channels;

        std::vector< std::pair< const SensorInterface<double>*, Axis* > > channels;
        ChannelType chan_meas;
        DataObjectInterface< ChannelType >* chan_DObj;

        std::map<std::string, const DigitalInput* > d_in;
        std::map<std::string, DigitalOutput* > d_out;

        typedef
        std::map<std::string, std::pair<AnalogDrive*, DataObjectInterface<double>* > > DriveMap;
        DriveMap drive;
        typedef
        std::map<std::string, std::pair< const SensorInterface<double>*, DataObjectInterface<double>* > > SensorMap;
        SensorMap sensor;
        typedef
        std::map<std::string, Axis* > AxisMap;
        AxisMap axes;
        
    };

    extern template class AxisSensor<>;

}

#endif

