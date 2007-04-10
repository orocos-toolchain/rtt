#include <iostream>
#include <vector>
#include <string>
#include "../DataSourceBase.hpp"
#include "../OperationInterface.hpp"
#include "../TaskObject.hpp"


using namespace std;

namespace RTT { namespace detail {

void DumpObject(OperationInterface* peer)
{
    if (!peer) { cerr << "Not dumping null task!"<<endl; return;}

    cerr << "Dumping Task "<< peer->getName() << endl;
    cerr << "Attributes: ";
    std::vector<std::string> objlist = peer->attributes()->names();
    for( std::vector<std::string>::iterator it = objlist.begin(); it != objlist.end(); ++it) {
        DataSourceBase::shared_ptr pds = peer->attributes()->getValue(*it)->getDataSource();
        cerr << *it <<"(="<< pds<<") ";
    }
    cerr << endl;

    cerr << "Events: ";
    objlist = peer->events()->getNames();
    for( std::vector<std::string>::iterator it = objlist.begin(); it != objlist.end(); ++it) {
        cerr << *it <<" ";
    }
    cerr << endl;

    objlist = peer->getObjectList();
    cerr << "Objects: ";
    if ( !objlist.empty() ) {
        for(vector<string>::iterator it = objlist.begin(); it != objlist.end(); ++it)
            cerr << *it << " ";
    } else {
        cerr <<"(none)";
    }
    cerr << endl << endl;

    // recurse:
    for(vector<string>::iterator it = objlist.begin(); it != objlist.end(); ++it)
        if ( *it != "this" )
            DumpObject( peer->getObject( *it ) );

}
}}
