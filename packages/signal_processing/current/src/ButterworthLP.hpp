/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ButterworthLP.hpp 

                        ButterworthLP.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 

#ifndef BUTTERWORTHLP_HPP
#define BUTTERWORTHLP_HPP

#include <math.h>

namespace MotionControl
{

/**
 * A second order low pass Butterworth Filter
 * @see http://kwon3d.com/theory/filtering/butt.html
 */
class ButterworthLP
{
public:
    /**
     * Create a 2nd order filter.
     *
     * @param f_sample
     *        The sample frequency of the data
     * @param f_cutoff
     *        The cutoff frequency of the filter
     */
    ButterworthLP( double f_sample, double f_cutoff)
        :N(2), fs(f_sample), fc(f_cutoff), fr(fs/fc), 
         x_1(0), x_2(0), xb_1(0), xb_2(0), sampleCount(0)
    {
        double k=0;
        double omegac=tan(M_PI / fr);
        double c_k = 1 + 2*cos(M_PI*(2*k+1)/(2*N))*omegac+omegac*omegac;
        a0 = a2 = omegac*omegac/c_k;
        a1 = 2*a0;
        b1 = 2*(omegac*omegac-1)/c_k;
        b2 = (1-2*cos(M_PI*(2*k+1)/2*N)*omegac+omegac*omegac)/c_k;
    }

    void reset()
    {
        sampleCount = 0;
    }

    /**
     * Filter the parameter value
     * 
     * @param x_0
     *        The value to be filtered
     * @return The filtered result
     */
    double filter(double x_0)
    {
        double res=0;
        ++sampleCount;

        if ( sampleCount < 3 )
            res = x_0;
        else
            res = a0*x_0+ a1*x_1 + a2*x_2 - b1*xb_1 -b2*xb_2;
        x_2 = x_1;
        x_1 = x_0;
        xb_2 = xb_1;
        xb_1 = res;
        return res;
    }

    
protected:
    double N;
    double fs;
    double fc;
    double fr;
    double a0,a1,a2;
    double b1,b2;

    double x_1, x_2;
    double xb_1, xb_2;

    int sampleCount;

};

}

#endif
