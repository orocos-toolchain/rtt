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

#include "KryptonK600PositionInterfaceThread.hpp"
#include <corelib/Logger.hpp>
#include <cassert>

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtai_mbx.h>

#ifdef __cplusplus
}
#endif

using namespace ORO_CoreLib;


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
	Logger::log() << Logger::Error << "K600PositionInterface: How many Leds?????" << Logger::endl;
	Logger::log() << Logger::Error << "K600PositionInterface: Prepare for Segfault :-)" << Logger::endl;
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

    Logger::log() << Logger::Info << "K600PositionInterface: Constructor: Starting SingleThread..." << Logger::endl;
    // this->makeHardRealtime();
    this->start();
  };

  template <class Data>
  K600PositionInterface<Data>::~K600PositionInterface()
  {
    Logger::log() << Logger::Info << "K600PositionInterface: Destructor" << Logger::endl;
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
  void K600PositionInterface<Data>::loop()
  {
    SEM * udp_message_arrived;
    MBX * udp_message;
    int ret;
    #define BUFSIZE 1500
    char msg[MAX_MESSAGE_LENGTH];

    Logger::log() << Logger::Info << "K600PositionInterface: Step" << Logger::endl;

    // If kernel Module is not loaded yet, Print error message and
    // wait for one second
    while (! ((udp_message_arrived = (SEM *) rt_get_adr(nam2num("KEDSEM"))) && 
	      (udp_message = (MBX *) rt_get_adr(nam2num("KEDMBX")))	      )
	   )
      {
	Logger::log() << Logger::Info << "K600PositionInterface: Can't find sem/mbx; sleep one second" << Logger::endl;
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
	    if ((ret = rt_mbx_receive_if(udp_message,(void *) &msg, MAX_MESSAGE_LENGTH)) < 0 )
	      Logger::log() << Logger::Info << "K600PositionInterface: Error rcv message from mbx" << Logger::endl;
	    else
	      {
		// Interprete Message
		if ( (this->interprete_K600_Msg(msg,&a,&b,&c,&d,&mychar,&e,&f,&g,&_coordinates[0])) == 0 )
		  {
		    // #define __DEBUG__
#ifdef __DEBUG__
		    Logger::log() << Logger::Info << "K600PositionInterface: Received message from mbx" << Logger::endl;
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
		    // Switch buffers
		    switchBuffers();
		  }
		else Logger::log() << Logger::Info << "K600PositionInterface: Bad message, or something went wrong in decoding" << Logger::endl;
	      }
	  }
	// Sem was posted by destructor, so just leave the while loop
	else break;
      }
    Logger::log() << Logger::Info << "K600PositionInterface: Got signal from Destructor (or something went wrong), exiting Step" << Logger::endl;
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
