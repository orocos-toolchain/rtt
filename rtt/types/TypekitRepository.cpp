/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  Typekit.cxx

                        Typekit.cxx -  description
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


#include "TypekitRepository.hpp"
#include "TypekitPlugin.hpp"
#include "TransportPlugin.hpp"
#include "../Logger.hpp"
#include <algorithm>

namespace RTT {
    using namespace detail;
    using namespace std;

    std::vector<TypekitPlugin*> TypekitRepository::Tools;
    std::vector<TransportPlugin*> TypekitRepository::Transports;

    void TypekitRepository::Import( TypekitPlugin& tkpr )
    {
        TypekitPlugin* tkp = &tkpr;
        Logger::In in("Typekit");
        if ( find( Tools.begin(), Tools.end(), tkp ) != Tools.end() ) {
            Logger::log() <<Logger::Debug << "Tool "<<tkp->getName() <<" already loaded."<<Logger::endl;
            return;
        }

        Logger::log() <<Logger::Info << "Loading Tool "<<tkp->getName() <<"."<<Logger::endl;
        Tools.push_back( tkp );

        if ( tkp->loadTypes() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load types."<<Logger::endl;
        }

        if ( tkp->loadConstructors() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load type constructors."<<Logger::endl;
        }
        if ( tkp->loadOperators() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load type operators."<<Logger::endl;
        }
        if ( tkp->loadGlobals() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load global variables."<<Logger::endl;
        }
    }

    void TypekitRepository::Import( TransportPlugin& trpr )
    {
        TransportPlugin* trp = &trpr;
        Logger::In in("Typekit");
        if ( find( Transports.begin(), Transports.end(), trp ) != Transports.end() ) {
            Logger::log() <<Logger::Debug << "Transport "<<trp->getName() <<" already loaded."<<Logger::endl;
            return;
        }

        Logger::log() <<Logger::Info << "Loading Transport "<<trp->getName() <<"."<<Logger::endl;
        Transports.push_back( trp );

        TypeInfoRepository::Instance()->registerTransport( trp );
    }

    std::vector<std::string> TypekitRepository::getTools()
    {
        std::vector<std::string> ret;
        for (std::vector<TypekitPlugin*>::const_iterator it = Tools.begin();
             it != Tools.end(); ++it)
            ret.push_back( (*it)->getName() );
        return ret;
    }

    std::vector<std::string> TypekitRepository::getTransports()
    {
        std::vector<std::string> ret;
        for (std::vector<TransportPlugin*>::const_iterator it = Transports.begin();
             it != Transports.end(); ++it)
            ret.push_back( (*it)->getName() );
        return ret;
    }

    bool TypekitRepository::hasTool( const std::string& toolname )
    {
        for (std::vector<TypekitPlugin*>::const_iterator it = Tools.begin();
             it != Tools.end(); ++it)
            if ((*it)->getName() == toolname)
                return true;
        return false;
    }

    bool TypekitRepository::hasTransport( const std::string& transportname )
    {
        for (std::vector<TransportPlugin*>::const_iterator it = Transports.begin();
             it != Transports.end(); ++it)
            if ((*it)->getName() == transportname)
                return true;
        return false;
    }


}
