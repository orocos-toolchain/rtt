/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  PID.hpp 

                        PID.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 

/*
 * PID controller with :
 *   - limitation of derivative gain
 *   - bumpless parameter changes
 *   - anti-windup protection
 *   - you can turn the D and the I actions of by setting the Ti or Td to zero
 *
 * For references, see Control Handbook pp. 198-209
 * The naming of the variables is the same as in the handbook
 * Exceptions:
 *  - 'b' in handbook is not used (setpoint weighting)
 *  - 'h' in handbook is called 'T' (= sampleperiod)
 *
 */


/* $Id: PID.hpp,v 1.14 2003/08/20 12:04:40 rwaarsin Exp $
 * $Log: PID.hpp,v $
 * Revision 1.14  2003/08/20 12:04:40  rwaarsin
 * Switched to MotionKernel
 *
 * Revision 1.13  2003/06/17 19:08:44  psoetens
 * renaming of ise to ize (american spelling)
 *
 * Revision 1.12  2002/11/19 19:49:33  psoetens
 * adapted PID to new servoloop interface
 *
 * Revision 1.11  2002/11/13 16:30:23  rwaarsin
 * Adapted ServoLoop to the new situation (ZeroTime/ZeroLatency)
 *
 * Revision 1.10  2002/10/10 14:22:06  psoetens
 * Added licence to all headers
 *
 * Revision 1.9  2002/10/01 17:01:39  pissaris
 * Major reindent and reformat of sourcecode.
 *
 * Revision 1.8  2002/08/29 16:42:58  pissaris
 * double -> ValueType
 *
 * Revision 1.7  2002/08/07 11:21:48  psoetens
 * Builds for userspace and kernel space
 * loads for userspace and kernel space
 * many bugfixes.
 * 'ported' lrl to orocos style STL
 *
 * Revision 1.6  2002/07/23 18:35:15  psoetens
 * compiling KUKA
 * changed some lrl to make it more orcos compatible
 * solved the SignalTp mess (or not ..?)
 *
 */

#ifndef PID_HPP
#define PID_HPP

#if 0
#include "ControlModuleInterface.hpp"
#include "ServoLoop.hpp"
#include "SignalTypes.hpp"
#endif

#include <corelib/Property.hpp>

namespace MotionControl
{

    template <class Base>
    class PID : public Base
    {
      // The typedefs
      typedef typename Base::SetPointType SetPointType;
      typedef typename Base::InputType InputType;
      typedef typename Base::ModelType ModelType;
      typedef typename Base::OutputType OutputType;
      
      static const unsigned int NR_CHANNELS = InputType::size;
      public:
            PID() 
	    : _K("K", "Proportional gain", 2000)
            , _Ti("Ti", "Integrative time cte", 0.0228)
            , _Td("Td", "Derivative time cte", 0.00238)
            , _N("N", "Limitation of derivative gain (e.g. value 10)", 10)
            , _Tt("Tt", "Tracking time cte   Td<Tt<Ti, smaller is better", 0.00238)
            , _minu("min_u", "Minimal input", -10)
            , _maxu("max_u", "Maximal input", 10)
	    , _output("PID_output", "...")
	    {
            }

	    virtual ~PID() {}

            /**
             * This method is a hook which is called when the kernel
             * is startING.
             */

	    virtual void kernelStarted()
	    {
              std::cout << "TEST" << std::endl;
              T = kernel()->getPeriod();          // sample period
              // discrete time parameters
	      for (unsigned int i = 0; i < NR_CHANNELS; i++)
	      {
                bi[i] = _K.get().data[i]* T / _Ti.get().data[i];
                ad[i] = ( 2*_Td.get().data[i] - _N.get().data[i]*T ) / 
                        ( 2*_Td.get().data[i] + _N.get().data[i]*T );
                bd[i] = ( 2*_K.get().data[i] * _N.get().data[i]*_Td.get().data[i] ) /
                        ( 2*_Td.get().data[i] + _N.get().data[i]*T );
                a0[i] = T / _Tt.get().data[i];
	      }
	    }

#if 0
            void change( PIDAttr& attr );
            // Change the PID parameters 'bumpless'
#endif
            virtual void init()
            // Initalizes the PID for usage
	    {
              // Set y_old to the current value...
	      pull();
	      y_old = y;
              // .. and the rest to zero;
	      for (unsigned int i = 0; i< NR_CHANNELS; i++)
	      {
                uI[i] = 0;
                uD[i] = 0;
	      }
	    };

