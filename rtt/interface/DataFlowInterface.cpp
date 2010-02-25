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
#include "../Logger.hpp"
#include "ServiceProvider.hpp"
#include "../TaskContext.hpp"

namespace RTT
{
    using namespace detail;

    DataFlowInterface::DataFlowInterface(TaskContext* parent /* = 0 */)
        : mparent(parent)
    {}

    DataFlowInterface::~DataFlowInterface() {
    }

    TaskContext* DataFlowInterface::getParent() {
        return mparent;
    }

    PortInterface& DataFlowInterface::addPort(PortInterface& port) {
        this->addLocalPort(port);
        if (mparent && mparent->provides()->hasService( port.getName()) != 0) {
            log(Warning) <<"'addPort' "<< port.getName() << ": name already in use as ServiceProvider. Replacing old instance." <<endlog();
            mparent->provides()->removeService(port.getName());
        }

        if (!mparent) {
            log(Warning) <<"'addPort' "<< port.getName() << ": DataFlowInterface not given to parent. Not adding ServiceProvider." <<endlog();
            return port;
        }
        ServiceProvider::shared_ptr ms( this->createPortObject( port.getName()) );
        if ( ms )
            mparent->provides()->addService( ms );
        // END NOTE.
        return port;
    }

    PortInterface& DataFlowInterface::addLocalPort(PortInterface& port) {
        for ( Ports::iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( (*it)->getName() == port.getName() ) {
                log(Warning) <<"'addPort' "<< port.getName() << ": name already in use. Replacing old instance." <<endlog();
            }

        mports.push_back( &port );
        port.setInterface( this );
        return port;
    }


    InputPortInterface& DataFlowInterface::addEventPort(InputPortInterface& port, InputPortInterface::NewDataOnPortEvent::SlotFunction callback) {
        this->addPort(port);
        if (callback)
            port.getNewDataOnPortEvent()->connect(callback );
        return port;
    }

    void DataFlowInterface::setupHandles() {
        for_each(handles.begin(), handles.end(), bind(&Handle::connect, _1));
    }

    void DataFlowInterface::cleanupHandles() {
        for_each(handles.begin(), handles.end(), bind(&Handle::disconnect, _1));
    }

    InputPortInterface& DataFlowInterface::addLocalEventPort(InputPortInterface& port, InputPortInterface::NewDataOnPortEvent::SlotFunction callback) {
        this->addLocalPort(port);

        if (mparent == 0) {
            log(Error) << "addLocalEventPort "<< port.getName() <<": DataFlowInterface not part of a TaskContext. Will not trigger any TaskContext nor register callback." <<endlog();
            return port;
        }
        bool running = mparent->stop();
        // setup synchronous callback, only purpose is to register that port fired and trigger the TC's engine.
        Handle h = port.getNewDataOnPortEvent()->connect(boost::bind(&TaskContext::dataOnPort, mparent, _1) );
        if (h) {
            log(Info) << mparent->getName() << " will be triggered when new data is available on InputPort " << port.getName() << endlog();
            handles.push_back(h);
        } else {
            log(Error) << mparent->getName() << " can't connect to event of InputPort " << port.getName() << endlog();
            return port;
        }

        mparent->dataOnPortSize(handles.size());
        if (callback)
            mparent->dataOnPortCallback(&port,callback); // the handle will be deleted when the port is removed.
        if (running)
            mparent->start();
        return port;
    }

    void DataFlowInterface::removePort(const std::string& name) {
        for ( Ports::iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( (*it)->getName() == name ) {
                if (mparent) {
                    mparent->provides()->removeService( name );
                    mparent->dataOnPortRemoved( *it );
                }
                (*it)->disconnect(); // remove all connections and callbacks.
                mports.erase(it);
                return;
            }
    }

    DataFlowInterface::Ports DataFlowInterface::getPorts() const {
        return mports;
    }

    DataFlowInterface::PortNames DataFlowInterface::getPortNames() const {
        return this->getNames();
    }

    DataFlowInterface::PortNames DataFlowInterface::getNames() const {
        std::vector<std::string> res;
        for ( Ports::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            res.push_back( (*it)->getName() );
        return res;
    }

    PortInterface* DataFlowInterface::getPort(const std::string& name) const {
        for ( Ports::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( (*it)->getName() == name )
                return *it;
        return 0;
    }

    std::string DataFlowInterface::getPortDescription(const std::string& name) const {
        for ( Ports::const_iterator it(mports.begin());
              it != mports.end();
              ++it)
            if ( (*it)->getName() == name )
                return (*it)->getDescription();
        return "";
    }

    ServiceProvider* DataFlowInterface::createPortObject(const std::string& name) {
        PortInterface* p = this->getPort(name);
        if ( !p )
            return 0;
        ServiceProvider* to = p->createPortObject();
        if (to) {
            std::string d = this->getPortDescription(name);
            if ( !d.empty() )
                to->doc( d );
            else
                to->doc("No description set for this Port. Use .doc() to document it.");
        }
        return to;
    }

    void DataFlowInterface::clear()
    {
        // remove TaskObjects:
        for ( Ports::iterator it(mports.begin());
              it != mports.end();
              ++it) {
            if (mparent)
                mparent->provides()->removeService( (*it)->getName() );
        }
        mports.clear();
    }
}
