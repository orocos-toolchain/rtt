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
