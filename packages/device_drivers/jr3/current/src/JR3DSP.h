#ifndef JR3DSP_H
#define JR3DSP_H

#include "JR3idm.h"

//-----------------------------------------------------------------------------
// Structures
struct ForceArrayScaled
{
  short Fx;
  short Fy;
  short Fz;
  short Tx;
  short Ty;
  short Tz;
  short V1;
  short V2;
};

struct ForceArray
{
  float Fx;
  float Fy;
  float Fz;
  float Tx;
  float Ty;
  float Tz;
};

struct SixAxisArray
{
  short axis_1;
  short axis_2;
  short axis_3;
  short axis_4;
  short axis_5;
  short axis_6;
};

//-----------------------------------------------------------------------------
// Accessible functions
void JR3DSP_check_sensor_and_DSP( unsigned int dsp );

void JR3DSP_set_offsets_scaled(const struct SixAxisArray* offsets, unsigned int dsp);
void JR3DSP_set_offsets(const struct ForceArray* offsets, unsigned int dsp);
// -----------------------------------
// Returns the error or the warning word of the sensor
unsigned short JR3DSP_get_error_word(unsigned int dsp);
unsigned short JR3DSP_get_warning_word(unsigned int dsp);

// Returns the transform nr currently in use
unsigned int JR3DSP_get_transform_nr(unsigned int dsp);

// -----------------------------------
// Functions to check/set the units (see man. pp 23)
int JR3DSP_isUnits_lbs_inLbs_mils(unsigned int dsp);
int JR3DSP_isUnits_N_dNm_mmX10(unsigned int dsp);
int JR3DSP_isUnits_kgF_kgFcm_mmX10(unsigned int dsp);
int JR3DSP_isUnits_klbs_kinlbs_mils(unsigned int dsp);

void JR3DSP_setUnits_lbs_inLbs_mils(unsigned int dsp);
void JR3DSP_setUnits_N_dNm_mmX10(unsigned int dsp);
void JR3DSP_setUnits_kgF_kgFcm_mmX10(unsigned int dsp);
void JR3DSP_setUnits_klbs_kinlbs_mils(unsigned int dsp);

// -----------------------------------
// Functions to get data directly from the filters
void JR3DSP_getDataFromFilter0_scaled(struct ForceArrayScaled* data, unsigned int dsp);
void JR3DSP_getDataFromFilter1_scaled(struct ForceArrayScaled* data, unsigned int dsp);
void JR3DSP_getDataFromFilter2_scaled(struct ForceArrayScaled* data, unsigned int dsp);
void JR3DSP_getDataFromFilter3_scaled(struct ForceArrayScaled* data, unsigned int dsp);
void JR3DSP_getDataFromFilter4_scaled(struct ForceArrayScaled* data, unsigned int dsp);
void JR3DSP_getDataFromFilter5_scaled(struct ForceArrayScaled* data, unsigned int dsp);
void JR3DSP_getDataFromFilter6_scaled(struct ForceArrayScaled* data, unsigned int dsp);

// -----------------------------------
// Functions to get the data from the filter, but also recalculate to 
// SI units
void JR3DSP_getDataFromFilter0(struct ForceArray* data, unsigned int dsp);
void JR3DSP_getDataFromFilter1(struct ForceArray* data, unsigned int dsp);
void JR3DSP_getDataFromFilter2(struct ForceArray* data, unsigned int dsp);
void JR3DSP_getDataFromFilter3(struct ForceArray* data, unsigned int dsp);
void JR3DSP_getDataFromFilter4(struct ForceArray* data, unsigned int dsp);
void JR3DSP_getDataFromFilter5(struct ForceArray* data, unsigned int dsp);
void JR3DSP_getDataFromFilter6(struct ForceArray* data, unsigned int dsp);
// ------------------------------------
// A rather specific tranformation: rotation and translation around the
// z-axix; (deg, m)
void JR3DSP_transformCoordinateSystem(float angle, float distance, unsigned int dsp);
#endif // JR3DSP_H
