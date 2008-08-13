/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:27 CET 2008  DataFlowInterface.cpp

                        DataFlowInterface.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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


#include "DataFlowInterface.hpp"
#include "Logger.hpp"

namespace RTT
{
    DataFlowInterface::DataFlowInterface(OperationInterface* parent /* = 0 */)
        : mparent(parent)
    {}

    DataFlowInterface::~DataFlowInterface() {
    }

    bool DataFlowInterface::addPort(PortInterface* port) {
        for ( PortStore::iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( it->first->getName() == port->getName() ) {
                log(Error) <<"Can not 'addPort' "<< port->getName() << ": name already in use." <<endlog();
                return false;
            }
        // This code belongs in addPort below, but in order to support
        // backwards compatible code, the PortObject is always created
        //
        if (mparent && mparent->getObject( port->getName()) != 0) {
            log(Error) <<"Can not 'addPort' "<< port->getName() << ": name already in use as TaskObject." <<endlog();
            return false;
        }

        mports.push_back( std::make_pair(port,std::string()) );
        OperationInterface* ms = this->createPortObject( port->getName());
        if ( ms )
            mparent->addObject( ms );
        return true;
    }

    bool DataFlowInterface::addPort(PortInterface* port, std::string description) {
        if (this->addPort(port) == false)
            return false;
        mports.back().second = description;
        return true;
    }

    void DataFlowInterface::removePort(const std::string& name) {
        for ( PortStore::iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( it->first->getName() == name ) {
                if (mparent)
                    mparent->removeObject( name );
                mports.erase(it);
                return;
            }
    }

    DataFlowInterface::Ports DataFlowInterface::getPorts() const {
        std::vector<PortInterface*> res;
        for ( PortStore::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            res.push_back( it->first );
        return res;
    }

    DataFlowInterface::PortNames DataFlowInterface::getPortNames() const {
        return this->getNames();
    }

    DataFlowInterface::PortNames DataFlowInterface::getNames() const {
        std::vector<std::string> res;
        for ( PortStore::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            res.push_back( it->first->getName() );
        return res;
    }

    PortInterface* DataFlowInterface::getPort(const std::string& name) const {
        for ( PortStore::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( it->first->getName() == name )
                return it->first;
        return 0;
    }

    std::string DataFlowInterface::getPortDescription(const std::string& name) const {
        for ( PortStore::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( it->first->getName() == name )
                return it->second;
        return "";
    }

    OperationInterface* DataFlowInterface::createPortObject(const std::string& name) {
        PortInterface* p = this->getPort(name);
        if ( !p )
            return 0;
        TaskObject* to = p->createPortObject();
        if (to) {
            std::string d = this->getPortDescription(name);
            if ( !d.empty() )
                to->setDescription( d );
            else
                to->setDescription("(No description set for this Port)");
        }
        return to;
    }

    void DataFlowInterface::clear()
    {
        // remove TaskObjects:
        for ( PortStore::iterator it(mports.begin());
              it != mports.end();
              ++it) {
            if (mparent)
                mparent->removeObject( it->first->getName() );
        }
        mports.clear();
    }
}
