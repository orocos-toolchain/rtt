/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  PID_Controller.hpp 

                        PID_Controller.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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

#ifndef ORO_PROCESS_CONTROL_PID_HPP
#define ORO_PROCESS_CONTROL_PID_HPP

#include <control_kernel/DataServer.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include <control_kernel/ExecutionExtension.hpp>
#include "execution/TemplateFactories.hpp"
#endif

#pragma interface

namespace ORO_ControlKernel
{
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    using namespace ORO_Execution;
#endif

    /**
     * The output of the PID_Controller.
     */
    struct PID_Controller_Output
        : ServedTypes< std::vector<double> > 
    {
        PID_Controller_Output() {
            this->insert(std::make_pair(0, "ChannelValues"));
        }
    };

    struct PID_Input
        : ServedTypes< std::vector<double> > 
    {
    };

    struct PID_SetPoint
        : ServedTypes< std::vector<double> > 
    {
    };

    /**
     * @brief A Multi-Channel (nSISO) PID Data Flow Controller Component.
     *
     * Features are :
     * @verbatim
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
     * @endverbatim
     *
     * It does a control action on the 'ChannelValues' of the setpoints and
     * inputs dataobjects and adds the 'FeedForward' signal from the setpoints
     * if present.
     *  @ingroup kcomps kcomp_controller 
     */
    class PID_Controller
        : public Controller<Expects<PID_Input>,
                            Expects<NoModel>,
                            Expects<PID_SetPoint>,
                            Writes<PID_Controller_Output>,
                            MakeAspect<PropertyExtension, KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                       , ExecutionExtension
#endif
                                       , ReportingExtension
                                       >::Result >

