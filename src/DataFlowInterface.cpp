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
