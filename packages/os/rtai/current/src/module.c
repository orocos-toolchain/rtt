/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:34:57 CEST 2002  module.c 

                       module.c -  description
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


#include <linux/kernel.h>
#include <linux/module.h>
#include "Framework.hpp"

// Do not include fosi.h to avoid multiple definitions !
#include "os/fosi.h"

/*
extern void __do_global_ctors( void );
extern void __do_global_dtors( void );

int rtos_printf( const char* c, ... );
*/

int init_module( void )
{
    DO_GLOBAL_CTORS();

    initFramework();
    rtos_printf( "\nCore loaded\n\n" );
    return 0;
}

void cleanup_module( void )
{
    terminateFramework();
    DO_GLOBAL_DTORS();
    rtos_printf( "\nCore unloaded\n\n" );
}


