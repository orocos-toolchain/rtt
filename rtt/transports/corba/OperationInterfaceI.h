/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationInterfaceI.h

                        OperationInterfaceI.h -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from
// ../../../ACE_wrappers/TAO/TAO_IDL/be/be_codegen.cpp:1133

#ifndef ORO_CORBA_OPERATIONREPOSITORYI_H_
#define ORO_CORBA_OPERATIONREPOSITORYI_H_

#include "rtt/transports/corba/corba.h"
#ifdef CORBA_IS_TAO
#include "OperationInterfaceS.h"
#else
#include "OperationInterfaceC.h"
#endif
#include "rtt/OperationInterface.hpp"
#include "rtt/internal/SendHandleC.hpp"
#include "rtt/internal/OperationInterfacePartFused.hpp"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class  RTT_corba_CSendHandle_i
  : public virtual POA_RTT::corba::CSendHandle,
  public virtual PortableServer::RefCountServantBase
{
      RTT::internal::SendHandleC mhandle, morig;
      RTT::OperationInterfacePart* mofp;
      std::vector<RTT::base::DataSourceBase::shared_ptr> cargs;
public:
  // Constructor
  RTT_corba_CSendHandle_i (RTT::internal::SendHandleC const& sh, RTT::OperationInterfacePart* ofp);

  // Destructor
  virtual ~RTT_corba_CSendHandle_i (void);

  virtual
  ::RTT::corba::CSendStatus collect (
      ::RTT::corba::CAnyArguments_out args);

  virtual
  ::RTT::corba::CSendStatus collectIfDone (
      ::RTT::corba::CAnyArguments_out args);

  virtual
  ::RTT::corba::CSendStatus checkStatus (
      void);

  virtual
  ::CORBA::Any * ret (
      void);

  virtual
  void checkArguments (
          const ::RTT::corba::CAnyArguments & args);

  virtual
  void dispose ();
};

class  RTT_corba_COperationInterface_i
  : public virtual POA_RTT::corba::COperationInterface
{
      RTT::OperationInterface* mfact;
      PortableServer::POA_var mpoa;
      RTT::Operation<bool(std::string)> loadPluginOperation;
      RTT::internal::OperationInterfacePartFused<bool(std::string)> loadPluginOperationPart;

  public:
    //Constructor
      RTT_corba_COperationInterface_i(RTT::OperationInterface* mfact, PortableServer::POA_ptr the_poa);

      PortableServer::POA_ptr _default_POA();

  // Destructor
  virtual ~RTT_corba_COperationInterface_i (void);

  virtual
  RTT::corba::COperationInterface::COperationDescriptions * getOperations (
      void);

  virtual
  ::RTT::corba::CArgumentDescriptions * getArguments (
      const char * operation);

  virtual
  ::RTT::corba::CTypeList * getCollectTypes (
      const char * operation);

  virtual
  char * getResultType (
      const char * operation);

  virtual
  char* getArgumentType(
          const char*,
          CORBA::UShort);

  virtual
  char* getCollectType(
          const char*,
          CORBA::UShort);

  virtual ::CORBA::UShort getArity (
      const char * operation);

  virtual ::CORBA::UShort getCollectArity (
      const char * operation);

  virtual
  char * getDescription (
      const char * operation);

  virtual
  void checkOperation (
      const char * operation,
      const ::RTT::corba::CAnyArguments & args);

  virtual
  ::CORBA::Any * callOperation (
      const char * operation,
      ::RTT::corba::CAnyArguments & args);

  virtual
  ::RTT::corba::CSendHandle_ptr sendOperation (
      const char * operation,
      const ::RTT::corba::CAnyArguments & args);

  virtual
  void sendOperationOneway (
      const char * operation,
      const ::RTT::corba::CAnyArguments & args);

private:
  RTT_corba_CSendHandle_i* sendOperationInternal (
      const char * operation,
      const ::RTT::corba::CAnyArguments & args);

  RTT::OperationInterfacePart *findOperation ( const char *operation );
  bool loadPlugin ( const std::string& pluginPath );
};


#endif /* OPERATIONREPOSITORYI_H_  */

