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
#include <device_drivers/DigitalInput.hpp>
#include <device_drivers/DigitalOutput.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateMethodFactory.hpp"
#include <control_kernel/ExecutionExtension.hpp>
#endif

#include <map>
#include <utility>
#include <string>
#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <control_kernel/DataServer.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>

#ifdef OROSEM_CONTROL_KERNEL_OLDKERNEL
#error "This Component only works with the new kernel infrastructure."
#endif

#pragma interface

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    using ORO_DeviceInterface::SensorInterface;
    using ORO_DeviceDriver::Axis;
    using ORO_DeviceDriver::AnalogDrive;
    using ORO_DeviceDriver::DigitalInput;
    using ORO_DeviceDriver::DigitalOutput;
    using boost::bind;

    /**
     * @brief An AxisSensor can write doubles and produce Channel Measurements.
     */
    struct AxisInput
        : public ServedTypes<double, std::vector<double> >
    {
        AxisInput()
        {
            this->insert( std::make_pair(1,"ChannelValues") );
        }

        void addDouble(const std::string& name )
        {
            this->insert( std::make_pair(0, name) );
        }

        void removeDouble(const std::string& name )
        {
            for(iterator t= this->begin(); t != this->end(); ++t)
                if ( t->first == 0 && t->second == name ) {
                    this->erase( t );
                    break;
                }
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
    class AxisSensor
        : public Sensor< Writes<AxisInput>, MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                       , ExecutionExtension
#endif
                                                       >::CommonBase >
    {
    public:
        typedef Sensor< Writes<AxisInput>, MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                      , ExecutionExtension
#endif
                                                      >::CommonBase > Base;
        typedef AxisInput InputDataObject;
        typedef std::vector<double> ChannelType;

        /**
         * @brief Create a Sensor with maximum <max_chan> virtual channels in "ChannelMeasurements" and
         * an unlimited number of DataObjects representing analog/digital channels
         */
        AxisSensor( int max_chan = 1 , const std::string& name = "AxisSensor" );

        virtual bool componentLoaded();

        virtual bool componentStartup();

        virtual void componentUnloaded();

        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push();

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Input and
         * all Axis status info (homed, enabled, breaked, ...)
         *
         */
        bool addAxis( const std::string& name, Axis* ax );

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
        bool addSensorOnChannel(const std::string& axis_name, const std::string& sensor_name, int virtual_channel );

        /** 
         * @brief Remove a Sensor from a virtual channel.
         * 
         * @param virtual_channel The channel to remove.
         * 
         */
        void removeSensorFromChannel(int virtual_channel);

        /**
         * @brief Remove a previously added Axis.
         */
        bool removeAxis( const std::string& name );

        /**
         * @name AxisSensor Methods.
         * Runtime commands for the AxisSensor.
         * @{
         */
        /**
         * @brief Inspect if an axis is enabled.
         */
        bool isEnabled( const std::string& name ) const;

        /**
         * @brief Inspect the position of an Axis.
         */
        double position( const std::string& name ) const;

        /**
         * @brief Inspect the status of a Digital Input or
         * Digital Output.
         */
        bool isOn( const std::string& name ) const;

        /**
         * @brief Inspect a Sensor value of the Axis.
         */
        double readSensor( const std::string& name ) const;
        /**
         * @}
         */

    protected:
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        ORO_Execution::DataSourceFactoryInterface* createDataSourceFactory();
#endif

        /**
         * Write Analog input to DataObject.
         */
        void drive_to_do( std::pair<std::string,std::pair<AnalogDrive*,
                          DataObjectInterface<double>* > > dd );
            
        void sensor_to_do( std::pair<std::string,std::pair< const SensorInterface<double>*,
                           DataObjectInterface<double>* > > dd );
            
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

        std::string axis_to_remove;
        int usingChannels;
    };
}

#endif

