/***************************************************************************
  tag: Johan Rutgeerts  Mon Jan 10 15:59:16 CET 2005  jr3dsp_lxrt.h 

                        jr3dsp_lxrt.h -  description
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
 
 

#ifndef JR3DSP_LXRT_H
#define JR3DSP_LXRT_H

#include <pkgconf/device_drivers_jr3.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef DECLARE
#define DECLARE static inline

// Need this for u32 etc type definitions
#define __KERNEL__
#include <asm/types.h>
#undef __KERNEL__

#ifdef OROINT_DEVICE_DRIVERS_JR3    
#include "jr3/jr3dsp.h"
#endif


// Every module that extends LXRT needs a unique MYIDX (1-15).
#define MYIDX			14

#define JR3DSP_CHECK_SENSOR_AND_DSP         60
#define JR3DSP_SET_OFFSETS                  61
#define JR3DSP_GET_ERROR_WORD               62
#define JR3DSP_SETUNITS_N_DNM_MMx10         63
#define JR3DSP_GETDATAFROMFILTER0           64
#define JR3DSP_GETDATAFROMFILTER1           65
#define JR3DSP_GETDATAFROMFILTER2           66
#define JR3DSP_GETDATAFROMFILTER3           67
#define JR3DSP_GETDATAFROMFILTER4           68
#define JR3DSP_GETDATAFROMFILTER5           69
#define JR3DSP_GETDATAFROMFILTER6           70
#define JR3DSP_TRANSFORMCOORDINATESYSTEM    71



#include <stdarg.h>
#define OROBLD_OS_LXRT_INTERNAL
#include <os/fosi.h>
#undef OROBLD_OS_LXRT_INTERNAL


union rtai_lxrt_t rtai_lxrt(short int dynx, short int lsize, int srq, void *arg);

#define SIZARG sizeof(arg)

    // hack until C++ inlining is fixed
#undef DECLARE
#define DECLARE static inline 


#ifdef OROINT_DEVICE_DRIVERS_JR3    
DECLARE void JR3DSP_check_sensor_and_DSP( unsigned int dsp )
{
  struct { unsigned int dsp; } arg = { dsp };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_CHECK_SENSOR_AND_DSP, &arg);
}

// UR1(1,3)
DECLARE void JR3DSP_set_offsets(const struct ForceArray* offsets, unsigned int dsp)
{
    struct ForceArray v_off;
    memcpy(&v_off, offsets, sizeof(struct ForceArray));
    struct { struct ForceArray* offsets; unsigned int dsp; unsigned int size; } arg = {&v_off, dsp, sizeof(struct ForceArray)};
    rtai_lxrt(MYIDX, SIZARG, JR3DSP_SET_OFFSETS, &arg);
}


DECLARE unsigned short JR3DSP_get_error_word(unsigned int dsp)
{
  unsigned short retval;
  struct { unsigned int dsp; } arg = { dsp };
  retval = rtai_lxrt(MYIDX, SIZARG, JR3DSP_GET_ERROR_WORD, &arg).i[LOW];
  return retval;
}

DECLARE void JR3DSP_setUnits_N_dNm_mmX10(unsigned int dsp)
{
  struct { unsigned int dsp; } arg = { dsp };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_SETUNITS_N_DNM_MMx10, &arg);
}

DECLARE void JR3DSP_getDataFromFilter0(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER0, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_getDataFromFilter1(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER1, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_getDataFromFilter2(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER2, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_getDataFromFilter3(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER3, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_getDataFromFilter4(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER4, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_getDataFromFilter5(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER5, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_getDataFromFilter6(struct ForceArray* data, unsigned int dsp)
{
  struct ForceArray pb_val;
  struct { struct ForceArray* data; unsigned int dsp; unsigned int size;} arg = { &pb_val, dsp, sizeof(struct ForceArray) };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_GETDATAFROMFILTER6, &arg);
  memcpy(data, &pb_val, sizeof(struct ForceArray) );
}

DECLARE void JR3DSP_transformCoordinateSystem(float angle, float distance, unsigned int dsp)
{
  struct { float angle; float distance; } arg = { angle, distance };
  rtai_lxrt(MYIDX, SIZARG, JR3DSP_TRANSFORMCOORDINATESYSTEM, &arg);
};
#endif //OROINT_DEVICE_DRIVERS_JR3    



/***************************************************************************
 ***************************************************************************
 **************************************************************************/


#ifdef __cplusplus
}
#endif // "C"

#endif
