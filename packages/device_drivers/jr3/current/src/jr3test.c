/***************************************************************************
  tag: Johan Rutgeerts  Mon Jan 10 15:59:16 CET 2005  jr3test.c 

                        jr3test.c -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Johan Rutgeerts
    email                : johan.rutgeerts@mech.kuleuven.ac.be
 
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
 
 
#include <linux/init.h>
#include <linux/module.h>

void JR3DSP_check_sensor_and_DSP( unsigned int dsp );

int __init jr3check_init_module(void)
{
    JR3DSP_check_sensor_and_DSP( 0 );
    JR3DSP_check_sensor_and_DSP( 1 );
    return 0;
}


void __exit jr3check_cleanup_module(void)
{
}


module_init(jr3check_init_module);
module_exit(jr3check_cleanup_module);


