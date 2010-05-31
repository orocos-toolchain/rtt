/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnPolicy.cpp

                        ConnPolicy.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


/*
 * ConnPolicy.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: kaltan
 */

#include "ConnPolicy.hpp"
#include "Property.hpp"
#include "PropertyBag.hpp"

using namespace std;

namespace RTT
{
    bool composeProperty(const PropertyBag& bag, ConnPolicy& result)
    {
        Property<int> i;
        Property<bool> b;
        Property<string> s;
        if ( bag.getType() != "ConnPolicy")
            return false;
        log(Debug) <<"Composing ConnPolicy..." <<endlog();
        i = bag.getProperty("type");
        if ( i.ready() )
            result.type = i.get();
        else if ( bag.find("type") ){
            log(Error) <<"ConnPolicy: wrong property type of 'type'."<<endlog();
            return false;
        }
        i = bag.getProperty("lock_policy");
        if ( i.ready() )
            result.lock_policy = i.get();
        else if ( bag.find("lock_policy") ){
            log(Error) <<"ConnPolicy: wrong property type of 'lock_policy'."<<endlog();
            return false;
        }
        i = bag.getProperty("size");
        if ( i.ready() )
            result.size = i.get();
        else if ( bag.find("size") ){
            log(Error) <<"ConnPolicy: wrong property type of 'size'."<<endlog();
            return false;
        }
        i = bag.getProperty("data_size");
        if ( i.ready() )
            result.data_size = i.get();
        else if ( bag.find("data_size") ){
            log(Error) <<"ConnPolicy: wrong property type of 'data_size'."<<endlog();
            return false;
        }
        i = bag.getProperty("transport");
        if ( i.ready() )
            result.transport = i.get();
        else if ( bag.find("transport") ){
            log(Error) <<"ConnPolicy: wrong property type of 'transport'."<<endlog();
            return false;
        }

        b = bag.getProperty("init");
        if ( b.ready() )
            result.init = b.get();
        else if ( bag.find("init") ){
            log(Error) <<"ConnPolicy: wrong property type of 'init'."<<endlog();
            return false;
        }
        b = bag.getProperty("pull");
        if ( b.ready() )
            result.pull = b.get();
        else if ( bag.find("pull") ){
            log(Error) <<"ConnPolicy: wrong property type of 'pull'."<<endlog();
            return false;
        }

        s = bag.getProperty("name_id");
        if ( s.ready() )
            result.name_id = s.get();
        else if ( bag.find("name_id") ){
            log(Error) <<"ConnPolicy: wrong property type of 'name_id'."<<endlog();
            return false;
        }
        return true;
    }
    void decomposeProperty(const ConnPolicy& cp, PropertyBag& targetbag)
    {
        log(Debug) <<"Decomposing ConnPolicy..." <<endlog();
        assert( targetbag.empty() );
        targetbag.setType("ConnPolicy");
        targetbag.ownProperty( new Property<int>("type","Data type", cp.type));
        targetbag.ownProperty( new Property<bool>("init","Initialize flag", cp.init));
        targetbag.ownProperty( new Property<int>("lock_policy","Locking Policy", cp.lock_policy));
        targetbag.ownProperty( new Property<bool>("pull","Fetch data over network", cp.pull));
        targetbag.ownProperty( new Property<int>("size","The size of a buffered connection", cp.size));
        targetbag.ownProperty( new Property<int>("transport","The prefered transport. Set to zero if unsure.", cp.transport));
        targetbag.ownProperty( new Property<int>("data_size","A hint about the data size of a single data sample. Set to zero if unsure.", cp.transport));
        targetbag.ownProperty( new Property<string>("name_id","The name of the connection to be formed.",cp.name_id));
    }

}
