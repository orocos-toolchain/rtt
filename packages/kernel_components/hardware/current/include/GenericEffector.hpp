#ifndef GENERIC_EFFECTOR_HPP
#define GENERIC_EFFECTOR_HPP

#include <device_drivers/AnalogOutput.hpp>
#include <device_drivers/DigitalOutput.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include <map>
#include <control_kernel/DataServer.hpp>

/**
 * @file GenericEffector.hpp
 * This file contains a for most applications
 * usefull Effector, which uses the Orocos Device
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
    

    /**
     * We can read integers, doubles and booleans.
     */
    struct GenericOutput
        : public ServedTypes<unsigned int, int, double, bool, std::vector<double> >
    {};

    /**
     * An Effector using the ORO_DeviceInterface devices
     * for accessing hardware devices. Off course, you
     * can also use simulated hardware.
     */
    template <class Base>
    class GenericEffector
        : public Base
    {
    public:
        typedef GenericOutput OutputDataObject;

        /**
         * Necessary typedefs.
         */
        typedef typename Base::OutputType OutputType;
            
        GenericEffector(int max_chan = 32) 
            :  Base("GenericEffector"),
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
            if ( std::count_if( a_out.begin(), a_out.end(), bind( &GenericEffector<Base>::lacksAOut, this, _1 ) ) ||
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
             * Acces Analog device drivers
             */
            std::for_each( a_out.begin(), a_out.end(), bind( &GenericEffector<Base>::write_to_aout, this, _1 ) );

            // gather results.
            chan_DObj->Get( chan_out );

            // writeout.
            for (unsigned int i=0; i < channels.size(); ++i)
                if ( channels[i] )
                    channels[i]->value( chan_out[i] );
        }

        bool addAnalogOutput( const std::string& name, AnalogOutInterface<unsigned int>* output, int channel, double offset=0.0, double scale=1.0 )
        {
            if ( a_out.count(name) != 0 || kernel()->isRunning() )
                return false;

            // we will fill in the dataobject pointer in componentStartup()
            DataObjectInterface<double>* tmp = 0;
            a_out[name] = std::make_pair( new AnalogOutput<unsigned int>( output, channel, offset, scale ), tmp);

            return true;
        }

        bool removeAnalogOutput( const std::string& name )
        {
            if ( a_out.count(name) != 1 || kernel()->isRunning() )
                return false;

            delete a_out[name].first;

            a_out.erase(name);
            return true;
        }

        bool addChannel( int virt_channel, AnalogOutInterface<unsigned int>* output, int channel, double offset=0.0, double scale=1.0 )
        {
            if ( channels[virt_channel] != 0 || kernel()->isRunning() )
                return false;

            // we will fill in the dataobject pointer in componentStartup()
            channels[virt_channel] = new AnalogOutput<unsigned int>( output, channel, offset, scale );
            ++usingChannels;
            return true;
        }

        bool removeChannel( int virt_channel )
        {
            if ( channels[virt_channel] == 0 || kernel()->isRunning() )
                return false;
            
            delete channels[virt_channel];
            --usingChannels;
            return true;
        }

        bool addDigitalOutput( const std::string& name, DigitalOutInterface* output, int channel, bool invert=false)
        {
            if ( d_out.count(name) != 0 || kernel()->isRunning() )
                return false;

            d_out[name] = new DigitalOutput( output, channel, invert );

            return true;
        }

        bool removeDigitalOutput( const std::string& name )
        {
            if ( d_out.count(name) != 1 || kernel()->isRunning() )
                return false;

            delete d_out[name];

            d_out.erase(name);
            return true;
        }

        void switchOn( const std::string& name )
        {
            if ( d_out.count(name) != 1 )
                return;
            d_out[name]->switchOn();
        }

        bool isOn( const std::string& name ) const
        {
            // use  std::logical_not<bool>() to detect isOff.
            if ( d_out.count(name) == 1 )
                return d_out[name]->isOn();
            return false;
        }

        bool isOff( const std::string& name ) const
        {
            // use  std::logical_not<bool>() to detect isOff.
            if ( d_out.count(name) == 1 )
                return !d_out[name]->isOn();
            return false;
        }

        void switchOff( const std::string& name )
        {
            if ( d_out.count(name) != 1 )
                return;
            d_out[name]->switchOff();
        }
        
    protected:

        /**
         * Write to Data to digital output.
         */
        void write_to_aout( std::pair<std::string, pair<AnalogOutput<unsigned int>*, DataObjectInterface<double>* > > dd )
        {
            dd.second.first->value( dd.second.second->Get() );
        }

        /**
         * Check if the Output DataObject lacks a user requested output.
         */
        bool lacksAOut( std::pair<std::string,pair<AnalogOutput<unsigned int>*, DataObjectInterface<double>* > > dd )
        {
            // fill in the dataobject, if it fails, abort startup...
            if ( !Base::Output::dObj()->Get(dd.first, dd.second.second) )
                return true; // signals it went wrong
            return false;
        }
            
        Property<int> max_channels;

        std::vector< AnalogOutput<unsigned int>* > channels;
        std::vector<double> chan_out;
        DataObjectInterface< std::vector<double> >* chan_DObj;

        std::map<std::string, DigitalOutput* > d_out;
        std::map<std::string, std::pair<AnalogOutput<unsigned int>*, DataObjectInterface<double>* > > a_out;
        
        int usingChannels;
    };

}
#endif

