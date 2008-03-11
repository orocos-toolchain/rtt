/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:27 CET 2008  OperationInterface.cpp 

                        OperationInterface.cpp -  description
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

    OperationInterface::~OperationInterface()
    {
        // better scope such that virtual subclass function is not called.
        OperationInterface::clear();
    }

    void OperationInterface::clear()
    {
        Objects::const_iterator it = mobjects.begin();
        while ( it != mobjects.end() ) {
            if ( (*it)->getParent() == this )
                delete *it;
            ++it;
        }
        mobjects.clear();
        mattributes.clear();
    }

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
