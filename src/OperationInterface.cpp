#include "OperationInterface.hpp"

namespace RTT
{
    OperationInterface::OperationInterface()
    {}

    OperationInterface::OperationInterface(ExecutionEngine* ee)
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
           :mevents( ee )
#endif
    {}


    bool OperationInterface::addObject( OperationInterface *obj ) {
        assert(obj);
        if ( getObject( obj->getName() ) ) {
            log(Error) <<"Can not load Object "<<obj->getName()<<": already loaded in this TaskContext." <<endlog();
            return false;
        }
        if ( obj->getParent() ) {
            log(Error) <<"Can not load Object "<<obj->getName()<<": already loaded in another TaskContext." <<endlog();
            return false;
        }
        mobjects.push_back(obj);
        obj->setParent( this );
        //obj->setEngine( this->engine() );
        return true;
    }

    OperationInterface* OperationInterface::getObject(const std::string& obj_name )
    {
        if (obj_name == "this")
            return this;
        Objects::const_iterator it = mobjects.begin();
        while ( it != mobjects.end() )
            if ( (*it)->getName() == obj_name )
                return *it;
            else
                ++it;
        return 0;
    }

    std::vector<std::string> OperationInterface::getObjectList() const
    {
        std::vector<std::string> res;
        res.push_back("this");
        std::transform(mobjects.begin(), mobjects.end(),
                       std::back_inserter( res ), boost::bind(&OperationInterface::getName,_1));
        return res;
    }

    bool OperationInterface::removeObject(const std::string& obj_name ) {
        if (obj_name == "this") {
            this->methods()->clear();
            this->commands()->clear();
            this->events()->clear();
            this->attributes()->clear();
            return true;
        }
        OperationInterface* tgt = getObject(obj_name);
        if (tgt) {
            mobjects.erase( find(mobjects.begin(), mobjects.end(), tgt) );
            if (tgt->getParent() == this )
                delete tgt;
            return true;
        }
        return false;
    }


}
