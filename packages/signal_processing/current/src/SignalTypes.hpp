/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  SignalTypes.hpp 

                        SignalTypes.hpp -  description
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
 
// ============================================================================
//
// = KERNEL_MODULE
//    LiAS_module.o
//
// = FILENAME
//    signaltypes.hpp
//
// = DESCRIPTION
//    A defintion of different types of signals in the systems. Basically not
//    very necessary, but a nice execise :-)
//
// = AUTHOR
//    Rene Waarsing <Rene.Waarsing@mech.kuleuven.ac.be>
//
//
// ============================================================================
//
// $Id: SignalTypes.hpp,v 1.15 2003/06/04 12:10:34 jrutgeer Exp $
// $Log: SignalTypes.hpp,v $
// Revision 1.15  2003/06/04 12:10:34  jrutgeer
// Changed "Model" to "Models" :)
//
// Revision 1.14  2003/06/04 12:08:12  jrutgeer
// Changed SignalTp<Estimates> to SignalTp<Models>
//
// Revision 1.13  2003/05/26 09:59:56  jrutgeer
// Removed some MotionKernel test implementation
// Renamed Read/WritePort's read and write functions to readFrom and writeTo
// Implemented TrajectoryInterpolatorSimple::reset(Vel)
//
// Revision 1.12  2003/04/14 12:08:28  psoetens
// fixes for the 3.2 compiler, the parabolic stuff is out and wraps round lrl
//
// Revision 1.11  2003/04/14 09:33:40  psoetens
// added template motionkernel and some cosmetic changes
//
// Revision 1.10  2002/11/12 09:21:14  psoetens
// * servoloop is now RunnableInterface
// * unified Actor stuff
// * Better VelocityMoudle semantics
//
// Revision 1.9  2002/10/21 15:07:46  pissaris
// Using standard PI.
//
// Revision 1.8  2002/10/10 14:22:06  psoetens
// Added licence to all headers
//
// Revision 1.7  2002/10/09 15:07:48  psoetens
// added FeedForwardSgn
//
// Revision 1.6  2002/10/01 17:01:39  pissaris
// Major reindent and reformat of sourcecode.
//
// Revision 1.5  2002/08/07 11:21:48  psoetens
// Builds for userspace and kernel space
// loads for userspace and kernel space
// many bugfixes.
// 'ported' lrl to orocos style STL
//
// Revision 1.4  2002/07/23 18:35:15  psoetens
// compiling KUKA
// changed some lrl to make it more orcos compatible
// solved the SignalTp mess (or not ..?)
//
// Revision 1.3  2002/07/19 16:02:15  psoetens
// Further comitting of motioncontrol stuff
//
// Revision 1.2  2002/07/15 15:12:27  psoetens
// further motion stuff
//
// Revision 1.1  2002/07/05 11:59:23  psoetens
// just some in between time committing of stuff
//
//
// ============================================================================
#ifndef SIGNALTYPES_HPP
#define SIGNALTYPES_HPP

namespace MotionControl
{

    /**
     * An virtually unlimmited set of types.
     */
    enum SignalTypes { 
        ReferenceSgn = 0, MeasurementSgn, ErrorSgn, ControlSgn, FeedForwardSgn, 
        SetPoints, Commands, Inputs, Models, Outputs 
    };

    /**
     * SignalTp is a way of assigning a type (kind) to numbers. Because of this extra
     * parameter, they get a semantic meaning which restricts its use. This means you
     * have to hand in some flexibility, but get type checking in return.
     * The Type of U is restricted too. <br>
     * It must support the following common operators:
     * {proper default constructor, operator+=, operator+, operator-, operator=, operator-= }
     *
     * The rules are as follows : 
     * 
     * <li>The result of any operation on SignalTp<T,U>  with an U returns an U (so the type is lost)</li>
     * <li>The result of any operation on SignalTp<T,U> with a SignalTp<R,U> returns an U (so the type is lost)</li>
     * <li>Any assignment of a SignalTp<T,U> with an U returns a SignalTp<T,U> (so the type is preserved)</li>
     * 
     * Examples : <br>
     * <li>U = SignalTp<T,U> * U </li>
     * <li>U = SignalTp<T,U> + SignalTp<R,U> </li>
     * <li>SignalTp<T,U> += U </li>
     *
     * The two processes of adding or removing type information could be called 'Enveloppping' and
     * 'Deenvelopping'.
     */
    template <int T, class U = double>
    struct SignalTp
    {
        typedef U value_type;

        /**
         * Default constructor
         */
        SignalTp () : value() {}

        /**
         * Constructor with initialisation of value.
         */
        SignalTp (const U& u_init ) : value( u_init )
        {}

        /**
         * Copy Constructor.
         */
        SignalTp ( const SignalTp<T,U>& orig ) : value( orig.value ) {}

        /**
         * Preserves type envelop.
         */
        SignalTp<T, U>& operator += ( const SignalTp<T, U>& a )
        {
            value += a.value;
            return *this;
        }

        /**
         * Preserves type envelop.
         */
        SignalTp<T, U>& operator -= ( const SignalTp<T, U>& a )
        {
            value -= a.value;
            return *this;
        }

        /**
         * Puts a value in an envelop.
         */
        SignalTp<T, U>& operator = ( const U& a )
        {
            value = a;
            return *this;
        }

        /**
         * Deenvelops the SignalTp
         */
        U operator - (const U& a) { return value - a; }

        U operator - (const SignalTp<T,U>& a) { return value - a.value; }

        U operator + (const SignalTp<T,U>& a) { return value + a.value; }
        
        U operator + (const U& a) { return value + a;  }
        
        /**
         * Automatic casting, returns a copy of the value.
         */
        operator U() const
        {
            return value;
        }

        /**
         * The value (contents) of this SignalTp.
         */
        U value;
    };

    //template <int T,class U>
    //U operator+(U d, const SignalTp<T,U>& a) { return d + a.value; }

    /**
     * A reference signal minus a measurement signal returns an error signal.
     */
    //template <class U>
    //SignalTp<ErrorSgn, U> operator - ( const SignalTp<ReferenceSgn, U>& a, const SignalTp<MeasurementSgn, U>& b )
    //{
    //    return U( a.value - b.value );
    //}

    /**
     * A Default + operator for any type...
     */
    //template <int S, int T, class U>
    //U operator + ( const SignalTp<S, U>& a, const SignalTp<T, U>& b )
    //{
    //    return U( a.value + b.value );
    //}

}

#endif

