
#include "corelib/OperationAcceptor.hpp"
#include "corelib/PropertyOperation.hpp"

namespace ORO_CoreLib
{
    namespace detail
    {
        bool OperationAcceptor::comply( PropertyOperation* op ) const
        { 
            //std::cout <<"******************PropOp called"<<std::endl;
            return false; 
        }

        bool OperationAcceptor::comply( DeepCopyOperation* op ) const
        { 
            op->deny(); 
            return false; 
        }

        bool OperationAcceptor::comply( FillOperation* op ) const
        { 
            op->deny(); 
            return false; 
        }
    }
}
