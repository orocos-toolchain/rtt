/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:17 CET 2005  serial_schunk.h 

                        serial_schunk.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef SERIALSCHUNK
#define SERIALSCHUNK

//#include <pthread.h>
#include <rt_com.h>
//#include <rt_comP.h>
#define SCHUNK_BUF_SIZE       RT_COM_BUF_SIZE


/**
 * The comport for the device (possible values: 0 and 1).
 */
#define COMPORT               0


/**
 * The name of the serial schunksensor device.
 */
#define SERIAL_NAME           "serialsensor"


typedef struct
{
    unsigned int value[ 6 ];
    int data_changed;
}

data_t;

/**
 * Return the last measured value for the X-force.
 */
unsigned int getLastXForce( void );

/**
 * Return the last measured value for the Y-force.
 */
unsigned int getLastYForce( void );

/**
 * Return the last measured value for the Z-force.
 */
unsigned int getLastZForce( void );

/**
 * Return the last measured value for the X-torque.
 */
unsigned int getLastXTorque( void );

/**
 * Return the last measured value for the Y-torque.
 */
unsigned int getLastYTorque( void );

/**
 * Return the last measured value for the Z-torque.
 */
unsigned int getLastZTorque( void );


#endif //SERIALSCHUNK
