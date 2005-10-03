/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  AxisEffector.hpp 

                        AxisEffector.hpp -  description
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
 
#ifndef AXIS_EFFECTOR_HPP
#define AXIS_EFFECTOR_HPP

#include <device_drivers/Axis.hpp>
#include <device_drivers/AnalogOutput.hpp>
#include <device_drivers/AnalogDrive.hpp>
#include <device_drivers/DigitalOutput.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/TemplateMethodFactory.hpp"
#include <control_kernel/ExecutionExtension.hpp>
#endif

#include <map>
#include <control_kernel/DataServer.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    using ORO_DeviceInterface::SensorInterface;
    using ORO_DeviceDriver::DigitalOutput;
    using boost::bind;
    using std::make_pair;
    using std::pair;

    /**
     * @brief An AxisEffector can read integers, doubles and booleans
     * and Channel Values.
     */
    struct AxisOutput
        : public ServedTypes<unsigned int, int, double, bool, std::vector<double> >
    {};

    /**
     * @brief An Effector using the ORO_DeviceDriver Axis implementation
     * for accessing machine or robot axes.
     *
     * @see ControlKernel
     * @ingroup kcomps kcomp_effector
     */
    class AxisEffector
        : public Effector< Expects<NoInput>, Expects<NoModel>, Expects<AxisOutput>, MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                           , ExecutionExtension
#endif
                                                           >::CommonBase >
    {
    public:
        typedef Effector< Expects<NoInput>, Expects<NoModel>, Expects<AxisOutput>, MakeFacet<KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                                          , ExecutionExtension
#endif
                                                          >::CommonBase > Base;
        typedef AxisOutput OutputDataObject;

        /**
         * @brief Create an Axis effector for Motion Control.
         *
         * @param max_chan The number of channels available in the
         *        output DataObject.
         */
        AxisEffector(int max_chan = 1, const std::string& name = "AxisEffector" );

        virtual bool componentStartup();

        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push();

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Output and a
         * Drive Enable Output
         *
         * A DigitalOutput is created with name "\a name.Drive" representing
         * the Drive enable.
         *
         * A DigitalOutput is created with name "\a name.Break" representing
         * the Drive mechanical break.
         *
         * A Drive Output is created with name "\a name_Velocity" representing
         * the Drive velocity.
         */
        bool addAxis( const std::string& name, ORO_DeviceDriver::Axis* ax );

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Output.
         *
         * A Drive Output is created with name "\a name_Velocity" representing
         * the Drive velocity.
         */
        bool addAxis( const std::string& name, ORO_DeviceInterface::AxisInterface* ax );

        /**
         * @brief Add an Axis object with a name on a Channel
         *
         * A Channel is added representing the Drive velocity on channel \a virt_channel.
         */
        bool addAxisOnChannel( const std::string& name, int virt_channel );

        /**
         * @brief Remove a previously added Axis.
         */
        bool removeAxis( const std::string& name );

        /** 
         * @brief Remove an Axis from a Channel
         * 
         * @param name The Axis to remove from a channel.
         */
        void removeAxisFromChannel( const std::string& name );

        /**
         * @name The AxisEffector Methods.
         */
        // @{
        /**
         * @brief Enable an Axis.
         */
        bool enableAxis( const std::string& name );

        /**
         * @brief Disable an Axis.
         */
        bool disableAxis( const std::string& name );

        /**
         * @brief Stop an Axis.
         */
        bool stopAxis( const std::string& name );

        /**
         * @brief Switch on a Digital Output.
         */
        bool switchOn( const std::string& name );
                    
        /**
         * @brief Switch off a Digital Output.
         */
        bool switchOff( const std::string& name );
           
        // @}

    protected:
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        MethodFactoryInterface* createMethodFactory();
#endif
            
        /**
         * Write to Data to AnalogDrives.
         */
        void write_to_drive( pair<std::string, pair<ORO_DeviceInterface::AxisInterface*, DataObjectInterface<double>* > > dd );

        /**
         * Check if the Output DataObject lacks a user requested AnalogDrive.
         */
        bool lacksDrive( pair<std::string,pair<ORO_DeviceInterface::AxisInterface*, DataObjectInterface<double>* > > dd );
            
        Property<int> max_channels;

        std::vector< ORO_DeviceInterface::AxisInterface* > channels;
        std::vector<double> chan_out;
        DataObjectInterface< std::vector<double> >* chan_DObj;

        std::map<std::string, DigitalOutput* > d_out;

        std::map<std::string, pair<ORO_DeviceInterface::AxisInterface*, DataObjectInterface<double>* > > drive;
        std::map<std::string, pair<ORO_DeviceInterface::AxisInterface*, int> > axes;
        
        int usingChannels;
    };
}
#endif

