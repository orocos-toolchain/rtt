/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  GenericSensor.hpp 

                        GenericSensor.hpp -  description
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
 
 
#ifndef GENERIC_SENSOR_HPP
#define GENERIC_SENSOR_HPP

#include <device_drivers/AnalogInput.hpp>
#include <device_drivers/DigitalInput.hpp>

//#include <device_interface/PositionInterface.hpp>
//#include <device_interface/SensorInterface.hpp>

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

/**
 * @file GenericSensor.hpp
 * This file contains a for most applications
 * usefull Sensor, which uses the Orocos Device
 * Interfaces to communicate with hardware.
 *
 */
namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif
    using namespace ORO_DeviceInterface;
    using namespace ORO_DeviceDriver;
    using boost::bind;
    using namespace boost;

    /**
     * @brief A GenericSensor can produce integers, doubles, booleans
     * and Channel Measurements.
     */
    struct GenericInput
        : public ServedTypes<unsigned int, int, double, bool, std::vector<double> >
    {
        GenericInput()
        {
            this->insert( make_pair(4,"ChannelMeasurements") );
        }
    };

    /**
     * @brief A Generic Sensor using the ORO_DeviceInterface devices
     * for accessing hardware devices. Off course, you
     * can also use simulated hardware.
     * @ingroup kcomps kcomp_sensor
     */
    template <class Base>
    class GenericSensor
        : public Base
    {
    public:
        typedef GenericInput InputDataObject;

        /**
         * Necessary typedefs.
         */
        typedef typename Base::InputType InputType;
            
        /**
         * @brief Create a Sensor with maximum <max_chan> virtual channels in "ChannelMeasurements" and
         * an unlimited number of DataObjects representing analog/digital channels
         */
        GenericSensor( int max_chan = 32) 
            :  Base("GenericSensor"),
               max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan)
        {
//             channels.resize(max_chan, AnalogInput<unsigned int>*(0) );
            channels.resize(max_chan, 0 );
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
             * Acces Analog device drivers
             */
            std::for_each( a_in.begin(), a_in.end(), bind( &GenericSensor<Base>::write_a_in_to_do, this, _1 ) );

            // gather results.
            for (unsigned int i=0; i < channels.size(); ++i)
                chan_meas[i] = channels[i] ? channels[i]->value() : 0 ;

            // writeout.
            chan_DObj->Set( chan_meas );
        }

        /**
         * @brief Add an AnalogInput device.
         *
         * A DataObject is created which contains
         * the converted value of the input. The raw value of the channel can be read
         * through the DataObject DO_name + "_raw".
         *
         * The resulting value is : value = value_raw*scale + offset
         *
         * @param DO_name The name of the DataObject.
         * @param input   The AnalogInInterface for reading the input.
         * @param channel The channel of the input.
         * @param offset  The offset to be added to the converted value of the input.
         * @param scale   Conversion factor for the raw input value.
         */
        bool addAnalogInput( const std::string& DO_name, AnalogInInterface<unsigned int>* input, int channel, double offset=0.0, double scale=1.0)
        {
            if ( a_in.count(DO_name) != 0  || kernel()->isRunning() )
                return false;

            typedef typename Base::Input::DataObject<double>::type doubleType;
            typedef typename Base::Input::DataObject<unsigned int>::type uintType;

            DataObjectInterface<double>* doi =  new doubleType( DO_name );
            DataObjectInterface<unsigned int>* raw_doi = new uintType( DO_name+"_raw" );
            Base::Input::dObj()->reg( doi );
            Base::Input::dObj()->reg( raw_doi );

            a_in[DO_name] = make_tuple( new AnalogInput<unsigned int>( input, channel, offset, scale ), raw_doi, doi );

            return true;
        }

        /**
         * @brief Remove a previously added AnalogInput.
         */
        bool removeAnalogInput( const std::string& name )
        {
            if ( a_in.count(DO_name) != 1 || kernel()->isRunning() )
                return false;

            tuple<AnalogInput<unsigned int>*, DataObjectInterface<unsigned int>*, DataObjectInterface<double>* > res = a_in[name];

            Base::Input::dObj()->deReg( get<1>(res) );
            Base::Input::dObj()->deReg( get<2>(res) );

            delete get<0>(res);
            delete get<1>(res);
            delete get<2>(res);

            a_in.erase(name);
            return true;
        }

        /**
         * @brief Add an analog Channel.
         *
         * A std::vector<double> DataObject
         * ( "ChannelMeasurements") is used which contains
         * the converted value of the input. 
         *
         * The resulting value is : value = value_raw*scale + offset
         *
         * @param virt_channel The virtual channel (in software).
         * @param input   The AnalogInInterface for reading the input.
         * @param channel The physical channel of the input (in hardware).
         * @param offset  The offset to be added to the converted value of the input.
         * @param scale   Conversion factor for the raw input value.
         */
        bool addChannel(int virt_channel, AnalogInInterface<unsigned int>* input, int channel, double offset=0.0, double scale=1.0)
        {
            if ( channels[virt_channel] != 0 || kernel()->isRunning() )
                return false;

            channels[virt_channel] = new AnalogInput<unsigned int>( input, channel, offset, scale ) ;

            return true;
        }

        /**
         * @brief Remove a previously added analog Channel.
         */
        bool removeChannel( int virt_channel )
        {
            if ( kernel()->isRunning() )
                return false;

            channels[virt_channel] = 0;
            return true;
        }

        /**
         * @brief Add an DigitalInput device.
         *
         * No DataObject is created, but the input is available through the execution
         * interface.
         *
         * @param name    The name of the Digital Input.
         * @param input   The DigitalInInterface for reading the input.
         * @param channel The channel of the input.
         * @param invert  True if the input must be inverted, false(default) otherwise.
         */
        bool addDigitalInput( const std::string& name, DigitalInInterface* input, int channel, bool invert=false)
        {
            if ( d_in.count(name) != 0 || kernel()->isRunning() )
                return false;

            d_in[name] = new DigitalInput( input, channel, invert );

            return true;
        }

        /**
         * @brief Remove a previously added Digital Input.
         */
        bool removeDigitalInput( const std::string& name )
        {
            if ( d_in.count(name) != 1 || kernel()->isRunning() )
                return false;

            DigitalInput* res = d_in[name];

            delete res;

            d_in.erase(name);
            return true;
        }

        /**
         * @name Generic Sensor Commands
         * Runtime commands to inspect the sensor values.
         * @{
         */
        /** 
         * @brief Inspect if a DigitalInput is on
         * 
         * @param name The name of the DigitalInput
         * 
         * @return true if so.
         */
        bool isOn(const std::string& name)
        {
            if ( d_in.count(name) == 0)
                return false;
            return d_in[name]->isOn();
        }

        /** 
         * @brief Return the value of an AnalogInput
         * 
         * @param name The name of the AnalogInput
         * 
         * @return The physical value.
         */
        double value(const std::string& name)
        {
            if ( a_in.count(name) == 0)
                return 0;
            return get<0>( a_in[name] )->value();
        }

        /** 
         * @brief Return the raw sensor value of an AnalogInput
         * 
         * @param name The name of the AnalogInput
         * 
         * @return The raw value.
         */
        unsigned int rawValue(const std::string& name)
        {
            if ( a_in.count(name) == 0)
                return 0;
            return get<0>( a_in[name] )->rawValue();
        }
        /**
         * @}
         */

    protected:

        /**
         * Write Analog input to DataObject.
         */
        void write_a_in_to_do( std::pair<std::string,tuple<AnalogInput<unsigned int>*,
                               DataObjectInterface<unsigned int>*,
                               DataObjectInterface<double>* > > dd )
        {
            // See boost::tuple for syntax
            get<1>(dd.second)->Set( get<0>(dd.second)->rawValue() );
            get<2>(dd.second)->Set( get<0>(dd.second)->value() );
        }
            
        Property<int> max_channels;

        std::vector< AnalogInput<unsigned int>* > channels;
        std::vector<double> chan_meas;
        DataObjectInterface< std::vector<double> >* chan_DObj;

        std::map<std::string, DigitalInput* > d_in;
        std::map<std::string,
                 tuple< AnalogInput<unsigned int>*,
                        DataObjectInterface<unsigned int>*,
                        DataObjectInterface<double>* > > a_in;
    };

}

#endif

