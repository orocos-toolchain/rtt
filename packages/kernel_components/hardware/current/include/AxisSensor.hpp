#ifndef AXIS_SENSOR_HPP
#define AXIS_SENSOR_HPP

#include <device_drivers/Axis.hpp>

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
     * We can write doubles.
     */
    struct AxisInput
        : public ServedTypes<double, std::vector<double> >
    {
        AxisInput()
        {
            this->insert( make_pair(1,"ChannelMeasurements") );
        }
    };

    /**
     * An Axis Reading Sensor for Motion Control.
     * @todo :
     * Better channel<->sensor assignment
     * Break will move from Drive to Axis.       
     */
    template <class Base>
    class AxisSensor
        : public Base
    {
    public:
        typedef AxisInput InputDataObject;
        typedef std::vector<double> ChannelType;

        /**
         * Create a Sensor with maximum <max_chan> virtual channels in "ChannelMeasurements" and
         * an unlimited number of DataObjects representing analog/digital channels
         */
        AxisSensor( int max_chan = 1) 
            :  Base("AxisSensor"),
               max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan)
        {
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
             * Acces Sensor device drivers
             */
            std::for_each( drive.begin(), drive.end(), bind( &AxisSensor<Base>::drive_to_do, this, _1 ) );
            std::for_each( sensor.begin(), sensor.end(), bind( &AxisSensor<Base>::sensor_to_do, this, _1 ) );

            // gather results.
            for (unsigned int i=0; i < channels.size(); ++i)
                chan_meas[i] = channels[i] ? channels[i]->readSensor() : 0 ;

            // writeout.
            chan_DObj->Set( chan_meas );
        }

        /**
         * @brief Add an Axis object with a name and create a Drive Velocity Input and
         * all Axis status info (homed, enabled, breaked, ...)
         *
         */
        bool addAxisDrive( const std::string& name, Axis* ax )
        {
            if ( axes.count(name) != 0 || kernel()->isRunning() )
                return false;

            // no channel tied == -1
            axes[name] = make_pair(ax, -1);

            d_out[ name + ".Drive" ] = ax->driveGet()->enableGet();
            d_out[ name + ".Break" ] = ax->driveGet()->breakGet();
            d_in[ name + ".Home" ] = ax->switchGet();

            // Create the dataobjects in the Input DO.
            typedef typename Base::Input::DataObject<double>::type doubleType;
            drive[ name ] = make_pair( ax->driveGet(), new doubleType(name+".Velocity") );
            Base::Input::dObj()->reg( drive[ name ].second );

            sensor[ name ] = make_pair( ax->sensorGet("Position"), new doubleType( name+".Position"));
            Base::Input::dObj()->reg( sensor[ name ].second );
            return true;
        }

        /**
         * @brief Add an Axis object with a name on a Channel.
         *
         * A Channel is added representing the Position on channel <virt_channel>.
         */
        bool addAxisChannel(int virt_channel, const std::string& name, Axis* ax )
        {
            if ( channels[virt_channel] != 0 || axes.count(name) != 0 || kernel()->isRunning() )
               return false;

            if ( addAxisDrive( name , ax) )
                {
                    axes[name].second = virt_channel;            // store the channel number
                    channels[virt_channel] = sensor[ name ].first;
                    return true;
                }
            return false;
        }

        bool removeAxis( const std::string& name )
        {
            if ( axes.count(name) != 1 || kernel()->isRunning() )
                return false;

            int channr = axes[name].second;
            if ( channr != -1)
                channels[channr] = 0;

            drive.erase( name );
            sensor.erase( name );
            d_out.erase( name + ".Drive" );
            d_out.erase( name + ".Break" );
            d_in.erase( name + ".Home" );

            axes.erase(name);
            return true;
        }

        bool isEnabled( const std::string& name )
        {
            return drive[name]->enableGet()->isOn();
        }

        double position( const std::string& name )
        {
            return sensor[name]->readSensor();
        }

        bool status( const std::string& name )
        {
            if ( d_in.count(name) == 1 )
                return d_in[name];
            else if (d_out.count(name) == 1 )
                return d_out[name];
            return false;
        }

    protected:

        /**
         * Write Analog input to DataObject.
         */
        void drive_to_do( std::pair<std::string,pair<AnalogDrive*,
                          DataObjectInterface<double>* > > dd )
        {
            dd.second.second->Set( dd.second.first->driveGet() );
        }
            
        void sensor_to_do( std::pair<std::string,pair<SensorInterface<double>*,
                           DataObjectInterface<double>* > > dd )
        {
            dd.second.second->Set( dd.second.first->readSensor() );
        }
            
        Property<int> max_channels;

        std::vector< SensorInterface<double>* > channels;
        ChannelType chan_meas;
        DataObjectInterface< ChannelType >* chan_DObj;

        std::map<std::string, DigitalInput* > d_in;
        std::map<std::string, DigitalOutput* > d_out;

        std::map<std::string, pair<AnalogDrive*, DataObjectInterface<double>* > > drive;
        std::map<std::string, pair<SensorInterface<double>*, DataObjectInterface<double>* > > sensor;
        std::map<std::string, pair<Axis*, int> > axes;
        
    };

}

#endif

