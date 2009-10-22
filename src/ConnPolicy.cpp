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
        i = bag.getProperty<int>("type");
        if ( i.ready() )
            result.type = i.get();
        else if ( bag.find("type") ){
            log(Error) <<"ConnPolicy: wrong property type of 'type'."<<endlog();
            return false;
        }
        i = bag.getProperty<int>("lock_policy");
        if ( i.ready() )
            result.lock_policy = i.get();
        else if ( bag.find("lock_policy") ){
            log(Error) <<"ConnPolicy: wrong property type of 'lock_policy'."<<endlog();
            return false;
        }
        i = bag.getProperty<int>("size");
        if ( i.ready() )
            result.size = i.get();
        else if ( bag.find("size") ){
            log(Error) <<"ConnPolicy: wrong property type of 'size'."<<endlog();
            return false;
        }
        i = bag.getProperty<int>("data_size");
        if ( i.ready() )
            result.data_size = i.get();
        else if ( bag.find("data_size") ){
            log(Error) <<"ConnPolicy: wrong property type of 'data_size'."<<endlog();
            return false;
        }
        i = bag.getProperty<int>("transport");
        if ( i.ready() )
            result.transport = i.get();
        else if ( bag.find("transport") ){
            log(Error) <<"ConnPolicy: wrong property type of 'transport'."<<endlog();
            return false;
        }

        b = bag.getProperty<bool>("init");
        if ( b.ready() )
            result.init = b.get();
        else if ( bag.find("init") ){
            log(Error) <<"ConnPolicy: wrong property type of 'init'."<<endlog();
            return false;
        }
        b = bag.getProperty<bool>("pull");
        if ( b.ready() )
            result.pull = b.get();
        else if ( bag.find("pull") ){
            log(Error) <<"ConnPolicy: wrong property type of 'pull'."<<endlog();
            return false;
        }

        s = bag.getProperty<string>("name_id");
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
        log(Debug) <<"Deomposing ConnPolicy..." <<endlog();
        assert( targetbag.empty() );
        targetbag.setType("ConnPolicy");
        targetbag.addProperty( new Property<int>("type","Data type", cp.type));
        targetbag.addProperty( new Property<bool>("init","Initialize flag", cp.init));
        targetbag.addProperty( new Property<int>("lock_policy","Locking Policy", cp.lock_policy));
        targetbag.addProperty( new Property<bool>("pull","Fetch data over network", cp.pull));
        targetbag.addProperty( new Property<int>("size","The size of a buffered connection", cp.size));
        targetbag.addProperty( new Property<int>("transport","The prefered transport. Set to zero if unsure.", cp.transport));
        targetbag.addProperty( new Property<int>("data_size","A hint about the data size of a single data sample. Set to zero if unsure.", cp.transport));
        targetbag.addProperty( new Property<string>("name_id","The name of the connection to be formed.",cp.name_id));
    }

}
