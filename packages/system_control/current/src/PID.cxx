/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  PID.cxx 

                        PID.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
/*
* PID controller with :
*   - set point weighting
*   - limitation of derivative gain
*   - bumpless parameter changes
*   - anti-windup protection
*
* AUTHOR : Erwin Aertbelien <Erwin.Aertbelien@mech.kuleuven.ac.be>
*/

/* $Id: PID.cpp,v 1.14 2003/06/17 19:08:44 psoetens Exp $
 * $Log: PID.cpp,v $
 * Revision 1.14  2003/06/17 19:08:44  psoetens
 * renaming of ise to ize (american spelling)
 *
 * Revision 1.13  2002/11/19 19:49:33  psoetens
 * adapted PID to new servoloop interface
 *
 * Revision 1.12  2002/11/13 16:30:23  rwaarsin
 * Adapted ServoLoop to the new situation (ZeroTime/ZeroLatency)
 *
 * Revision 1.11  2002/10/28 12:59:15  psoetens
 * fixed possible bug in ? : statement
 *
 * Revision 1.10  2002/10/01 17:01:39  pissaris
 * Major reindent and reformat of sourcecode.
 *
 * Revision 1.9  2002/10/01 12:50:54  pissaris
 * Conversion.
 *
 * Revision 1.8  2002/08/29 16:42:58  pissaris
 * double -> ValueType
 *
 * Revision 1.7  2002/07/23 18:35:15  psoetens
 * compiling KUKA
 * changed some lrl to make it more orcos compatible
 * solved the SignalTp mess (or not ..?)
 *
 * Revision 1.6  2002/07/16 09:15:27  rwaarsin
 * Bugje: pointers measurement en ref_gen werden niet geinitializeerd
 *
 * Revision 1.5  2002/07/15 12:55:58  rwaarsin
 * New working PID
 *
*/

#include "corelib/PID.hpp"

namespace MotionControl
{

    PID::PID( RunnableTaskInterface* sli,

              const struct PIDAttr* attr )
            : _attr( *attr )
            , uI( 0.0 )
            , uD( 0.0 )
            , uP( 0.0 )
            , ref_gen( 0 )
            , measurement( 0 )
    {
        T = sli->getPeriod();          // sample period

        // discrete time parameters
        bi = _attr.K * T / _attr.Ti;
        ad = ( 2*_attr.Td - _attr.N*T ) / ( 2*_attr.Td + _attr.N*T );
        bd = ( 2*_attr.K * _attr.N*_attr.Td ) / ( 2*_attr.Td + _attr.N*T );
        a0 = T / _attr.Tt;
    }

    PID::~PID()
    {}

    void

    PID::change( const struct PIDAttr& newAttr )
    // Change the PID parameters 'bumpless'
    {
        // bumpless parameter changes
        uI += _attr.K * ( y_setp - y ) + newAttr.K * ( y_setp - y );
        _attr = newAttr;

        // Recalculate the discrete time parameters
        // The parameters for the integral action (if Ti == 0, no integral action)

        if ( _attr.Ti != 0 )
            bi = _attr.K * T / _attr.Ti;
        else
            bi = 0.0;

        // The parameters for the derivative action (if Td == 0, no derivative action)
        if ( _attr.Td != 0 )
        {
            ad = ( 2 * _attr.Td - _attr.N * T ) / ( 2 * _attr.Td + _attr.N * T );
            bd = ( 2 * _attr.K * _attr.N * _attr.Td ) / ( 2 * _attr.Td + _attr.N * T );
            a0 = T / _attr.Tt;
        }

        else
        {
            ad = 0.0;
            bd = 0.0;
            a0 = 0.0;
        };
    }

    void
    PID::outputGet( ValueType& steeringSignal )
    // Calculate the PID and put the output in ppl
    {
        // Get the current measurement and setpoint
        if ( ref_gen != 0 )  ref_gen->outputGet( y_setp ); else y_setp = 0;
        if ( measurement != 0 )  measurement->outputGet( y ); else y = 0;

        // Compute the proportional
        uP = _attr.K * ( y_setp - y );
        // update derivative part

        if ( _attr.Td != 0 )
            uD = ad * uD - bd * ( y - y_old );

        // Compute control variable
        v = uP + uD + uI;

        // Check for saturation
        u = v;

        if ( v < _attr.minu )
            u = _attr.minu;

        if ( v > _attr.maxu )
            u = _attr.maxu;

        // Update integral part with windup protection
        if ( _attr.Ti != 0 )
            uI += bi * ( y_setp - y ) + a0 * ( u - v );

        y_old = y;

        // u = the steering signal
        steeringSignal = u;
    }

    void
    PID::init()
    // Initalizes the PID for usage
    {
        // Set y_old to the current value...
        if ( measurement != 0 )  measurement->outputGet( y ); else y = 0;
        y_old = y;
        // .. and the rest to zero;
        uI = 0;
        uD = 0;
    };

}
