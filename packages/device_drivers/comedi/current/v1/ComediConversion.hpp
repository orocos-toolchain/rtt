/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediConversion.hpp 

                        ComediConversion.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 
 
#ifndef COMEDICONVERSION_HPP
#define COMEDICONVERSION_HPP

#ifdef __KERNEL__
extern "C"
{
#   include <linux/comedilib.h>
}

#else
#   include <comedilib.h>
#endif

class ComediConversion
{

    public:
        /*
         * Converts data given in sample values (lsampl_t, between 0 and maxdata) into physical units (double).
         * The parameter range represents the conversion information to use,
         * and the parameter maxdata represents the maximum possible data value for the channel that the data was read.
         * Conversion of endpoint sample values, that is, sample values equal to 0 or maxdata,
         * is affected by the Comedilib out-of-range behavior.
         * If the out-of-range behavior is set to COMEDI_OOR_NAN, endpoint values are converted to NAN.
         * If the out-of-range behavior is set to COMEDI_OOR_NUMBER, the endpoint values are converted similarly to other values.
         * If there is an error, NAN is returned.
         *
         *  @param data : the sample data to convert
         *  @param rng : range, conversion information
         *  @param maxdata : maximum possible data value for the channel that the data was read
         */ 
        //        double to_phys(lsampl_t data, comedi_range* rng, lsampl_t maxdata);

        /*
         * Converts data given in physical units (data) into sample values (lsampl_t, between 0 and maxdata).
         * The parameter rng represents the conversion information to use,
         * and the parameter maxdata represents the maximum possible data value for the channel that the data will be written to.
         * Conversion is not affected by out-of-range behavior.
         * Out-of-range data parameters are silently truncated to the range 0 to maxdata.
         *
         *  @param data : the physical data to convert
         *  @param rng : range, conversion information
         *  @param maxdata : maximum possible data value for the channel that the data was read
         */ 
        //        lsampl_t from_phys(double data, comedi_range* rng, lsampl_t maxdata);

        /*
         * Convert n samples to physical values.
         * The amount of oor values (out-of-range) is returned.
         * 
         *  @param dest : the location to store the results from the conversion
         *  @param dst_stride : destination step size ?????
         *  @param src : the source data samples
         *  @param src_stride : source step size ?????
         *  @param rng : range, conversion information
         *  @param maxdata : maximum possible data value for the channel that the data was read
         *  @param n : amount of samples to convert
         */ 
        //int sampl_to_phys(double* dest, int dst_stride, sampl_t* src, int src_stride, comedi_range* rng, lsampl_t maxdata, int n);

        /*
         * Convert n physical values to samples.
         * The amount of out-of-range values is returned.
         *
         *  @param dest : the location to store the results from the conversion to samples
         *  @param dst_stride : destination step size ?????
         *  @param src : the source physical data
         *  @param src_stride : source step size ?????
         *  @param rng : range, conversion information
         *  @param maxdata : maximum possible data value for the channel that the data was read
         *  @param n : amount of physical values to convert
         */ 
        //int sample_from_phys(sampl_t* dest, int dst_stride, double* src, int src_stride, comedi_range* rng, lsampl_t maxdata, int n);

    private:
};

#endif
