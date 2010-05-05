#ifndef ORO_GETSIGNATURE_HPP
#define ORO_GETSIGNATURE_HPP

#include "UnMember.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * Returns a function signature from a C or C++ member function pointer type.
         */
        template<class FunctionT>
        struct GetSignature {
            typedef typename internal::UnMember< typename boost::remove_pointer<FunctionT>::type >::type Signature;
        };

        /**
         * Returns a function signature from a C or C++ member function pointer type,
         * suitable for DS operations.
         */
        template<class FunctionT>
        struct GetSignatureDS {
            typedef typename internal::ArgMember< typename boost::remove_pointer<FunctionT>::type >::type Signature;
        };

    }
}


#endif
