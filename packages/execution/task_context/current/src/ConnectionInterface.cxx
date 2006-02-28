
#include "execution/ConnectionInterface.hpp"

namespace ORO_Execution
{

    ConnectionInterface::ConnectionInterface() { atomic_set(&refcount,0); }

    ConnectionInterface::~ConnectionInterface()
    {
    }

}
void intrusive_ptr_add_ref( ORO_Execution::ConnectionInterface* p )
{ 
    atomic_inc(&(p->refcount) ); 
}

void intrusive_ptr_release( ORO_Execution::ConnectionInterface* p )
{
    if ( atomic_dec_and_test(&(p->refcount) ) ) delete p;
}
