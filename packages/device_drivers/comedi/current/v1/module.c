/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:45 CEST 2002  module.c 

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


#include "os/fosi.h" 
/*#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/modversions.h>
*/

extern void __do_global_ctors( void );
extern void __do_global_dtors( void );

int rtos_printf( const char* c, ... );

int init_module( void )
{
    __do_global_ctors();

    return 0;
}

void cleanup_module( void )
{
    __do_global_dtors();
}


