#ifndef ORO_UN_MEMBER_HPP
#define ORO_UN_MEMBER_HPP

#include <boost/mpl/erase.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_signature.hpp>


namespace ORO_Execution
{
    namespace detail
    {
        template<class F>
        class UnMember
        {
            typedef boost::function_type_signature<F> member_signature; 
            typedef typename boost::mpl::erase<typename member_signature::types,
                                               typename boost::mpl::next<typename boost::mpl::begin<member_signature>::type>::type>::type non_member_signature;
        public:
            typedef typename boost::function_type<boost::plain_function,non_member_signature>::type type;
            //typedef typename non_member_signature::representee type;
        };
    }
}


#endif
