// Copyright (C) 2003 Klaas Gadeyne <klaas dot gadeyne at mech dot kuleuven dot ac dot be>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

// #include <PMA_xytable_config.h>
#include "KryptonK600PositionInterfaceThread.hpp"
#include <cassert>

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtai_mbx.h>

#ifdef __cplusplus
}
#endif

namespace ORO_DeviceDriver
{
#define SINGLE_THREAD_PRIORITY 0 
// Same priority as the ZTT


  // Maximum Message length in bytes
  // Coordinates are expressed as doubles (8 bytes on i386)
#define MAX_MESSAGE_LENGTH (15 + MAX_NUM_COORD * 8)

  // Please see K600_UDP_Protocol.txt in this directory if you want
  // to understand the function below
  // TODO:  Add some checking to see if message was correct!
  template <class Data> int
  K600PositionInterface<Data>::interprete_K600_Msg(char * msg, 
						   unsigned short * a, 
						   unsigned short * b, 
						   unsigned short * c, 
						   unsigned short * d,
						   char * mychar, 
						   unsigned short * e, 
						   unsigned short * f, 
						   unsigned short * g, 
						   double** k)
  {
    unsigned short * unshtmp;
    char * chartmp;
    unsigned short i;
    unsigned int index = 15;
    unshtmp = (unsigned short *)&msg[0];
    *a = *unshtmp;
    unshtmp = (unsigned short *)&msg[2];
    *b = *unshtmp;
    unshtmp = (unsigned short *)&msg[4];
    *c = *unshtmp;
    unshtmp = (unsigned short *)&msg[6];
    *d = *unshtmp;

    chartmp = &msg[8];
    *mychar = *chartmp;

    unshtmp = (unsigned short *)&msg[9];
    *e = *unshtmp;
    unshtmp = (unsigned short *)&msg[11];
    *f = *unshtmp;
    unshtmp = (unsigned short *)&msg[13];
    *g = *unshtmp;

    // Remove this check after sending decent msg!
    if (*g < MAX_NUM_LEDS)
      {
	for (i = 0; i < *g * 3 ; i++)
	  {
	    *k = (double *) &msg[index];
	    index += 8; // A double is 8 bytes (chars)
	    ++k;
	  }
	return 0;
      }
    else 
      {
	rtos_printf("K600PositionInterface<Data>::How many Leds????? %d\n",*g);
	rtos_printf("Prepare for Segfault :-))\n");
	return -1;
      }
  }

  template <class Data>
  K600PositionInterface<Data>::K600PositionInterface(int num_leds)
    : ORO_OS::SingleThread(SINGLE_THREAD_PRIORITY,"KryptonEncoderThread"),
      _writeBuffer_p(&_buf1), _readBuffer_p(&_buf2), _num_leds(num_leds), _exit(false)
  {
    _buf1.resize(_num_leds * NUM_COORD_PER_LED);
    _buf2.resize(_num_leds * NUM_COORD_PER_LED);

    // Dirty code: Necessary??
    for (int i = 0; i < MAX_NUM_COORD; i++)
      {
	_coordinates[i] = new double;
      }

    rtos_printf( "\tK600PI::Constructor: Starting SingleThread...\n" );
    // this->makeHardRealtime();
    this->start();
  };

  template <class Data>
  K600PositionInterface<Data>::~K600PositionInterface()
  {
    rtos_printf( "\tK600PI::Destructor\n" );
    // In the while loop, we might be waiting for the sem, so first
    // post it...
    SEM * udp_message_arrived;
    this->_exit = true;
    udp_message_arrived = (SEM *) rt_get_adr(nam2num("KEDSEM"));
    rt_sem_signal (udp_message_arrived);
  }

  template <class Data>
  int K600PositionInterface<Data>::PositionGet(std::vector<Data> & p)
  {
    ORO_OS::MutexLock Locker(_readLock);
    p = *_readBuffer_p;
    return 0;
  }

  // Don't forget: Step of SingleThread is only executed once
  template <class Data>
  void K600PositionInterface<Data>::step()
  {
    SEM * udp_message_arrived;
    MBX * udp_message;
    int ret;
#define BUFSIZE 1500
    char msg[MAX_MESSAGE_LENGTH];

    rtos_printf("K600PositionInterface Step\n");

    // If kernel Module is not loaded yet, Print error message and
    // wait for one second
    while (! ((udp_message_arrived = (SEM *) rt_get_adr(nam2num("KEDSEM"))) && 
	      (udp_message = (MBX *) rt_get_adr(nam2num("KEDMBX")))	      )
	   )
      {
	rtos_printf("K600PositionInterface Step: Can't find sem/mbx; sleep one second\n");
	rt_sleep(nano2count(1000000000));
      }

    // Prepare for exit is set by the destructor of SingleThread

    // data from message
    unsigned short a,b,c,d,e,f,g; char mychar;
    while(!_exit)
      {
	// Wait until kernel Module posts semaphore
	rt_sem_wait(udp_message_arrived);

	// Check if we really got a message or if the
	// udp_message_arrived was posted by the destructor
	if (!_exit)
	  {
	    // rtos_printf("K600PositionInterface Step: Got UDP packet\n");
	    if ((ret = rt_mbx_receive_if(udp_message,(void *) &msg, MAX_MESSAGE_LENGTH)) < 0 )
	      rtos_printf("K600PositionInterface step: Error rcv message from mbx\n");
	    else
	      {
		// Interprete Message
		if ( (this->interprete_K600_Msg(msg,&a,&b,&c,&d,&mychar,&e,&f,&g,&_coordinates[0])) == 0 )
		  {
		    // #define __DEBUG__
#ifdef __DEBUG__
		    rtos_printf("Received message: %u, %u, %u, %u, %c, %u, %u, %u, ", a, b, c, d, mychar, e, f, g);
		    for (int i = 0 ; i < g*3 ; i++)
		      rtos_printf("%f, ",*(_coordinates[i]));
		    rtos_printf("\n");
#endif // __DEBUG__
		    // Check if message numleds corresponds to numleds given by user,
		    assert(_num_leds == g);

		    // Copy Data into write buffer
		    // TODO:  interprete_K600_msg should write immediately into
		    // write buffer!!
		    for (unsigned int i = 0; i < _writeBuffer_p->size() ; i++)
		      {
			// Maybe you want to convert to float?
			(*_writeBuffer_p)[i] = static_cast<Data>(*(_coordinates[i]));
		      }
		    // rtos_printf("K600PositionInterface::step() Buffer = %f\n", *_writeBuffer_p);
		    // Switch buffers
		    switchBuffers();
		  }
		else rtos_printf("K600PositionInterface step: Bad message, or something went wrong in decoding\n");
	      }
	  }
	// Sem was posted by destructor, so just leave the while loop
	else break;
      }
    rtos_printf("K600PositionInterface Step: Got signal from Destructor (or something went wrong), exiting Step\n");
  }

  template <class Data>
  void K600PositionInterface<Data>::switchBuffers()
  {
    // this is called *after* each refresh, so we know that it is safe
    // use spinlocks or implement an 'atomic' swing(b1,b2,p1,p2) function !!!
    ORO_OS::MutexTryLock locker( _readLock );
    if ( locker.isSuccessful() )
      if ( _readBuffer_p == &_buf1 )
	{
	  _readBuffer_p = &_buf2;
	  _writeBuffer_p = &_buf1;
	}
      else
	{
	  _readBuffer_p = &_buf1;
	  _writeBuffer_p = &_buf2;
	}
  }

};
