/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  JointInterface.hpp 

                        JointInterface.hpp -  description
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
//    JointInterface.h
//
// = DESCRIPTION
//    Classes for a JointInterface
//
// = AUTHOR
//    Rene Waarsing <Rene.Waarsing@mech.kuleuven.ac.be>
//
//
// ============================================================================
//
// $Id: JointInterface.hpp,v 1.6 2003/06/17 18:49:44 psoetens Exp $
// $Log: JointInterface.hpp,v $
// Revision 1.6  2003/06/17 18:49:44  psoetens
// adapted new spelling : initialize, finalize, serialize
//
// Revision 1.5  2003/05/15 16:25:11  psoetens
// removed unneeded interfaces
//
// Revision 1.4  2002/10/10 14:18:35  psoetens
// Added licence information to all header files
//
// Revision 1.3  2002/10/01 17:01:38  pissaris
// Major reindent and reformat of sourcecode.
//
// Revision 1.2  2002/07/23 18:25:26  psoetens
// Conversion to DeviceServer
//
// Revision 1.1  2002/07/19 15:46:22  psoetens
// merging Oelewapperke-merge1 with HEAD
//
// Revision 1.1  2002/02/13 15:38:00  rwaarsin
// First version
//
//
// ============================================================================
#ifndef JOINTINTERFACE_HPP
#define JOINTINTERFACE_HPP

//-----------------------------------------------------------------------------
// Includes

template < class T >
class JointInterface
{

    public:
        typedef T DriveStruct;

        //  enum ControlMode {ZeroHold, Velocity, Homing};


        virtual ~JointInterface()
        {}

        ;

        // Gives the JointInterface class the function of an actuator
        //    virtual int drive(const DriveStruct& value);

        // The offset is added to every output of drive
        //    virtual void offset(const DriveStruct& o);

        // reset ALL encoders
        //    void encoderClear();

        // returns number of JointInterface
        //    int getNr();

        //--------------------------------------------
        // Interface to the JointInterface commands

        // Set a certain Velocity in the VelToPosInt
        //    void setVelocity(const float w);

        // Initialize the JointInterface for homing
        //    void initializeHoming();

        // Start the control loop, unbreak.
        //    void start ();

        // break, stop the control loop.
        //    void stop  ();

        // Delete the present trajectory..
        //    void clearState ();

        // be carefull calling this function, it is not protected! If you
        // call this function when a servoloop is running, bad things can happen!
        //    bool setMode(ControlMode& newMode);

};

#endif   // JointInterface_H
