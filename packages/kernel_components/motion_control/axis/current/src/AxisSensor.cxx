/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:06 CEST 2004  AxisSensor.cxx 

                        AxisSensor.cxx -  description
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
#include "kernel_components/AxisSensor.hpp"

namespace ORO_ControlKernel {

    AxisSensor::AxisSensor( int max_chan , const std::string& name ) 
        :  Base(name),
           max_channels("MaximumChannels","The maximum number of virtual analog channels", max_chan),
           axis_to_remove(""),
           usingChannels(0)
    {
        Axis* _a = 0;
        SensorInterface<double>* _d = 0;
        channels.resize(max_chan, std::make_pair(_d,_a) );
        chan_meas.resize(max_chan, 0.0);
    }

    bool AxisSensor::componentLoaded()
    {
        if ( !Base::Input::dObj()->Get("ChannelValues",chan_DObj) )
            return false;
        // kind-of resize of the vector in the dataobject:
        chan_DObj->Set(chan_meas); 

        // Get All DataObjects of Added Axes
        for(AxisMap::iterator ax= axes.begin(); ax != axes.end(); ++ax)
            {
                if ( axis_to_remove == ax->first )
                    continue; // detect removal of axis...
                DataObjectInterface<double>* d;
                if ( this->Base::Input::dObj()->Get( ax->first+".Velocity", d) == false )
                    std::cout << "AxisSensor::componentLoaded : Velocity of "+ax->first+" not found !"<<std::endl;

                drive[ ax->first ] = std::make_pair( ax->second->driveGet(), d );

                std::vector<std::string> res( ax->second->sensorList() );
                for ( std::vector<std::string>::iterator it = res.begin(); it != res.end(); ++it)
                    {
                        std::string sname( ax->first+"."+*it );
                        if ( this->Base::Input::dObj()->Get( sname, d ) == false )
                            std::cout << "AxisSensor::componentLoaded : "+*it+" of "+ax->first+" not found !"<<std::endl;
                        sensor[ sname ] = std::make_pair( ax->second->sensorGet( *it ),d);
                    }
            }

        return true;
    }

    void AxisSensor::componentUnloaded() {
        // Delete all Data Objects is done by DataObject Server
        // just cleanup stuff in opposite to componentLoaded
        for(AxisMap::iterator ax= axes.begin(); ax != axes.end(); ++ax)
            {
                drive.erase( ax->first );

                std::vector<std::string> res( ax->second->sensorList() );
                for ( std::vector<std::string>::iterator it = res.begin(); it != res.end(); ++it)
                    {
                        std::string sname( ax->first + "."+*it );
                        sensor.erase( sname );
                    }
            }
    }

    bool AxisSensor::componentStartup() {
        this->push();
        return true;
    }


    void AxisSensor::push()      
    {
        /*
         * Acces Sensor device drivers
         */
        std::for_each( drive.begin(), drive.end(), bind( &AxisSensor::drive_to_do, this, _1 ) );
        std::for_each( sensor.begin(), sensor.end(), bind( &AxisSensor::sensor_to_do, this, _1 ) );

        // Only write to channels if user requested so.
        if ( usingChannels ) {
            for (unsigned int i=0; i < channels.size(); ++i)
                chan_meas[i] = channels[i].first ? channels[i].first->readSensor() : 0 ;

            // writeout.
            chan_DObj->Set( chan_meas );
        }
    }

    bool AxisSensor::addAxis( const std::string& name, Axis* ax )
    {
        if ( !this->inKernel() || axes.count(name) != 0 || this->kernel()->isRunning() )
            return false;

        // no channel tied == -1
        axes[name] = ax;

        d_out[ name + ".Drive" ] = ax->driveGet()->enableGet();
        if ( ax->breakGet() )
            d_out[ name + ".Break" ] = ax->breakGet();
        if ( ax->homeswitchGet() )
            d_in[ name + ".Home" ] = ax->homeswitchGet();

        // Before Reload, Add All DataObjects :
        assert( this->Base::Input::dObj() );
        this->Base::Input::dObj()->addDouble(name+".Velocity");

        // Repeat for each additional sensor...
        std::vector<std::string> res( ax->sensorList() );
        for ( std::vector<std::string>::iterator it = res.begin(); it != res.end(); ++it)
            {
                std::string sname( name+"."+*it );
                this->Base::Input::dObj()->addDouble( sname );
            }

        if ( this->inKernel() )
            this->kernel()->reload(this);

        return true;
    }

