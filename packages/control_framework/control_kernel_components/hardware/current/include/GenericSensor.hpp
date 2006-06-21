/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  GenericSensor.hpp 

                        GenericSensor.hpp -  description
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
 
#ifndef GENERIC_SENSOR_HPP
#define GENERIC_SENSOR_HPP

#include <device_drivers/AnalogInput.hpp>
#include <device_drivers/DigitalInput.hpp>

//#include <device_interface/PositionInterface.hpp>
//#include <device_interface/SensorInterface.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/TemplateFactories.hpp"
#include <control_kernel/ExecutionExtension.hpp>
#endif

#include <map>
#include <utility>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <control_kernel/DataServer.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/Logger.hpp>

#ifdef OROSEM_CONTROL_KERNEL_OLDKERNEL
#error "This Component only works with the new kernel infrastructure."
#endif

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

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
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
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
            this->insert( std::make_pair(4,"ChannelValues") );
        }

        void addDouble(const std::string& name )
        {
            this->insert( std::make_pair(2, name) );
        }

        void removeDouble(const std::string& name )
        {
            for(iterator t= this->begin(); t != this->end(); ++t)
                if ( t->first == 2 && t->second == name ) {
                    this->erase( t );
                    break;
                }
        }
        void addUint(const std::string& name )
        {
            this->insert( std::make_pair(0, name) );
        }

        void removeUint(const std::string& name )
        {
            for(iterator t= this->begin(); t != this->end(); ++t)
                if ( t->first == 0 && t->second == name ) {
                    this->erase( t );
                    break;
                }
        }
    };

    /**
     * @brief A Generic Sensor using the ORO_DeviceInterface devices
     * for accessing hardware devices. Off course, you
     * can also use simulated hardware.
     * @ingroup kcomps kcomp_sensor
     */
    class GenericSensor
        : public Sensor< Writes<GenericInput>,
                                   MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                              , ExecutionExtension