            virtual void pull()      
            {
	      // Remember the current value
	      y_old = y;
	      Base::Input::dObj()->Get(y);
	      Base::SetPoint::dObj()->Get(y_setp);
	    }
            
            // Here, the calculation of the new actuator command will be done
	    virtual void calculate()
            {
	      for(unsigned int i = 0; i< NR_CHANNELS; i++)
	      {
                // Compute the proportional
                uP[i] = _K.get().data[i] * ( y_setp.ref().data[i] - y.meas().data[i] );
                // update derivative part

                if ( _Td.get().data[i]  != 0 )
                  uD[i] = ad[i] * uD[i] - bd[i] * ( y.meas().data[i] - y_old.meas().data[i] );

                // Compute control variable
                v = uP[i] + uD[i] + uI[i];

                // Check for saturation
                u = v;

                if ( v < _minu.get().data[i]  )
                  u = _minu.get().data[i] ;

                if ( v > _maxu.get().data[i]  )
                  u = _maxu.get().data[i] ;

                // Update integral part with windup protection
                if ( _Ti.get().data[i]  != 0 )
                  uI[i] += bi[i] * ( y_setp.ref().data[i] - y.meas().data[i] ) + a0[i] * ( u - v );
                y_old.meas().data[i] = y.meas().data[i];

                // u = the steering signal
                z_output.actu().data[i] = u;
		_output.value() = z_output.actu();
              }
            }
            
            // Write the actuator command to the actuator
	    virtual void push()      
            {
              Base::Output::dObj()->Set(z_output);
	    }

            /**
             * @see MotionReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
		
                bag.add(&_output);
            }

            /**
             * @see MotionPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
		std::cerr << "Properties imported! : " << _K << _Ti << _Td << _N << _Tt << _minu << _maxu << std::endl;
                composeProperty(bag, _K);
                composeProperty(bag, _Ti); 
                composeProperty(bag, _Td); 
                composeProperty(bag, _N); 
                composeProperty(bag, _Tt); 
                composeProperty(bag, _minu);
                composeProperty(bag, _maxu);

		std::cout << "Properties imported! : " << _K << _Ti << _Td << _N << _Tt << _minu << _maxu << std::endl;
                return true;
            }
      
      protected:

            double T;
            // Desciption of PID in discrete time
            double bi[NR_CHANNELS], ad[NR_CHANNELS], bd[NR_CHANNELS], a0[NR_CHANNELS];
            double uI[NR_CHANNELS];        // integrative part of PID
            double uD[NR_CHANNELS];        // derivative part of PID
            double uP[NR_CHANNELS];        // Proportianal part of PID
            
	    InputType y, y_old;
            // The measurement and the last measurment

            SetPointType y_setp;
            // The setpoint (reference)

            OutputType z_output;
            // The p[roduced output

            double v, u;      // Variables used for calculating the output

            // Variables defining the PID in continuos time
	    ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _K;         // proportional gain
            ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _Ti;        // integrative time cte
            ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _Td;        // derivative time cte
            ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _N;         // limitation of deriavtive gain (e.g. value 10)
            ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _Tt;        // tracking time cte   Td<Tt<Ti, smaller is better
            ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _minu;      // minimal input
            ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _maxu;      // maximal input

	    ORO_CoreLib::Property<MotionControl::MultiVector<NR_CHANNELS, double> > _output;    // The output
	};

}

#endif // PID_HPP

