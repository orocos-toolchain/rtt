/***************************************************************************
  tag: Johan Rutgeerts  Mon Jan 10 15:59:16 CET 2005  jr3dsp_lxrt.c 

                        jr3dsp_lxrt.c -  description
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
 
 


/**
 * This file is intended to be compiled as a kernel module and export all the
 * device driver kernel functions to LXRT. One has to include devices_lxrt.h
 * in his lxrt program to interact with the device drivers.
 *
 * I will use LXRT index 15 and concentrate all device acces through this file.
 * Comedi is already supported in LXRT by the default rtai (from 2.4.11 on) package.
 *
 * Feb 2003 Johan Rutgeerts
 */


#include <linux/module.h>

#include "jr3dsp_lxrt.h"
#include <rtai.h>
#include <rtai_lxrt.h>


MODULE_LICENSE("GPL");
//EXPORT_NO_SYMBOLS;

/**
 * The first parameter is the bitmask and denotes what marshalling must
 * happen and if the call is realtime (bit 0), the second parameter is the function
 * to be called.
 */
static struct rt_fun_entry rt_apci_fun[] = {
#ifdef OROINT_DEVICE_DRIVERS_JR3
    [  JR3DSP_CHECK_SENSOR_AND_DSP] = { 0, JR3DSP_check_sensor_and_DSP},
    [  JR3DSP_SET_OFFSETS         ] = { UR1(1,3), JR3DSP_set_offsets},
    [  JR3DSP_GET_ERROR_WORD      ] = { 0, JR3DSP_get_error_word},
    [  JR3DSP_SETUNITS_N_DNM_MMx10] = { 0, JR3DSP_setUnits_N_dNm_mmX10},
    [  JR3DSP_GETDATAFROMFILTER0  ] = { UW1(1,3), JR3DSP_getDataFromFilter0},
    [  JR3DSP_GETDATAFROMFILTER1  ] = { UW1(1,3), JR3DSP_getDataFromFilter1},
    [  JR3DSP_GETDATAFROMFILTER2  ] = { UW1(1,3), JR3DSP_getDataFromFilter2},
    [  JR3DSP_GETDATAFROMFILTER3  ] = { UW1(1,3), JR3DSP_getDataFromFilter3},
    [  JR3DSP_GETDATAFROMFILTER4  ] = { UW1(1,3), JR3DSP_getDataFromFilter4},
    [  JR3DSP_GETDATAFROMFILTER5  ] = { UW1(1,3), JR3DSP_getDataFromFilter5},
    [  JR3DSP_GETDATAFROMFILTER6  ] = { UW1(1,3), JR3DSP_getDataFromFilter6},
    [  JR3DSP_TRANSFORMCOORDINATESYSTEM ] = { 0, JR3DSP_transformCoordinateSystem}
#endif //OROINT_DEVICE_DRIVERS_JR3
};



/* init module */
int init_module(void)
{

	if( set_rt_fun_ext_index(rt_apci_fun, MYIDX)) {
		printk("Recompile your module with a different index\n");
		return -EACCES;
	}

	return(0);
}

/*  cleanup module */
void cleanup_module(void)
{
	reset_rt_fun_ext_index(rt_apci_fun, MYIDX);
}