#endif
                                              >::Result>
    {
        typedef Sensor< Writes<GenericInput>,
                        MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                   , ExecutionExtension
#endif
                                   >::Result> Base;
    public:
        typedef GenericInput InputDataObject;
            
        /**
         * @brief Create a Sensor with maximum \a max_chan virtual channels in "ChannelValues" and
         * an unlimited number of DataObjects representing analog/digital channels
         */
        GenericSensor( int max_chan = 32, const std::string& name="GenericSensor") 
            :  Base( name ),
               max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan),
               usingChannels(0)
        {
//             channels.resize(max_chan, AnalogInput<unsigned int>*(0) );
            channels.resize(max_chan, 0 );
            chan_meas.resize(max_chan, 0.0);
        }

        bool componentLoaded()
        {
            if ( !Input->dObj()->Get("ChannelValues",chan_DObj) )
                return false;
            // kind-of resize of the vector in the dataobject:
            chan_DObj->Set(chan_meas); 

            // Get the inserted Analog Inputs
            for (AInMap::iterator itl = a_in.begin(); itl != a_in.end(); ++itl) {
                if ( !Input->dObj()->Get(itl->first+"_raw", get<1>(itl->second) ) )
                    Logger::log() << "Raw Analog Input not found in load !"<<Logger::endl;
                if ( !Input->dObj()->Get(itl->first,get<2>( itl->second) ) )
                    Logger::log() << "Analog Input not found in load !"<<Logger::endl;
            }

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
            std::for_each( a_in.begin(), a_in.end(), bind( &GenericSensor::write_a_in_to_do, this, _1 ) );

            if (usingChannels) {
                for (unsigned int i=0; i < channels.size(); ++i)
                    chan_meas[i] = channels[i] ? channels[i]->value() : 0 ;

                // writeout.
                chan_DObj->Set( chan_meas );
            }
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
         */
        bool addAnalogInput( const std::string& DO_name, AnalogInInterface<unsigned int>* input, int channel)
        {
            if ( a_in.count(DO_name) != 0  || this->kernel()->isRunning() )
                return false;

            // Before Reload, Add All DataObjects :
            this->Input->dObj()->addDouble( DO_name );
            this->Input->dObj()->addUint( DO_name+"_raw" );

            a_in[DO_name] =
                tuple<AnalogInput<unsigned int>*,
                DataObjectInterface<unsigned int>*,
                DataObjectInterface<double>* >( new AnalogInput<unsigned int>( input, channel ), 0, 0 );

            if (this->inKernel() )
                this->kernel()->reload(this);

            return true;
        }

        /**
         * @brief Remove a previously added AnalogInput.
         */
        bool removeAnalogInput( const std::string& DO_name )
        {
            if ( a_in.count(DO_name) != 1 || this->kernel()->isRunning() )
                return false;

            tuple<AnalogInput<unsigned int>*, DataObjectInterface<unsigned int>*, DataObjectInterface<double>* > res = a_in[DO_name];

            a_in.erase(DO_name);
            return true;
        }

        /**
         * @brief Add an analog Channel.
         *
         * A std::vector<double> DataObject
         * ( "ChannelValues") is used which contains
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
        bool addChannel(int virt_channel, AnalogInInterface<unsigned int>* input, int channel )
        {
            if ( channels[virt_channel] != 0 || this->kernel()->isRunning() )
                return false;

            ++usingChannels;

            channels[virt_channel] = new AnalogInput<unsigned int>( input, channel ) ;

            return true;
        }

        /**
         * @brief Remove a previously added analog Channel.
         */
        bool removeChannel( int virt_channel )
        {
            if ( this->kernel()->isRunning() )
                return false;

            --usingChannels;

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
            if ( d_in.count(name) != 0 || this->kernel()->isRunning() )
                return false;

            d_in[name] = new DigitalInput( input, channel, invert );

            return true;
        }

        /**
         * @brief Remove a previously added Digital Input.
         */
        bool removeDigitalInput( const std::string& name )
        {
            if ( d_in.count(name) != 1 || this->kernel()->isRunning() )
                return false;

            DigitalInput* res = d_in[name];

            delete res;

            d_in.erase(name);
            return true;
        }

        /**
         * @name Generic Sensor Commands
         * Runtime commands to inspect the sensor values.
         * This is done outside the Data Flow, thus also
         * possible when the GenericSensor is not selected.
         * @{
         */
        /** 
         * @brief Inspect if a DigitalInput is on
         * 
         * @param name The name of the DigitalInput
         * 
         * @return true if so.
         */
        bool isOn(const std::string& name) const
        {
            DInMap::const_iterator it = d_in.find(name);
            if ( it == d_in.end() )
                return false;
            return it->second->isOn();
        }

        /** 
         * @brief Return the value of an AnalogInput
         * 
         * @param name The name of the AnalogInput
         * 
         * @return The physical value.
         */
        double value(const std::string& name) const
        {
            AInMap::const_iterator it = a_in.find(name);
            if ( it == a_in.end() )
                return 0;
            return get<0>( it->second )->value();
        }

        /** 
         * @brief Return the raw sensor value of an AnalogInput
         * 
         * @param name The name of the AnalogInput
         * 
         * @return The raw value.
         */
        int rawValue(const std::string& name) const
        {
            AInMap::const_iterator it = a_in.find(name);
            if ( it == a_in.end() )
                return 0;
            return get<0>( it->second )->rawValue();
        }

        /** 
         * Return the number of Channels
         * 
         */
        int getChannels() const
        {
            return chan_meas.size();
        }
        /**
         * @}
         */

    protected:
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< GenericSensor >* ret =
                newDataSourceFactory( this );
            ret->add( "isOn", 
                      data( &GenericSensor::isOn,
                            "Inspect the status of a Digital Input.",
                            "Name", "The Name of the Digital Input."
                            ) );
            ret->add( "value", 
                      data( &GenericSensor::value,
                            "Inspect the value of an Analog Input.",
                            "Name", "The Name of the Analog Input."
                            ) );
            ret->add( "rawValue", 
                      data( &GenericSensor::rawValue,
                            "Inspect the raw value of an Analog Input.",
                            "Name", "The Name of the Analog Input."
                            ) );
            ret->add( "channels", 
                      data( &GenericSensor::getChannels,
                            "Get the number of channels this Sensor monitors."
                            ) );
            return ret;
        }
#endif

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

        typedef std::map<std::string, DigitalInput* > DInMap;
        DInMap d_in;
        typedef 
        std::map<std::string,
                 tuple< AnalogInput<unsigned int>*,
                        DataObjectInterface<unsigned int>*,
                        DataObjectInterface<double>* > > AInMap;
        AInMap a_in;

        int usingChannels;
    };

}

#endif

