/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  AxisEffector.hpp 

                        AxisEffector.hpp -  description
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
 
 
#ifndef AXIS_EFFECTOR_HPP
#define AXIS_EFFECTOR_HPP

#include <device_drivers/AnalogOutput.hpp>
#include <device_drivers/DigitalOutput.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include <map>
#include <control_kernel/DataServer.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif
    using namespace ORO_DeviceInterface;
    using namespace ORO_DeviceDriver;
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
    template <class Base>
    class AxisEffector
        : public Base
    {
    public:
        typedef AxisOutput OutputDataObject;

        /**
         * Necessary typedefs.
         */
        typedef typename Base::OutputType OutputType;
            
        /**
         * @brief Create an Axis effector for Motion Control.
         *
         * @param max_chan The number of channels available in the
         *        output DataObject.
         */
        AxisEffector(int max_chan = 1) 
            :  Base("AxisEffector"),
               max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan),
               usingChannels(0)
        {
            channels.resize(max_chan, 0 );
            chan_out.resize(max_chan, 0);
        }

        virtual bool componentStartup()
        {
            // We check if the required DataObjects are present
            // they are introduced by the Controller component.
            // By PS : if a dataobject is not found, I do not care... I check for nulls everywhere.
            // otherwise each controller must provide a Axis.Drive dataobject, which eliminates
            // the use of channels. So : if it is there, I use it, otherwise not.
            std::count_if( drive.begin(), drive.end(), bind( &AxisEffector<Base>::lacksDrive, this, _1 ) );
            if (
                //std::count_if( drive.begin(), drive.end(), bind( &AxisEffector<Base>::lacksDrive, this, _1 ) ) != 0 ||
                 ( usingChannels && ! Base::Output::dObj()->Get("ChannelValues", chan_DObj) ) )
                return false;
            return true;
        }

        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            /*
             * Acces Drives
             */
            std::for_each( drive.begin(), drive.end(), bind( &AxisEffector<Base>::write_to_drive, this, _1 ) );

            // gather results.
            chan_DObj->Get( chan_out );

            // writeout.
            for (unsigned int i=0; i < channels.size(); ++i)
                if ( channels[i] )
                    channels[i]->drive( chan_out[i] );
        }

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Output and a
         * Drive Enable Output
         *
         * A DigitalOutput is created with name "<name>.Drive" representing
         * the Drive enable.
         *
         * A DigitalOutput is created with name "<name>.Break" representing
         * the Drive mechanical break.
         *
         * A Drive Output is created with name "<name>.Velocity" representing
         * the Drive velocity.
         */
        bool addAxis( const std::string& name, Axis* ax )
        {
            if ( axes.count(name) != 0 || kernel()->isRunning() )
                return false;

            axes[name] = make_pair(ax, -1);

            d_out[ name + ".Drive" ] = ax->driveGet()->enableGet();
            d_out[ name + ".Break" ] = ax->breakGet();
            // we will fill in the dataobject pointer in componentStartup()
            DataObjectInterface<double>* tmp = 0;
            drive[name + ".Velocity" ] = make_pair( ax, tmp);

            return true;
        }

        /**
         * @brief Add an Axis object with a name on a Channel
         *
         * A DigitalOutput is created with name "<name>.Drive" representing
         * the Drive enable.
         *
         * A Channel is added representing the Drive velocity on channel <virt_channel>.
         */
        bool addAxisOnChannel( const std::string& name, int virt_channel )
        {
            if ( virt_channel >= max_channels ||
                 channels[virt_channel] != 0 ||
                 axes.count(name) == 0 ||
                 kernel()->isRunning() )
                return false;

            ++usingChannels;
            axes[name].second = virt_channel;
            channels[virt_channel] = axes[name].first;

            return true;
        }

        /**
         * @brief Remove a previously added Axis.
         */
        bool removeAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 || kernel()->isRunning() )
                return false;

            int channr = axes[name].second;
            if ( channr != -1)
                {
                    channels[channr] = 0;
                    --usingChannels;
                }

            drive.erase( name + ".Velocity" );
            d_out.erase( name + ".Drive" );
            d_out.erase( name + ".Break" );

            axes.erase(name);
            return true;
        }

        /**
         * @name The AxisEffector Commands.
         */
        // @{
        /**
         * @brief Enable an Axis.
         */
        void enableAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 )
                return;
            axes[name].first->stop();
        }

        /**
         * @brief Disable an Axis.
         */
        void disableAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 )
                return;
            axes[name].first->lock();
        }

        /**
         * @brief Switch on a Digital Output.
         */
        void switchOn( const std::string& name )
        {
            if (d_out.count(name) != 1)
                return;
            d_out[name]->switchOn();
        }
                    
        /**
         * @brief Switch off a Digital Output.
         */
        void switchOff( const std::string& name )
        {
            if (d_out.count(name) != 1)
                return;
            d_out[name]->switchOff();
        }
           
        // @}

    protected:
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER

        bool true_gen() const { return true; }

        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< AxisEffector<Base> >* ret =
                newCommandFactory( this );
            ret->add( "switchOn",
                      command( &AxisEffector<Base>::switchOn,
                               &AxisEffector<Base>::true_gen,
                               "Switch A Digital Output on",
                               "Name","The Name of the DigitalOutput."
                               ) ); 
            ret->add( "switchOff",
                      command( &AxisEffector<Base>::switchOff,
                               &AxisEffector<Base>::true_gen,
                               "Switch A Digital Output off",
                               "Name","The Name of the DigitalOutput."
                               ) ); 
            ret->add( "enableAxis",
                      command( &AxisEffector<Base>::enableAxis,
                               &AxisEffector<Base>::true_gen,
                               "Enable an Axis",
                               "Name","The Name of the Axis."
                               ) ); 
            ret->add( "disableAxis",
                      command( &AxisEffector<Base>::disableAxis,
                               &AxisEffector<Base>::true_gen,
                               "Disable (lock) an Axis",
                               "Name","The Name of the Axis."
                               ) ); 
            return ret;
        }
#endif
            
        /**
         * Write to Data to AnalogDrives.
         */
        void write_to_drive( pair<std::string, pair<Axis*, DataObjectInterface<double>* > > dd )
        {
            // If it does not lack the Axis.Drive dataobject :
            if ( dd.second.second != 0)
                dd.second.first->drive( dd.second.second->Get() );
        }

        /**
         * Check if the Output DataObject lacks a user requested AnalogDrive.
         */
        bool lacksDrive( pair<std::string,pair<Axis*, DataObjectInterface<double>* > > dd )
        {
            // fill in the dataobject, if it fails, abort startup...
            if ( !Base::Output::dObj()->Get(dd.first, dd.second.second) )
                return true; // signals it went wrong
            return false;
        }
            
        Property<int> max_channels;

        std::vector< Axis* > channels;
        std::vector<double> chan_out;
        DataObjectInterface< std::vector<double> >* chan_DObj;

        std::map<std::string, DigitalOutput* > d_out;

        std::map<std::string, pair<Axis*, DataObjectInterface<double>* > > drive;
        std::map<std::string, pair<Axis*, int> > axes;
        
        int usingChannels;
    };

}
#endif