    {
        typedef Controller<Expects<PID_Input>,
                           Expects<NoModel>,
                           Expects<PID_SetPoint>,
                           Writes<PID_Controller_Output>,
                           MakeAspect<PropertyExtension, KernelBaseFunction
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
                                      , ExecutionExtension
#endif
                                      , ReportingExtension
                                      >::Result > Base;
      
    public:
        PID_Controller(int _num_chans = 1, const std::string& name = "PID_Controller" ) 
            : Base ( name ),
              num_chans( _num_chans ),
              err("Error", "Tracking error", std::vector<double>( num_chans, 0) )
            , _K("K", "Proportional gain", std::vector<double>( num_chans, 0) )
            , _Kff("Kff", "Proportional gain", std::vector<double>( num_chans, 1) )
            , _Ti("Ti", "Integrative time cte", std::vector<double>( num_chans, 0))
            , _Td("Td", "Derivative time cte", std::vector<double>( num_chans, 0))
            , _N("N", "Limitation of derivative gain (e.g. value 10)", std::vector<double>( num_chans, 1))
            , _Tt("Tt", "Tracking time constant  Td<Tt<Ti, smaller is better, must be != 0", std::vector<double>( num_chans, 0.5))
            , _minu("min_u", "Minimal input", std::vector<double>( num_chans, -1))
            , _maxu("max_u", "Maximal input", std::vector<double>( num_chans, 1))
	    {
            bi = new double [num_chans];
            ad = new double [num_chans];
            bd = new double [num_chans];
            a0 = new double [num_chans];
            uI = new double [num_chans];
            uD = new double [num_chans];
            uP = new double [num_chans];
            uFF = new double [num_chans];

            for (int i=0; i < num_chans; ++i) 
                uI[i] = uD[i] = uP[i] = uFF[i] = 0;

            _result.resize(num_chans, 0.0);
            _refPos.resize(num_chans, 0.0);
            y.resize(num_chans, 0.0);
            y_old.resize(num_chans, 0.0);
            _ff.resize(num_chans, 0.0);
        }

	    virtual ~PID_Controller() {
            delete[] bi;
            delete[] ad;
            delete[] bd;
            delete[] a0;
            delete[] uI;
            delete[] uD;
            delete[] uP;
            delete[] uFF;
        }

	    virtual bool componentLoaded()
	    {
            std::cout << "Loading PID_Controller" << std::endl;
            T = kernel()->getPeriod();          // sample period
            // discrete time parameters
            for ( int i = 0; i < num_chans; i++)
                {
                    recalculate(i, _K.get()[i]);
                }

            return Output::dObj()->Get("ChannelValues",outp_dObj);
	    }

        void changeMinU(int channel, double newMinU )
        {
            if (channel <0 || channel >= num_chans)
                return;
        
            _minu.set()[channel] = newMinU;
        }

        void changeMaxU(int channel, double newMaxU )
        {
            if (channel <0 || channel >= num_chans)
                return;
        
            _maxu.set()[channel] = newMaxU;
        }

        /**
         * @name The PID_Controller Commands.
         * @{
         */

        /**
         * @brief Change the time integrative constant of a channel.
         */
        void changeTi(int channel, double newTi )
        {
            if (channel <0 || channel >= num_chans)
                return;
        
            _Ti.set()[channel] = newTi;
            recalculate( channel, _K.get()[channel] );
        }

        /**
         * @brief Change the time derivative constant of a channel.
         */
        void changeTd(int channel, double newTd )
        {
            if (channel <0 || channel >= num_chans)
                return;
        
            _Td.set()[channel] = newTd;
            recalculate( channel, _K.get()[channel] );
        }

        /**
         * @brief Change the proportional gain of a channel.
         */
        void changeK(int channel, double newK )
        {
            if (channel <0 || channel >= num_chans)
                return;
        
            recalculate( channel, newK );
        }

        /**
         * @brief Reset the controller, ie, set uI of all channels to zero.
         */
        void resetController()
        {
            for (int i=0; i < num_chans; ++i)
                resetChannel(i);
        }

        /**
         * @brief Reset a channel, ie, set uI of this channel to zero.
         */
        void resetChannel(int channel)
        {
            if (channel <0 || channel >= num_chans)
                return;
            uI[channel] = 0;
            uD[channel] = 0;
        }
        /**
         * @}
         */

        virtual bool componentStartup()
	    {
            if ( ! SetPoint::dObj()->Get("ChannelValues",setp_dObj) ||
                 ! Input::dObj()->Get("ChannelValues", inp_dObj) )
                {
                    return false;
                }
            if ( ! SetPoint::dObj()->Get("FeedForward",setp_ff_dObj) )
                setp_ff_dObj = 0;

            // Set y_old to the current value...
            pull();
            y_old = y;
            // .. and the rest to zero;
            for ( int i = 0; i< num_chans; i++)
                {
                    uI[i] = 0;
                    uD[i] = 0;
                    uFF[i] = 0;
                    _ff[i] = 0;
                }
            calculate();
            push();

            return true;
	    };

        virtual void pull()      
        {
            // Remember the current value
            inp_dObj->Get(y);
            setp_dObj->Get(_refPos);
            if ( setp_ff_dObj )
                setp_ff_dObj->Get(_ff);
	    }
            
        // Here, the calculation of the new actuator command will be done
	    virtual void calculate()
        {
            for( int i = 0; i< num_chans; i++)
                {
                    uFF[i] = _ff[i] * _Kff.get()[i];
                    // Compute the proportional
                    err.set()[i] = _refPos[i] - y[i];
                    uP[i] = _K.get()[i] * ( err.get()[i] );
                    // update derivative part

                    if ( _Td.get()[i]  != 0 )
                        uD[i] = ad[i] * uD[i] - bd[i] * ( y[i] - y_old[i] );

                    // Compute control variable
                    v = uP[i] + uD[i] + uI[i] + uFF[i];

                    // Check for saturation
                    u = v;

                    if ( v < _minu.get()[i]  )
                        u = _minu.get()[i] ;

                    if ( v > _maxu.get()[i]  )
                        u = _maxu.get()[i] ;

                    // Update integral part with windup protection
                    if ( _Ti.get()[i]  != 0 )
                        uI[i] += bi[i] * ( _refPos[i] - y[i] ) + a0[i] * ( u - v );
                    y_old[i] = y[i];

                    // u = the steering signal
                    _result[i] = u;
                }
        }
            
        // Write the actuator command to the actuator
	    virtual void push()      
        {
            outp_dObj->Set( _result );
	    }

        /**
         * @see MotionPropertyExtension.hpp class PropertyComponentInterface
         */
        virtual bool updateProperties( const PropertyBag& bag)
        {
            //std::cerr << "Properties imported! : " << _K << _Ti << _Td << _N << _Tt << _minu << _maxu << std::endl;
            composeProperty(bag, _K);
            composeProperty(bag, _Kff);
            composeProperty(bag, _Ti); 
            composeProperty(bag, _Td); 
            composeProperty(bag, _N); 
            composeProperty(bag, _Tt); 
            composeProperty(bag, _minu);
            composeProperty(bag, _maxu);

            //std::cout << "Properties imported! : " << _K << _Ti << _Td << _N << _Tt << _minu << _maxu << std::endl;
            return true;
        }

        virtual void exportProperties( PropertyBag& bag )
        {
            bag.add(&_K);
            bag.add(&_Kff);
            bag.add(&_Ti);
            bag.add(&_Td);
            bag.add(&_N);
            bag.add(&_Tt);
            bag.add(&_minu);
            bag.add(&_maxu);
        }
      
        virtual void exportReports( PropertyBag& bag )
        {
            bag.add(&err);
        }
    protected:

        /**
         * @brief A bumpless recalculation of discrete
         * parameters if any continuous parameter changed.
         *
         * @param c The channel to change
         * @param newK the new K parameter (provide the old one if unchanged).
         */
        void recalculate(int c, double newK)
        {
            if (c <0 || c >= num_chans)
                return;
            // bumpless parameter changes
            uI[c] += _K.get()[c] * ( _refPos[c] - y[c] ) - newK * ( _refPos[c] - y[c] );
            _K.set()[c] = newK;

            // Recalculate the discrete time parameters
            // The parameters for the integral action (if Ti == 0, no integral action)
            if ( _Ti.get()[c] != 0 ) {
                a0[c] = T / _Tt.get()[c];
                bi[c] = _K.get()[c] * T / _Ti.get()[c];
            }
            else {
                a0[c] = 0.0;
                bi[c] = 0.0;
            }

            // The parameters for the derivative action (if Td == 0, no derivative action)
            if ( _Td.get()[c] != 0 )
                {
                    ad[c] = ( 2 * _Td.get()[c] - _N.get()[c] * T ) / ( 2 * _Td.get()[c] + _N.get()[c] * T );
                    bd[c] = ( 2 * _K.get()[c] * _N.get()[c] * _Td.get()[c] ) / ( 2 * _Td.get()[c] + _N.get()[c] * T );
                }
            else
                {
                    ad[c] = 0.0;
                    bd[c] = 0.0;
                };
        }

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        MethodFactoryInterface* createMethodFactory()
        {
            TemplateMethodFactory< PID_Controller >* ret =
                newMethodFactory( this );
            ret->add( "changeK",
                      method( &PID_Controller::changeK,
                               "Change the Gain of the PID Controller",
                               "Channel","The Channel number (starting from 0).",
                               "Gain","The new Gain (K)."
                               ) ); 
            ret->add( "changeTd",
                      method( &PID_Controller::changeTd,
                               "Change the Derivative Td constant.",
                               "Channel","The Channel number (starting from 0).",
                               "Td","The new Derivative time constant (Td)."
                               ) ); 
            ret->add( "changeTi",
                      method( &PID_Controller::changeTd,
                               "Change the Integration Ti constant.",
                               "Channel","The Channel number (starting from 0).",
                               "Ti","The new Integration time constant (Ti)."
                               ) ); 
            ret->add( "resetController",
                      method( &PID_Controller::resetController,
                               "Reset the whole controller (ie all integration uI and derivative uD = 0)."
                               ) ); 
            ret->add( "resetChannel",
                      method( &PID_Controller::resetChannel,
                               "Reset the uI (integration) and uD (derivative) of a channel to zero.",
                               "Channel","The Channel number (starting from 0)."
                               ) ); 
            return ret;
        }

#endif


        int num_chans;

        typedef std::vector<double> ChannelType;
        DataObjectInterface<ChannelType>* inp_dObj;
        DataObjectInterface<ChannelType>* outp_dObj;
        DataObjectInterface<ChannelType>* setp_dObj;
        DataObjectInterface<ChannelType>* setp_ff_dObj;
        ChannelType   _result;
        ChannelType   _refPos;
        ChannelType   y, y_old, _ff;
        Property< std::vector< double> > err;      // tracking error

        double T;
        // Desciption of PID in discrete time
        double *bi, *ad, *bd, *a0;
        double *uI;        // integrative part of PID
        double *uD;        // derivative part of PID
        double *uP;        // Proportianal part of PID
        double *uFF;       // FeedForward part of PID
            
        double v, u;      // Variables used for calculating the output

        // Variables defining the PID in continuos time
	    Property<std::vector< double> > _K;         // proportional gain
	    Property<std::vector< double> > _Kff;         // feedforward gain
        Property<std::vector< double> > _Ti;        // integrative time cte
        Property<std::vector< double> > _Td;        // derivative time cte
        Property<std::vector< double> > _N;         // limitation of deriavtive gain (e.g. value 10)
        Property<std::vector< double> > _Tt;        // tracking time cte   Td<Tt<Ti, smaller is better
        Property<std::vector< double> > _minu;      // minimal input
        Property<std::vector< double> > _maxu;      // maximal input
	};

}

#endif 

