/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:06 CEST 2004  AxisEffector.cxx 

                        AxisEffector.cxx -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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

#pragma implementation
#include "control_kernel/AxisEffector.hpp"

namespace ORO_ControlKernel {

    using ORO_DeviceDriver::AnalogDrive;

    AxisEffector::AxisEffector(int max_chan, const std::string& name  ) 
        :  Base( name ),
           max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan),
           usingChannels(0)
    {
        channels.resize(max_chan, 0 );
        chan_out.resize(max_chan, 0);
    }

    bool AxisEffector::componentStartup()
    {
        // We check if the required DataObjects are present
        // they are introduced by the Controller component.
        // By PS : if a dataobject is not found, I do not care... I check for nulls everywhere.
        // otherwise each controller must provide a Axis.Drive dataobject, which eliminates
        // the use of channels. So : if it is there, I use it, otherwise not.
        std::count_if( drive.begin(), drive.end(), bind( &AxisEffector::lacksDrive, this, _1 ) );
        if (
            //std::count_if( drive.begin(), drive.end(), bind( &AxisEffector::lacksDrive, this, _1 ) ) != 0 ||
            ( usingChannels && ! Base::Output::dObj()->Get("ChannelValues", chan_DObj) ) )
            return false;
        return true;
    }

    void AxisEffector::push()      
    {
        /*
         * Access Drives, if a dataobject is present
         */
        std::for_each( drive.begin(), drive.end(), bind( &AxisEffector::write_to_drive, this, _1 ) );

        /* Access Drives if the user uses 'ChannelValues' */
        if (usingChannels ) {
            chan_DObj->Get( chan_out );

            // writeout.
            for (unsigned int i=0; i < channels.size(); ++i) {
                if ( channels[i] ) {
                    channels[i]->drive( chan_out[i] );
                }
            }
        }
    }

    bool AxisEffector::addAxis( const std::string& name, Axis* ax )
    {
        if ( axes.count(name) != 0 || this->kernel()->isRunning() )
            return false;

        axes[name] = make_pair(ax, -1);

        d_out[ name + ".Drive" ] = ax->getDrive()->enableGet();
        d_out[ name + ".Brake" ] = ax->getBrake();
        // we will fill in the dataobject pointer in componentStartup()
        DataObjectInterface<double>* tmp = 0;
        drive[name + ".Velocity" ] = make_pair( ax, tmp);

        return true;
    }

    bool AxisEffector::addAxisOnChannel( const std::string& name, int virt_channel )
    {
        if ( virt_channel >= max_channels ||
             channels[virt_channel] != 0 ||
             axes.count(name) == 0 ||
             this->kernel()->isRunning() )
            return false;

        ++usingChannels;
        axes[name].second = virt_channel;
        channels[virt_channel] = axes[name].first;

        return true;
    }

    void AxisEffector::removeAxisFromChannel( const std::string& name )
    {
        if (
             axes.count(name) == 0 ||
             axes[name].second == -1 ||
             this->kernel()->isRunning() )
            return;

        channels[ axes[name].second ] = 0;
        axes[name].second = -1;
        --usingChannels;
    }

    bool AxisEffector::removeAxis( const std::string& name )
    {
        if ( axes.count(name) != 1 || this->kernel()->isRunning() )
            return false;

        int channr = axes[name].second;
        if ( channr != -1)
            {
                channels[channr] = 0;
                --usingChannels;
            }

        drive.erase( name + ".Velocity" );
        d_out.erase( name + ".Drive" );
        d_out.erase( name + ".Brake" );

        axes.erase(name);
        return true;
    }

    void AxisEffector::enableAxis( const std::string& name )
    {
        if ( axes.count(name) != 1 )
            return;
        axes[name].first->stop();
    }

    void AxisEffector::disableAxis( const std::string& name )
    {
        if ( axes.count(name) != 1 )
            return;
        axes[name].first->lock();
    }

    void AxisEffector::switchOn( const std::string& name )
    {
        if (d_out.count(name) != 1)
            return;
        d_out[name]->switchOn();
    }
                    
    void AxisEffector::switchOff( const std::string& name )
    {
        if (d_out.count(name) != 1)
            return;
        d_out[name]->switchOff();
    }
           
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION

    using namespace ORO_Execution;

    MethodFactoryInterface* AxisEffector::createMethodFactory()
    {
        TemplateMethodFactory< AxisEffector >* ret =
            newMethodFactory( this );
        ret->add( "switchOn",
                  method( &AxisEffector::switchOn,
                           "Switch A Digital Output on",
                           "Name","The Name of the DigitalOutput."
                           ) ); 
        ret->add( "switchOff",
                  method( &AxisEffector::switchOff,
                           "Switch A Digital Output off",
                           "Name","The Name of the DigitalOutput."
                           ) ); 
        ret->add( "enableAxis",
                  method( &AxisEffector::enableAxis,
                           "Enable an Axis",
                           "Name","The Name of the Axis."
                           ) ); 
        ret->add( "disableAxis",
                  method( &AxisEffector::disableAxis,
                           "Disable (lock) an Axis",
                           "Name","The Name of the Axis."
                           ) ); 
        return ret;
    }
#endif
            
    void AxisEffector::write_to_drive( pair<std::string, pair<Axis*, DataObjectInterface<double>* > > dd )
    {
        // This only do something if the Controller component has an AxisName.Drive DataObject
        // If it does not lack the Axis.Drive dataobject :
        if ( dd.second.second != 0)
            dd.second.first->drive( dd.second.second->Get() );
    }

    bool AxisEffector::lacksDrive( pair<std::string,pair<Axis*, DataObjectInterface<double>* > > dd )
    {
        // fill in the dataobject, if it fails, abort startup...
        if ( !Base::Output::dObj()->Get(dd.first, dd.second.second) )
            return true; // signals it went wrong
        return false;
    }
}
