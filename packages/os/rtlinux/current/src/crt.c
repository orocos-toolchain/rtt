/*
* Project: rtai_cpp - RTAI C++ Framework 
*
* File: $Id: crt.c,v 1.4 2003/03/31 16:07:36 psoetens Exp $
*
* Specialized bits of code needed to support construction and
* destruction of file-scope objects in C++ code.
*
* This code was partly taken from the GCC sources.
*
* Copyright:
*
* Copyright (C) 1991, 94, 95, 96, 97, 1998 Free Software Foundation, Inc.
*   Contributed by Ron Guilmette (rfg@monkeys.com).
* Copyright (C) 2002 Erwin Rol (erwin@muffin.org).
*
* Licence:
*           
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License 
* as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*
* As a special exception, if you link this library with files
* compiled with GCC to produce an executable, this does not cause
* the resulting executable to be covered by the GNU General Public License.
* This exception does not however invalidate any other reasons why
* the executable file might be covered by the GNU General Public License.  
*
*/

static int errno;
#define __KERNEL_SYSCALLS__

#include <linux/config.h> 
//#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <asm/unistd.h>
#include <linux/vmalloc.h>

//#include <rt_mem_mgr.h>

//extern int rt_printk(const char *format, ...);
extern int printk( const char *format, ... );

#ifndef CTORS_SECTION_ASM_OP
#define CTORS_SECTION_ASM_OP    ".section\t.ctors,\"aw\""
#endif
#ifndef DTORS_SECTION_ASM_OP
#define DTORS_SECTION_ASM_OP    ".section\t.dtors,\"aw\""
#endif

/*  Declare a pointer to void function type.  */
typedef void ( *func_ptr ) ( void );

#ifdef CRT_BEGIN

extern int atexit( func_ptr function );
extern void __do_atexit( void );
extern int __init_atexit( int max_size );

static func_ptr __DTOR_LIST__[];
static void
__do_global_dtors( void )
{
    static func_ptr * p = __DTOR_LIST__ + 1;
    static int completed = 0;

    printk( "GLOBAL DTORS START\n" );

    while ( *p )
    {
        p++;
        ( *( p - 1 ) ) ();
    }

    printk( "GLOBAL DTORS END\n" );

    completed = 1;
}

extern func_ptr __CTOR_LIST__[];
static void
__do_global_ctors( void )
{
    unsigned long nptrs = ( unsigned long ) __CTOR_LIST__[ 0 ];
    unsigned i;

    printk( "GLOBAL CTORS START\n" );

    if ( nptrs == ( unsigned long ) - 1 )
        for ( nptrs = 0; __CTOR_LIST__[ nptrs + 1 ] != 0; nptrs++ )

            ;

    for ( i = nptrs; i >= 1; i-- )
        __CTOR_LIST__[ i ] ();

    printk( "GLOBAL CTORS END\n" );
}

#ifdef USE_MAIN

extern int main( int argc, char* argv[] );

static int main_thread( void *unused )
{
    sigset_t tmpsig;
    int res = -1;

    sprintf( current->comm, "main" );
    daemonize();

    /* Block all signals except SIGKILL and SIGSTOP */
    spin_lock_irq( &current->sigmask_lock );
    tmpsig = current->blocked;
    siginitsetinv( &current->blocked, sigmask( SIGKILL ) | sigmask( SIGSTOP ) );
    recalc_sigpending( current );
    spin_unlock_irq( &current->sigmask_lock );

    __do_global_ctors();

    res = main( 0, 0 );

    __do_global_dtors();
    __do_atexit();

    return res;
}

#endif // USE_MAIN



// RTAI-- MODULE INIT and CLEANUP functions
/*
int __init startup_init(void)
{
 __init_atexit( 128 );
 
#ifdef USE_MAIN
 kernel_thread(main_thread, NULL , CLONE_FS | CLONE_FILES | CLONE_SIGHAND);
#else
 __do_global_ctors();
#endif
 return 0;
}
 
void startup_cleanup(void)
{
#ifdef USE_MAIN
 int waitpid_result = 1;
 
 while( waitpid_result > 0 )
  waitpid_result = waitpid(-1,NULL,__WCLONE|WNOHANG);
#else
 __do_global_dtors();
 __do_atexit();
#endif
}
 
module_init(startup_init)
module_exit(startup_cleanup)
*/ 
/* Force cc1 to switch to .data section.  */
static func_ptr force_to_data[ 0 ] __attribute__ ( ( __unused__ ) ) = { };

asm ( CTORS_SECTION_ASM_OP );     /* cc1 doesn't know that we are switching! */
static func_ptr __CTOR_LIST__[ 1 ] __attribute__ ( ( __unused__ ) )
= { ( func_ptr ) ( -1 ) };

asm ( DTORS_SECTION_ASM_OP );     /* cc1 doesn't know that we are switching! */
static func_ptr __DTOR_LIST__[ 1 ] __attribute__ ( ( __unused__ ) )
= { ( func_ptr ) ( -1 ) };

#else // CRT_BEGIN

/* Force cc1 to switch to .data section.  */
static func_ptr force_to_data[ 0 ] __attribute__ ( ( __unused__ ) ) = { };

asm ( CTORS_SECTION_ASM_OP );     /* cc1 doesn't know that we are switching! */
static func_ptr __CTOR_END__[ 1 ] __attribute__ ( ( __unused__ ) )
= { ( func_ptr ) 0 };

asm ( DTORS_SECTION_ASM_OP );     /* cc1 doesn't know that we are switching! */
static func_ptr __DTOR_END__[ 1 ] __attribute__ ( ( __unused__ ) )
= { ( func_ptr ) 0 };

#endif // CRT_BEGIN
