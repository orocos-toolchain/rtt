/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ScriptingAccessProxy.hpp

                        ScriptingAccessProxy.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_EXECUTION_SCRIPTING_ACCESS_PROXY_HPP
#define ORO_EXECUTION_SCRIPTING_ACCESS_PROXY_HPP

#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#else
#include <omniORB4/CORBA.h>
#include <omniORB4/poa.h>
#endif

#include "ScriptingAccessC.h"
#include "../../scripting/ScriptingAccess.hpp"

namespace RTT
{namespace Corba
{

    /**
     * A Corba Proxy for the ScriptingAccess class.
     */
    class RTT_CORBA_API ScriptingAccessProxy
        : public RTT::ScriptingAccess
    {
    protected:
        Corba::ScriptingAccess_var msa;
    public:
        ScriptingAccessProxy( Corba::ScriptingAccess_ptr sa )
            : ScriptingAccess( 0 ),
              msa( Corba::ScriptingAccess::_duplicate( sa ) )
        {}

        virtual ~ScriptingAccessProxy();

        using ScriptingAccess::loadPrograms;
        using ScriptingAccess::loadStateMachines;

        virtual int execute( const std::string& code );
        virtual DispatchInterface::shared_ptr getCommand( int ticket );

        virtual bool hasProgram( std::string name) const;
        virtual bool loadPrograms( std::string code, std::string filename, bool do_throw = false );
        virtual bool unloadProgram( std::string name, bool do_throw = false );

        virtual std::vector<std::string> getPrograms() const;
        virtual int getProgramLine(std::string name) const;
        virtual std::string getProgramText(std::string name ) const;
        virtual std::string getProgramStatus(std::string name ) const;

        virtual bool loadStateMachines( std::string code, std::string filename, bool do_throw = false );
        virtual bool unloadStateMachine( std::string name, bool do_throw = false );

        virtual bool hasStateMachine( std::string name) const;
        virtual std::vector<std::string> getStateMachines() const;
        virtual std::string getStateMachineText(std::string name ) const;
        virtual std::string getStateMachineStatus(std::string name ) const;
        virtual int getStateMachineLine(std::string name ) const;
        virtual std::string getCurrentState(std::string name ) const;
    };
}}

#endif
