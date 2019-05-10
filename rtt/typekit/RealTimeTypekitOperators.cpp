/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  RealTimeTypekit.cxx

                        RealTimeTypekit.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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



#include "rtt-typekit-config.h"
#include "RealTimeTypekit.hpp"
#include "../types/Types.hpp"
#include "../types/Operators.hpp"
#include "../types/OperatorTypes.hpp"
#include "../internal/mystd.hpp"
#include "../rtt-fwd.hpp"
#include "../FlowStatus.hpp"
#include "../SendStatus.hpp"
#include "../ConnPolicy.hpp"
#include "../typekit/Types.hpp"
#include <ostream>
#include <sstream>
#ifdef OS_RT_MALLOC
#include "../rt_string.hpp"
#endif

namespace RTT
{
    using namespace std;
    using namespace detail;

#ifndef RTT_NO_STD_TYPES
    template<class T>
    struct get_capacity
        : public std::unary_function<T, int>
    {
        int operator()(T cont ) const
        {
            return cont.capacity();
        }
    };

    template<class T>
    struct get_size
        : public std::unary_function<T, int>
    {
        int operator()(T cont ) const
        {
            return cont.size();
        }
    };

    /** @cond */
    /** Strings concatenation
     */
    template <class T>
    struct string_concatenation : public std::binary_function<const std::string&, T, std::string> {
        std::string operator()(const std::string& s, T t) const {
            std::ostringstream oss(s, std::ios_base::ate);
            oss << std::boolalpha << t;
            return oss.str();
        }
    };
#ifdef OS_RT_MALLOC
    template <class T>
    struct rt_string_concatenation : public std::binary_function<const rt_string&, T, rt_string> {
        rt_string operator()(const rt_string& s, T t) const {
            rt_ostringstream oss(s, std::ios_base::ate);
            oss << std::boolalpha << t;
            return oss.str();
        }
    };
#endif
    /** @endcond */
#endif

