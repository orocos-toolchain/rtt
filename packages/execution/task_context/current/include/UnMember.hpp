#ifndef ORO_UN_MEMBER_HPP
#define ORO_UN_MEMBER_HPP

#include <boost/mpl/erase.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/int.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_signature.hpp>


namespace RTT
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

        template<class F>
        class ArgMember
        {
            typedef boost::function_type_signature<F> member_signature; 
            // remove the class from the arg list:
            typedef typename boost::mpl::erase<typename member_signature::types,
                                               typename boost::mpl::next<typename boost::mpl::begin<member_signature>::type>::type>::type non_member_signature;
            // insert it as first argument.
            typedef typename boost::mpl::insert<non_member_signature,
                                                typename boost::mpl::next<typename boost::mpl::begin<non_member_signature>::type>::type,
                                                typename boost::add_pointer<typename boost::mpl::at<typename member_signature::types,boost::mpl::int_<1> >::type>::type
                                               >::type arg_signature;
            //typedef typename arg_signature::blah ok;
            //typedef typename member_signature::types::blah ok2;
        public:
            typedef typename boost::function_type<boost::plain_function,arg_signature>::type type;
            //typedef typename boost::function_type<boost::plain_function,arg_signature>::type::fail type2;
            //typedef typename non_member_signature::representee type;
        };

        template<class F>
        class UnPointer
        {
            typedef boost::function_type_signature<F> signature; 
        public:
            typedef typename boost::function_type<boost::plain_function,signature>::type type;
            //typedef typename non_member_signature::representee type;
        };
    }
}


#endif
