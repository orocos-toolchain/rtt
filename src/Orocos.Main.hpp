/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Orocos.Main.hpp 

                        Orocos.Main.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
/**
 * @mainpage RTT Online API Documentation
 * 
 * @section intro Introduction
 * This is the Online API documentation of the Orocos Real-Time Toolkit.
 *
 * @section recom Recommendation
 * It is easiest to browse the classes by modules, to not
 * loose the overview. Click for example on the
 * <a href="modules.html"><tt>Modules List</tt></a>
 * link, select a module and then you get the most important classes
 * for this module.
 *
 * You will most likely also need to read the <a href="http://www.orocos.org/rtt/">online manuals</a>.
 *
 * @section corelib Real-Time Toolkit classes (RTT) 
 *
 * The classes in the RTT namespace form the application independent
 * control services which defines the portable framework for 
 * defining software components.
 *
 * A component is implemented using the RTT::TaskContext class. The
 * Component interface consits of five parts: events: RTT::Event, commands:
 * RTT::Command, methods: RTT::Method, properties: RTT::Property,
 * RTT::Attribute and data ports: RTT::DataPort, RTT::BufferPort. 
 *
 * A Component is run by a RTT::PeriodicActivity or
 * RTT::NonPeriodicActivity which attaches a thread to the Execution Engine.
 * The RTT::ExecutionEngine is the beating heart of each
 * component which executes the the application code, reacts to
 * events, processes commands etc.  Scripting is enabled by the
 * RTT::ScriptingAccess, XML can be read by the
 * RTT::MarshallingAccess. Many other classes are available as well.
 *
 * @section rttcorba CORBA support (network communication)
 *
 * Components can be connected over a network using the CORBA classes.
 * Only two classes are required: RTT::Corba::ControlTaskServer and
 * RTT::Corba::ControlTaskProxy. The former exports a local TaskContext instance
 * to the network, using the CORBA Naming Service if available, the latter
 * represents a remote TaskContext (located using the name or IOR) and allows
 * local TaskContexts to communicate with the remote instance.
 *
 * The RTT must be compiled with the --enable-corba flag in order to
 * enable this extension.
 *
 * @section devdrivers Hardware Access and Device Interface 
 *
 * Orocos has an abstraction layer and some utility classes to access
 * common devices (IO, encoders,...) from applications. The Device
 * Interface headers can be found in rtt/dev.
 *
 * The device drivers are not implemented by the RTT, but provided by components.
 * See the Orocos Components Library (OCL) documentation for a list of supported
 * devices and examples.
 */