    bool RealTimeTypekitPlugin::loadOperators()
    {
        OperatorRepository::shared_ptr oreg = OperatorRepository::Instance();

        // boolean stuff:
        oreg->add( newUnaryOperator( "!", std::logical_not<bool>() ) );
        oreg->add( newBinaryOperator( "&&", std::logical_and<bool>() ) );
        oreg->add( newBinaryOperator( "||", std::logical_or<bool>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<bool>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<bool>() ) );

        // int stuff
        oreg->add( newUnaryOperator( "-", std::negate<int>() ) );
        oreg->add( newUnaryOperator( "+", identity<int>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<int>() ) );
        oreg->add( newBinaryOperator( "/", divides3<int,int,int>() ) ); // use our own divides<> which detects div by zero
        oreg->add( newBinaryOperator( "%", std::modulus<int>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<int>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<int>() ) );
        oreg->add( newBinaryOperator( "<", std::less<int>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<int>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<int>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<int>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<int>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<int>() ) );
#ifndef ORO_EMBEDDED
        // uint stuff
        oreg->add( newUnaryOperator( "+", identity<unsigned int>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<unsigned int>() ) );
        oreg->add( newBinaryOperator( "/", divides3<unsigned int,unsigned int,unsigned int>() ) ); // use our own divides<> which detects div by zero
        oreg->add( newBinaryOperator( "%", std::modulus<unsigned int>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<unsigned int>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<unsigned int>() ) );
        oreg->add( newBinaryOperator( "<", std::less<unsigned int>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<unsigned int>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<unsigned int>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<unsigned int>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<unsigned int>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<unsigned int>() ) );
        // llong stuff
        oreg->add( newUnaryOperator( "-", std::negate<long long>() ) );
        oreg->add( newUnaryOperator( "+", identity<long long>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<long long>() ) );
        oreg->add( newBinaryOperator( "/", divides3<long long,long long,long long>() ) ); // use our own divides<> which detects div by zero
        oreg->add( newBinaryOperator( "%", std::modulus<long long>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<long long>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<long long>() ) );
        oreg->add( newBinaryOperator( "<", std::less<long long>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<long long>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<long long>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<long long>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<long long>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<long long>() ) );
        // ullong stuff
        oreg->add( newUnaryOperator( "+", identity<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "/", divides3<unsigned long long,unsigned long long,unsigned long long>() ) ); // use our own divides<> which detects div by zero
        oreg->add( newBinaryOperator( "%", std::modulus<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "<", std::less<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<unsigned long long>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<unsigned long long>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<unsigned long long>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<unsigned long long>() ) );
#endif
        // double stuff..
        oreg->add( newUnaryOperator( "-", std::negate<double>() ) );
        oreg->add( newUnaryOperator( "+", identity<double>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<double>() ) );
        oreg->add( newBinaryOperator( "/", std::divides<double>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<double>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<double>() ) );
        oreg->add( newBinaryOperator( "<", std::less<double>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<double>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<double>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<double>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<double>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<double>() ) );
#ifndef ORO_EMBEDDED
        // float stuff
        oreg->add( newUnaryOperator( "-", std::negate<float>() ) );
        oreg->add( newUnaryOperator( "+", identity<float>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<float>() ) );
        oreg->add( newBinaryOperator( "/", std::divides<float>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<float>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<float>() ) );
        oreg->add( newBinaryOperator( "<", std::less<float>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<float>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<float>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<float>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<float>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<float>() ) );
#endif
#ifndef RTT_NO_STD_TYPES
        // strings
        // causes memory allocation....
        oreg->add( newBinaryOperator( "+", std::plus<std::string>() ) );
        oreg->add( newBinaryOperator( "+", string_concatenation<int>() ) );
        oreg->add( newBinaryOperator( "+", string_concatenation<unsigned int>() ) );
        oreg->add( newBinaryOperator( "+", string_concatenation<double>() ) );
        oreg->add( newBinaryOperator( "+", string_concatenation<float>() ) );
        oreg->add( newBinaryOperator( "+", string_concatenation<bool>() ) );
        oreg->add( newBinaryOperator( "+", string_concatenation<char>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<const std::string&>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to< const std::string&>() ) );
        oreg->add( newBinaryOperator( "<", std::less<const std::string&>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<const std::string&>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<std::string>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<std::string>() ) );
#endif

#ifdef OS_RT_MALLOC
        oreg->add( newBinaryOperator( "+", std::plus<rt_string>() ) );
        oreg->add( newBinaryOperator( "+", rt_string_concatenation<int>() ) );
        oreg->add( newBinaryOperator( "+", rt_string_concatenation<unsigned int>() ) );
        oreg->add( newBinaryOperator( "+", rt_string_concatenation<double>() ) );
        oreg->add( newBinaryOperator( "+", rt_string_concatenation<float>() ) );
        oreg->add( newBinaryOperator( "+", rt_string_concatenation<bool>() ) );
        oreg->add( newBinaryOperator( "+", rt_string_concatenation<char>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<const rt_string&>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to< const rt_string&>() ) );
        oreg->add( newBinaryOperator( "<", std::less<const rt_string&>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<const rt_string&>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<rt_string>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<rt_string>() ) );
#endif

#ifndef ORO_EMBEDDED
        // chars
        oreg->add( newBinaryOperator( "==", std::equal_to<char>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<char>() ) );
        oreg->add( newBinaryOperator( "<", std::less<char>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<char>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<char>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<char>() ) );
#if 0
        // causes memory allocation....
        oreg->add( newUnaryOperator( "-", std::negate<const std::vector<double>&>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<const std::vector<double>&>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<const std::vector<double>&>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<const std::vector<double>&>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<const std::vector<double>&, double, const std::vector<double>&>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<const std::vector<double>&, const std::vector<double>&, double>() ) );
        oreg->add( newBinaryOperator( "/", divides3<const std::vector<double>&, const std::vector<double>&, double>() ) );
#endif
#endif

        // FlowStatus
        oreg->add( newBinaryOperator( "==", std::equal_to<FlowStatus>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to< FlowStatus>() ) );
        oreg->add( newBinaryOperator( "<", std::less<FlowStatus>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<FlowStatus>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<FlowStatus>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<FlowStatus>() ) );

        // WriteStatus
        oreg->add( newBinaryOperator( "==", std::equal_to<WriteStatus>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to< WriteStatus>() ) );
        oreg->add( newBinaryOperator( "<", std::less<WriteStatus>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<WriteStatus>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<WriteStatus>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<WriteStatus>() ) );

        // SendStatus
        oreg->add( newBinaryOperator( "==", std::equal_to<SendStatus>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to< SendStatus>() ) );
        oreg->add( newBinaryOperator( "<", std::less<SendStatus>() ) );
        oreg->add( newBinaryOperator( ">", std::greater<SendStatus>() ) );
        oreg->add( newBinaryOperator( "<=", std::less_equal<SendStatus>() ) );
        oreg->add( newBinaryOperator( ">=", std::greater_equal<SendStatus>() ) );

        return true;
    }
}
