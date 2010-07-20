#include "OperationRepositoryPartFused.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::detail;


string OperationRepositoryPartHelper::description(OperationBase* op) {
    return op->getDescriptions().front();
}

vector<interface::ArgumentDescription> OperationRepositoryPartHelper::getArgumentList(OperationBase* op, const int arity, const vector<string>& arg_types ) {
    vector<string> descr = op->getDescriptions();
    vector<interface::ArgumentDescription> ret;
    for ( int i =1; i <= arity; i +=2 ) {
        // resize for undocumented arguments.
        descr.resize( 1 + arity *2 );
        ret.push_back(interface::ArgumentDescription(descr[i],descr[i+1], arg_types[i -1] ));
    }
    return ret;
}
