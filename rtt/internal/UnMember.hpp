/***************************************************************************
  tag: FMTC  do nov 2 13:06:07 CET 2006  UnMember.hpp

                        UnMember.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_UN_MEMBER_HPP
#define ORO_UN_MEMBER_HPP

#include <boost/mpl/erase.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/int.hpp>
#include <boost/type_traits.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/components.hpp>
#include <boost/function_types/member_function_pointer.hpp>

namespace RTT
{
    namespace internal
    {
        /**
         * This class converts a member function type R (X::)(Args) to
         * a plain function type R (Args) which can be used by a boost::function
         * or similar. If you have a type R(X::*)(Args) ( a function \b pointer type),
         * use: @code UnMember< boost::remove_pointer<R(X::*)(Args)>::type>::type @endcode
         */
        template<class F>
        class UnMember
        {
            typedef boost::function_types::components<F> member_signature;
            typedef typename boost::mpl::erase<typename member_signature::types,
                                               typename boost::mpl::next<typename boost::mpl::begin<member_signature>::type>::type>::type non_member_signature;
        public:
            typedef typename boost::function_types::function_type<non_member_signature>::type type;
        };

        /**
         * The inverse of UnMember. This class converts a non-member function type R (Args) to
         * a member function type R (X::*)(Args) which can be used by a boost::function
         * or similar.
         */
        template<class F,class Class>
        class AddMember
        {
            typedef boost::function_types::components<F> non_member_signature;
            typedef typename boost::mpl::insert<non_member_signature,
                                               typename boost::mpl::next<typename boost::mpl::begin<non_member_signature>::type>::type,
                                               Class>::type with_member_signature;
        public:
            typedef typename boost::function_types::member_function_pointer<with_member_signature>::type type;
        };

        /**
         * A complexer variant of UnMember: Convert a member function type to
         * a function type which contains the object as first argument.
         * Thus R (X::*)(Args) becomes R (X::*, Args)
         */
        template<class F>
        class ArgMember
        {
            typedef boost::function_types::components<F> member_signature;
            // TODO this erase/insert cycle can be removed and just use member_signature in function_type<T>
            // remove the class from the arg list:
            typedef typename boost::mpl::erase<typename member_signature::types,
                                               typename boost::mpl::next<typename boost::mpl::begin<member_signature>::type>::type>::type non_member_signature;
            // insert it as first argument.
            typedef typename boost::mpl::insert<non_member_signature,
                                                typename boost::mpl::next<typename boost::mpl::begin<non_member_signature>::type>::type,
                                                typename boost::add_pointer<typename boost::mpl::at<typename member_signature::types,boost::mpl::int_<1> >::type>::type
                                               >::type arg_signature;
        public:
            typedef typename boost::function_types::function_type<arg_signature>::type type;
        };

        /**
         * Convert a function R (X::)(Args) to a plain function signature R(X::,Args)
         */
        template<class F>
        class UnPointer
        {
            typedef boost::function_types::components<F> signature;
        public:
            typedef typename boost::function_types::function_type<signature>::type type;
        };
    }
}


#endif
