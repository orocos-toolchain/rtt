/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:37 CEST 2002  EncoderAbsolute.cpp 

                       EncoderAbsolute.cpp -  description
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



#include "corelib/EncoderAbsolute.hpp"


namespace ORO_DeviceDriver
{
    const double PI = 3.14159265358979320;

    const double EncoderDataKuka361::CONV1 = 94.14706;
    const double EncoderDataKuka361::CONV2 = -103.23529;
    const double EncoderDataKuka361::CONV3 = 51.44118;
    const double EncoderDataKuka361::CONV4 = 175;
    const double EncoderDataKuka361::CONV5 = 150;
    const double EncoderDataKuka361::CONV6 = 131.64395;

    const int EncoderDataKuka361::ASOFFSET[ 6 ] =
        {
            1000004, 1000000, 1000002, 999995, 999048, 1322416
        };

    const double EncoderDataKuka361::Enc2Rad[ 6 ] =
        {
            2 * PI / EncoderDataKuka361::CONV1 / EncoderDataKuka361::ENC_RES,
            2 * PI / EncoderDataKuka361::CONV2 / EncoderDataKuka361::ENC_RES,
            2 * PI / EncoderDataKuka361::CONV3 / EncoderDataKuka361::ENC_RES,
            2 * PI / EncoderDataKuka361::CONV4 / EncoderDataKuka361::ENC_RES,
            2 * PI / EncoderDataKuka361::CONV5 / EncoderDataKuka361::ENC_RES,
            2 * PI / EncoderDataKuka361::CONV6 / EncoderDataKuka361::ENC_RES
        };

    // the weird ssi DataKuka361 mapping mapping[0] gives the
    // number of the encoder of the 1st axis.
    //const int EncoderDataKuka361::mapping[6] = { 5,2,4,1,3,0 };
    //const int EncoderDataKuka361::mapping[6]={2,0,1,1,2,0};

    //mapping niet nodig.
    const int EncoderDataKuka361::mapping[ 6 ] =
        {
            0, 1, 2, 0, 1, 2
        };

};
