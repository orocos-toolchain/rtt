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
