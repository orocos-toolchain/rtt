/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:17 CET 2005  schunksensor.h 

                        schunksensor.h -  description
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
 
 
#ifndef SCHUNKSENSOR_H
#define SCHUNKSENSOR_H

#include <os/fosi.h>
#include <linux/comedilib.h>

unsigned int subd = 0;
unsigned int maxdata = 0;
comedi_t *it;

//array which stores the last X,Y,Z,Xtorque,Ytorque and Ztorque
unsigned int lastVal[ 6 ] = {0, 0, 0, 0, 0, 0};

//opens a comedi device with devminor 'devminor' and subdevice 'subd'
int initSensor( unsigned int devminor, unsigned int subd );
int readChannel( unsigned int chan, unsigned int range, unsigned int aref, int* value );

//Read all 6 channels and store them
int readAllChannels( void );

//get the last X,Y,Z,.. read in readAllChannels()
unsigned int getLastXval( void );
unsigned int getLastYval( void );
unsigned int getLastZval( void );
unsigned int getLastXtorqueval( void );
unsigned int getLastYtorqueval( void );
unsigned int getLastZtorqueval( void );

#endif //SCHUNKSENSOR_H
