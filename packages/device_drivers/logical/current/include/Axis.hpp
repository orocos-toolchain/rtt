/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:14 CEST 2002  Axis.hpp 

                       Axis.hpp -  description
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


#ifndef AXIS_HPP
#define AXIS_HPP

//#include <device_interfac/ActuatorInterface.hpp>
#include <device_interface/EncoderIncrementalInterface.hpp>
//#include <device_drivers/Encoder.hpp>
#include <can/Encoder.hpp>
//#include <fdi/SwitchHomingInterface.hpp>
#include "DigitalInput.hpp"
#include "Drive.hpp"

#include <corelib/EventListenerInterface.hpp>
#include <corelib/EventCompleterInterface.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <os/rtstreams.hpp>
#include <math.h>

namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;
    using namespace CAN;
    using namespace std;
    /**
     * Axis is an example of how you can easily implement
     * a generic axis using the FDI. Most people can use this class to see
     * how some tasks are combined in a component (the Axis) and that this
     * component can execute them on request. 
     *
     * Take a look at the ActuatorInterface, EncoderIncrementalInterface,
     * SwitchHomingInterface classes present in the fdi directory for 
     * the interfaces which your classes must implement so you can use Axis.
     *
     * A useful extension would be incorporating a control loop with a trajectory
     *  so that the homing is more 'professional'.
     *
     * example : 
     * my_AX = new Axis(SomeActuator,SomeEncoder,SomeHomingSwitch);
     */
    class Axis
    {
    public:

        /**
         * Constructs an Axis object and initializes the Actuator,Encoder and
         * Homing switch with the appropriate attributes.
         * 
         * @post the actuator is stopped
         */
        Axis( Drive* a, double _v_to_u,  Encoder* e, double _mm_to_inc,  DigitalInput* s ) 
            : act( a ), encoder( e ), swt( s ), v_to_u(_v_to_u), mm_to_inc(_mm_to_inc), posOffset(0)
        {
            act->disableDrive();
            this->reset();
        }

        virtual ~Axis();

        /**
         * Resets the component.
         * @post The actuator is stopped
         * @post Callibration is undone
         */
        void reset();

        void enable();

        void disable();

        void turnSet( int t );

        void move( double v );

        double positionGet();

        void positionSet( double newpos );
        
        void callibrate( double cal_pos, double direction);

        double velocity();

        /**
         * Returns the actuator used
         */
        Drive* actuatorGet();

        /**
         * Returns the encoder used
         */
        Encoder* encoderGet();

        /**
         * Returns the homing switch used
         */
        DigitalInput* switchGet();

    private:
        /**
         * Our actuator (motor)
         */
        Drive* act;
        /**
         * Our encoder
         */
        Encoder* encoder;

        /**
         * Our homing switch
         */
        DigitalInput* swt;

        double v_to_u;
        double mm_to_inc;
        double posOffset;

    };

}

#if 0

class HomeTask;
friend class HomeTask;

/**
 * The HomeTask will home the axis until an event is 
 * received from the homing switch. It will then reset the encoder
 * and stop the axis.
 */
class HomeTask : public TaskPreemptible, 
                 public EventListenerInterface,
                 public EventCompleterInterface
{
public:
    HomeTask(Axis* ax) 
        : TaskPreemptible(0.01), phase(0), parent(ax), k(0.0001), t(0)
    {
    }
    virtual bool initialize()
    {
        phase = 0;
        homingPos = 0;
        cout << "Homing..." << endl;
        parent->swt->homingEvent()->addHandler( this, this );
        double res = 0.05 * parent->act->minDriveGet(); // 5% of backward
        parent->act->drive( res );
        return true;
    }
    virtual void step() 
    {
        // you can put a trajectory here.
        if (phase == 1)
            {
                int cur_pos;
                parent->encincr->readCounter(cur_pos);
                int ref_pos = int( 500.0 * sin(2.*M_PI/10.*t) ); // sin goes forward then backward until negative.
                int e = ref_pos - cur_pos;
                cout <<"R: "<<ref_pos<<" t: "<<float(t)<<" cur: "<<cur_pos<<endl;
                parent->act->drive(k*e*parent->act->maxDriveGet() );
                t += 0.01;
            }
    }
    virtual void finalize()
    {
        parent->swt->homingEvent()->removeHandler( this, this );
        cout <<endl;
    }

    virtual void handleEvent( CallbackInterface* ei )
    {
        cout << "Homing Event Occured !\n";
        parent->encincr->readCounter( homingPos );
        ei->complete();
    }

    virtual void completeEvent()
    {
        parent->act->stop();     
        parent->encincr->reset(); 
                
        rtos_printf( "Completer : Homed at %d", homingPos );

        if ( phase == 0 )
            {
                t = 0;
                phase = 1;
            }
        else
            {
                phase = 0;
                this->stop();
            }
    }

protected:
    /**
     * A variable used for home()
     */
    int homingPos;

    int phase;

    Axis* parent;

    double k;
    double t;

} homeTask;

class CallibrationTask;
friend class CallibrationTask;

/**
 * The CallibrationTask simply measures drift and sets the offset of
 * the actuator accordingly.
 */
class CallibrationTask 
    : public TaskPreemptible
{
public:
    CallibrationTask(Axis* ax) : TaskPreemptible(0.01), parent(ax) {}

    virtual bool initialize()
    {
        encPos = 0; oldPos = 0; offset = 0; t = 0;
        parent->act->stop();
        parent->encincr->readCounter( oldPos );
        // this should take how many time ? is this present in comedi ?
        rtos_printf( "Starting Callibration.\n" );
        return true;
    }

    virtual void step()
    {
        parent->encincr->readCounter( encPos );

        if ( t < 100 )
            {
                if ( oldPos < encPos )
                    --offset; // go slower
                else if ( oldPos > encPos )
                    ++offset; // go faster
                        
                // apply changes
                parent->act->offsetSet( offset );
                parent->act->stop();
                        
                ++t;
                oldPos = encPos;
            }
        else
            stop();
    }
    virtual void finalize()
    {
        rtos_printf( "Offset : %d. Took me %d loops\n", offset, t );
    }
protected:
    Axis* parent;
    int encPos, oldPos, t, offset;
} calTask;


#endif


#endif

