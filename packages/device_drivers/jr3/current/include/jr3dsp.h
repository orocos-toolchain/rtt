/***************************************************************************
  tag: Johan Rutgeerts  Mon Jan 10 15:59:16 CET 2005  jr3dsp.h 

                        jr3dsp.h -  description
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
 
 
#ifndef JR3DSP_H
#define JR3DSP_H

/**
 * An array of measured forces.
 */
struct ForceArray
{
  float Fx;
  float Fy;
  float Fz;
  float Tx;
  float Ty;
  float Tz;
};


#endif
