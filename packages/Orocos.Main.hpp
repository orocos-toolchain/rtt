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
 * @mainpage Orocos Online API Documentation
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
 * It is easiest to browse the classes by modules, to not
 * loose the overview. Click for example on the
 * <a href="modules.html"><tt>Modules List</tt></a>
 * link, select a module and then you get the most important classes
 * for this module.
 *
 * You will most likely also need to read the <a href="http://www.orocos.org/documentation.html">online manuals</a>.
 *
 * @section corelib Orocos Real-Time Toolkit (RTT)
 * The classes in the RTT namespace form the application
 * independent control services 
 * which defines the portable framework using the five parts of a Component interface: 
 * events: RTT::Event, commands: RTT::Command, methods: RTT::Method, properties:
 * RTT::Property, RTT::Attribute and data ports: RTT::DataPort, RTT::BufferPort.
 * A Control Component is run by a RTT::PeriodicActivity or RTT::NonPeriodicActivity and inherits
 * from a RTT::TaskContext. The RTT::ExecutionEngine is the heart of each component
 * which contains the application code, reacts to events, processes commands etc.
 * Scripting is enabled by the RTT::ProgramLoader, XML can be read by 
 * the RTT::PropertyLoader. Many other classes are available as well.
 * The <a href="../../orocos-manual.html">Orocos Software Manual</a> introduces these classes.
 *
 * @section devdrivers Hardware Access and Device Drivers
 * Orocos has an abstraction layer and some utility classes to easily
 * access your devices from within Orocos and allow you to reuse Orocos
 * infrastructure.
 * @subsection comedi Comedi Device Communication
 * We have implemented LXRT and GNU/Linux C++ wrappers for Comedi devices,
 * which will use automatically the correct interface, depending 
 * on the target you build for. See the RTT namespace with
 * classes RTT::ComediSubDeviceAOut, RTT::ComediEncoder,
 * RTT::ComediSubDeviceDIn, etc. They express the comedi library as
 * an Orocos device API, which allows you to use them in other
 * classes which need I/O.
 * @subsection can CANOpen Device Communication
 * The RTT::CAN namespace contains device independent
 * implementations for realtime CAN communication using a RTT::CAN::CANPieController
 * and device driver. Device implementations
 * are available for some Beckhoff modules.
 * @subsection dev_integration Device Driver Integration
 * If you want to integrate your existing device drivers with Orocos,
 * they need to be wrapped to the Orocos Device API in order
 * to be used with existing implementations. You can take a look at the
 * device drivers package to find some examples.
 *
 */
