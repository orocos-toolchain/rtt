/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  Toolkit.cxx 

                        Toolkit.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#include <rtt/Toolkit.hpp>
#include <rtt/Logger.hpp>
#include <algorithm>

namespace RTT
{
    using namespace std;

    std::vector<ToolkitPlugin*> Toolkit::Tools;

    void Toolkit::Import( ToolkitPlugin& tkpr )
    {
        ToolkitPlugin* tkp = &tkpr;
        Logger::In in("Toolkit");
        if ( find( Tools.begin(), Tools.end(), tkp ) != Tools.end() ) {
            Logger::log() <<Logger::Debug << "Tool "<<tkp->getName() <<" already loaded."<<Logger::endl;
            return;
        }

        Logger::log() <<Logger::Info << "Loading Tool "<<tkp->getName() <<"."<<Logger::endl;
        Tools.push_back( tkp );

        if ( tkp->loadTypes() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load types."<<Logger::endl;
        }

        if ( tkp->loadOperators() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load type operators."<<Logger::endl;
        }
    }

    std::vector<std::string> Toolkit::getTools()
    {
        std::vector<std::string> ret;
        for (std::vector<ToolkitPlugin*>::const_iterator it = Tools.begin();
             it != Tools.end(); ++it)
            ret.push_back( (*it)->getName() );
        return ret;
    }

    bool Toolkit::hasTool( const std::string& toolname )
    {
        for (std::vector<ToolkitPlugin*>::const_iterator it = Tools.begin();
             it != Tools.end(); ++it)
            if ((*it)->getName() == toolname)
                return true;
        return false;
    }
        

}
