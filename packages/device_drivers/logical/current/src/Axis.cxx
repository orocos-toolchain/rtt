/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:43 CEST 2002  Axis.cpp 

                       Axis.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "device_drivers/Axis.hpp"




namespace ORO_DeviceDriver
{
    struct Axis_PositionSensor
        : public SensorInterface<double>
    {
        Axis* ax;
        double min;
        double max;

        Axis_PositionSensor(Axis* _ax, double _minpos, double _maxpos) : ax(_ax), min(_minpos), max(_maxpos) {}

        virtual int readSensor( double& p ) const
        {
            p = ax->positionGet();
            return 0;
        }

        void limit(double _min, double _max) 
        {
            min = _min;
            max = _max;
        }

        virtual double readSensor() const
        {
            return ax->positionGet();
        }

        virtual double maxMeasurement() const
        {
            return max;
        }

        virtual double minMeasurement() const
        {
            return min;
        }

        virtual double zeroMeasurement() const
        {
            return 0.0;
        }
    };


    using namespace ORO_CoreLib;

    Axis::Axis( AnalogDrive* a,  EncoderInterface* e, double _unit_to_inc,  DigitalInput* s) 
        : act( a ), encoder( e ), swt( s ), unit_to_inc(_unit_to_inc), posOffset(0), status(false)
    {
        this->reset();
        pos_sens = new Axis_PositionSensor(this, 0.0, 0.0);
        this->sensorSet("Position",pos_sens);
    }

    Axis::~Axis()
    {
        reset();
        delete pos_sens;
        delete act;
        delete encoder;
        delete swt;
    }

    void Axis::drive( double vel )
    {
        act->driveSet( vel );
    }

    void Axis::enable()
    {
        act->stop();
        status = true;
    }

    bool Axis::isEnabled()
    {
        return status;
    }

    void Axis::disable()
    {
        act->lock();
        status = false;
    }

    void Axis::positionLimits(double min, double max)
    {
        pos_sens->limit(min, max);
    }

    double Axis::positionGet()
    {
        return double( encoder->turnGet()*encoder->resolution() +  encoder->positionGet() ) / unit_to_inc + posOffset;
    }

    void Axis::sensorSet(const std::string& name,  SensorInterface<double>* _sens)
    {
        sens[name] = _sens;
    }

    SensorInterface<double>* Axis::sensorGet(const std::string& name)
    {
        if (sens.count(name) == 0)
            return 0;
        else
            return sens[name];
    }

    void Axis::positionSet( double newpos )
    {
        posOffset = newpos -  positionGet();
    }

    double Axis::calibrate( double calpos, double sign )
    {
        double currentPos;
        if ( sign < 0 )
            {
                if ( unit_to_inc > 0)
                    currentPos = calpos - ( encoder->resolution() - encoder->positionGet() ) / unit_to_inc;
                else
                    currentPos = calpos - encoder->positionGet() / unit_to_inc;
            }
        else
            {
                if ( unit_to_inc > 0)
                    currentPos =  calpos + encoder->positionGet() / unit_to_inc;
                else
                    currentPos = calpos + (encoder->resolution() - encoder->positionGet() ) / unit_to_inc;
            }
        double oldposition = positionGet();
        positionSet( currentPos );
        return positionGet() - oldposition;
    }

    void Axis::turnSet( int t )
    {
        encoder->turnSet( t );
    }

    void Axis::reset()
    { 
        act->driveSet( 0 );
        act->enableBreak();
        act->disableDrive();
    }

    AnalogDrive* Axis::driveGet()
    {
        return act;
    }

    EncoderInterface* Axis::encoderGet()
    {
        return encoder;
    }

    DigitalInput* Axis::switchGet()
    {
        return swt;
    }


};
