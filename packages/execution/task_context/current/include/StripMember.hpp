#ifndef ORO_STRIP_MEMBER_HPP
#define ORO_STRIP_MEMBER_HPP

#include <boost/mpl/erase.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_signature.hpp>


namespace ORO_Execution
{
    namespace detail
    {
        template<class F>
        class StripMember
        {
            typedef function_type_signature<F> member_signature; 
            typedef typename mpl::erase_c<member_signature,1>::type non_member_signature;
        public:
            typedef typename non_member_signature::representee type;
        };
    }
}


#endif
