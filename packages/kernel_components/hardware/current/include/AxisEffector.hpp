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
     * We can read integers, doubles and booleans.
     */
    struct AxisOutput
        : public ServedTypes<unsigned int, int, double, bool, std::vector<double> >
    {};

    /**
     * An Effector using the ORO_DeviceDriver Axis implementation
     * for machine or robot axes.
     *
     * @see ControlKernel
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
            if (
                std::count_if( drive.begin(), drive.end(), bind( &AxisEffector<Base>::lacksDrive, this, _1 ) ) ||
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
                    channels[i]->driveSet( chan_out[i] );
        }

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Output and a
         * Drive Enable Output
         *
         * A DigitalOutput is created with name "<name>.Drive" representing
         * the Drive enable.
         * A DigitalOutput is created with name "<name>.Break" representing
         * the Drive mechanical break.
         * A Drive Output is created with name "<name>.Velocity" representing
         * the Drive velocity.
         */
        bool addAxis( const std::string& name, Axis* ax )
        {
            if ( axes.count(name) != 0 || kernel()->isRunning() )
                return false;

            axes[name] = make_pair(ax, -1);

            d_out[ name + ".Drive" ] = ax->driveGet()->enableGet();
            d_out[ name + ".Break" ] = ax->driveGet()->breakGet();
            // we will fill in the dataobject pointer in componentStartup()
            DataObjectInterface<double>* tmp = 0;
            drive[name + ".Velocity" ] = make_pair( ax->driveGet(), tmp);

            return true;
        }

        /**
         * @brief Add an Axis object with a name on a Channel
         *
         * A DigitalOutput is created with name "<name>.Drive" representing
         * the Drive enable.
         * A Channel is added representing the Drive velocity on channel <virt_channel>.
         */
        bool addAxisChannel(int virt_channel, const std::string& name, Axis* ax )
        {
            if ( channels[virt_channel] != 0 || axes.count(name) != 0 || kernel()->isRunning() )
                return false;

            if ( !addAxis( name, ax) )
                return false;

            axes[name].second = virt_channel;
            channels[virt_channel] = ax->driveGet();

            return true;
        }

        bool removeAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 || kernel()->isRunning() )
                return false;

            int channr = axes[name].second;
            if ( channr != -1)
                channels[channr] = 0;

            drive.erase( name + ".Velocity" );
            d_out.erase( name + ".Drive" );
            d_out.erase( name + ".Break" );

            axes.erase(name);
            return true;
        }

        void enableAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 )
                return;
            axes[name]->enable();
        }

        void disableAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 )
                return;
            axes[name]->disable();
        }

    protected:
            
        /**
         * Write to Data to AnalogDrives.
         */
        void write_to_drive( pair<std::string, pair<AnalogDrive*, DataObjectInterface<double>* > > dd )
        {
            dd.second.first->driveSet( dd.second.second->Get() );
        }

        /**
         * Check if the Output DataObject lacks a user requested AnalogDrive.
         */
        bool lacksDrive( pair<std::string,pair<AnalogDrive*, DataObjectInterface<double>* > > dd )
        {
            // fill in the dataobject, if it fails, abort startup...
            if ( !Base::Output::dObj()->Get(dd.first, dd.second.second) )
                return true; // signals it went wrong
            return false;
        }
            
        Property<int> max_channels;

        std::vector< AnalogDrive* > channels;
        std::vector<double> chan_out;
        DataObjectInterface< std::vector<double> >* chan_DObj;

        std::map<std::string, DigitalOutput* > d_out;

        std::map<std::string, pair<AnalogDrive*, DataObjectInterface<double>* > > drive;
        std::map<std::string, pair<Axis*, int> > axes;
        
        int usingChannels;
    };

}
#endif