    bool AxisSensor::addSensorOnChannel(const std::string& axis_name, const std::string& sensor_name, int virtual_channel )
    {
        if ( virtual_channel >= max_channels ||
             channels[virtual_channel].first != 0 ||
             axes.count(axis_name) != 1 ||
             axes[axis_name]->sensorGet( sensor_name ) == 0 ||
             this->kernel()->isRunning() )
            return false;

        ++usingChannels;
        // The owner Axis is stored in the channel.
        channels[virtual_channel] = std::make_pair( axes[axis_name]->sensorGet( sensor_name ), axes[axis_name] );
        return true;
    }

    void AxisSensor::removeSensorFromChannel( int virtual_channel )
    {
        if ( virtual_channel >= max_channels ||
             virtual_channel < 0 ||
             channels[virtual_channel].first == 0 ||
             this->kernel()->isRunning() )
            return;

        --usingChannels;

        Axis* _a = 0;
        SensorInterface<double>* _d = 0;
        // Reset the channel
        channels[virtual_channel] = std::make_pair( _d, _a);
    }

    bool AxisSensor::removeAxis( const std::string& name )
    {
        if ( axes.count(name) != 1 || this->kernel()->isRunning() )
            return false;

        for ( std::vector< std::pair< const SensorInterface<double>*, Axis* > >::iterator it = channels.begin();
              it != channels.end();
              ++it )
            if ( it->second == axes[name] )
                {
                    it->first = 0; // clear the channel occupied by an axis sensor
                    it->second = 0;
                }

        // cleanup the rest...
        d_out.erase( name + ".Drive" );
        d_out.erase( name + ".Break" );
        d_in.erase( name + ".Home" );

        // remove drive 
        this->Base::Input::dObj()->removeDouble(name+".Velocity");

        // Repeat for each additional sensor...
        std::vector<std::string> res( axes[name]->sensorList() );
        for ( std::vector<std::string>::iterator it = res.begin(); it != res.end(); ++it)
            {
                std::string sname( name+"."+*it );
                this->Base::Input::dObj()->removeDouble( sname );
            }

        // a bit of a hack, see componentLoaded :
        axis_to_remove = name;
        // reflect remove operation in DataObjects
        if ( this->inKernel() )
            this->kernel()->reload(this);

        // erase the axis as last, or the reload will not find it during cleanup
        axes.erase(name);
        axis_to_remove = "";
        return true;
    }

    bool AxisSensor::isEnabled( const std::string& name ) const
    {
        AxisMap::const_iterator it = axes.find(name);
        if ( it != axes.end() )
            return it->second->isEnabled();
        return false;
    }

    double AxisSensor::position( const std::string& name ) const
    {
        SensorMap::const_iterator it = sensor.find(name+".Position");
        if ( it != sensor.end() )
            return it->second.first->readSensor();
        return 0;
    }

    double AxisSensor::readSensor( const std::string& name ) const
    {
        SensorMap::const_iterator it = sensor.find(name);
        if ( it != sensor.end() )
            return it->second.first->readSensor();
        return 0;
    }

    bool AxisSensor::isOn( const std::string& name ) const
    {
        if ( d_in.count(name) == 1 )
            return d_in.find(name)->second->isOn();
        else if (d_out.count(name) == 1 )
            return d_out.find(name)->second->isOn();
        return false;
    }
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;

    DataSourceFactoryInterface* AxisSensor::createDataSourceFactory()
    {
        TemplateDataSourceFactory< AxisSensor >* ret =
            newDataSourceFactory( this );
        ret->add( "isOn", 
                  data( &AxisSensor::isOn,
                        "Inspect the status of a Digital Input or Output.",
                        "Name", "The Name of the Digital IO."
                        ) );
        ret->add( "position", 
                  data( &AxisSensor::position,
                        "Inspect the status of the Position of an Axis.",
                        "Name", "The Name of the Axis."
                        ) );
        ret->add( "readSensor", 
                  data( &AxisSensor::readSensor,
                        "Inspect the status of a Sensor of an Axis.",
                        "FullName", "The Name of the Axis followed by a '::' and the Sensor name (e.g. 'Position')."
                        ) );
        ret->add( "isEnabled", 
                  data( &AxisSensor::isEnabled,
                        "Inspect the status of an Axis.",
                        "Name", "The Name of the Axis."
                        ) );
        return ret;
    }
#endif
    void AxisSensor::drive_to_do( std::pair<std::string,std::pair<AnalogDrive*,
                                  DataObjectInterface<double>* > > dd )
    {
        dd.second.second->Set( dd.second.first->driveGet() );
    }
            
    void AxisSensor::sensor_to_do( std::pair<std::string,std::pair< const SensorInterface<double>*,
                                   DataObjectInterface<double>* > > dd )
    {
        dd.second.second->Set( dd.second.first->readSensor() );
    }

}
