/***************************************************************************
 tag: Peter Soetens  Mon Jun 17 11:55:32 CEST 2002  motioncontrol/src/PControl6D.hpp 

                       motioncontrol/src/PControl.hpp -  description
                          -------------------
   begin                : Mon June 17 2002
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





#ifndef PCONTROL6D_HPP
#define PCONTROL6D_HPP

#include "DiffModule.hpp"
#include "SignalTypes.hpp"
#include "MultiVector.hpp"

namespace MotionControl
{

    struct P6DAttr
    {
        Double6D K;            // proportional 6D
    };

    /**
     * A very simple Proportional control module
     */

    class PControl6D
                : public DiffModule <
                SignalTp<ReferenceSgn, Double6D>,
                SignalTp<MeasurementSgn, Double6D>,
                SignalTp<ControlSgn, Double6D> >
    {

        public:
            typedef InputOne RefInput;
            typedef InputTwo MesInput;
            /**
             * Creates a new Proportional controller
             * @param sci   The task running this module
             * @param attr The attributes of this control module
             */

            PControl6D( struct P6DAttr* attr )
                    : K( attr->K )
            {}

            virtual ~PControl6D()
            {}

            virtual void init()
            {}

            virtual void outputGet( OutputStruct& ppl )
            {
                SignalTp<ReferenceSgn, Double6D> r;
                SignalTp<MeasurementSgn, Double6D> m;
                one->outputGet( r );
                two->outputGet( m );

                ppl.value = ( r.value - m.value ) * K;
            }

        protected:
            Double6D K;
    };
}

#endif

