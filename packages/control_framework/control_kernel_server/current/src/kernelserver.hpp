/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:46:14 CET 2004  kernelserver.hpp 

                        kernelserver.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

namespace ORO_ControlKernel {
    class ControlKernelInterface;
}

/**
 * This is a helper function for users.using the kernel directly.
 */
void setKernel( ORO_ControlKernel::ControlKernelInterface* kernel );

/**
 * Start the corba server, call this after setKernel.
 */
int startServer(int argc, char* argv[]);
