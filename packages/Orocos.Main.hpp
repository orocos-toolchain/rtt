/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Orocos.Main.hpp 

                        Orocos.Main.hpp -  description
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
 
/**
 * @mainpage <a href="http://www.orocos.org">Orocos</a> Online API Documentation
 * 
 * @section intro Introduction
 * This is the Online API documentation of all available
 * packages in Orocos. Not necessarily all files listed
 * here are installed on your system. The Orocos
 * Documentation web page on the main site contains links
 * to individual packages (which still contains cross
 * references to others).
 *
 * @section recom Recommendation
 * It is easiest to browse the classes by namespace, to not
 * loose the overview. Click for example on the
 * <a href="namespaces.html"><tt>Namespace List</tt></a>
 * link, select a namespace and then you get all the classes
 * defined within this namespace. At the bottom of that page,
 * you'll find a more elaborate description of the namespace.
 *
 * You will most likely also need to read the <a href="http://www.orocos.org/documentation.php">online manuals</a>.
 *
 * @section corelib Orocos Realtime Services
 * The classes in the following namespaces form the application
 * independent core services : ORO_OS, ORO_CoreLib, ORO_DeviceInterface and ORO_Execution,
 * which define the portable framework using the ORO_CoreLib::Event, ORO_CoreLib::PeriodicTask, ORO_Execution::TaskContext,
 * ORO_CoreLib::DataObject, the script ORO_Execution::Parser, command ORO_Execution::Processor and many other classes.
 *
 * @section apps Applications
 * The following application frameworks are available : 
 *
 * @subsection comps Motion Control Kernel and Components
 * The <a href="modules.html"><tt>Modules</tt></a> page contains
 * a summary of all available components for the Orocos Control
 * Kernel framework. There are components for kinematics ( using ORO_KinDyn ),
 * 1D and 6D ORO_Geometry::Path and ORO_Geometry::VelocityProfile planning in a
 * ORO_Geometry::Trajectory ( using ORO_Geometry ), and mere signal processing
 * ( using ORO_SignalProcessing ).
 *
 * @note The Control Kernel Client GUI is not documented in this document.
 * It comes with its own documentation and you can fetch it from the 
 * <a href="http://www.orocos.org/Download.php">Download</a> page.
 *
 * @subsection can CANOpen Device Communication
 * The CAN namespace contains device independent
 * implementations for realtime CAN communication using a CAN::CANPieController
 * and device driver in the ORO_DeviceDriver namespace. Device implementations
 * are available for some Beckhoff modules.
 *
 */
