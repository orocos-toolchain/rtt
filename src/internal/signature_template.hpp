/***************************************************************************
  tag: FMTC  do nov 2 13:05:58 CET 2006  signature_template.hpp

                        signature_template.hpp -  description
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



// Include the appropriate functionN header
#define OROCOS_SIGNATURE_FUNCTION_N BOOST_JOIN(boost::function,OROCOS_SIGNATURE_NUM_ARGS)
#define OROCOS_SIGNATURE_FUNCTION_N_HEADER BOOST_JOIN(<boost/function/function,OROCOS_SIGNATURE_NUM_ARGS.hpp>)
#include OROCOS_SIGNATURE_FUNCTION_N_HEADER

// Determine if a comma should follow a listing of the arguments/parameters
#if OROCOS_SIGNATURE_NUM_ARGS == 0
#  define OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS
#else
#  define OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS ,
#endif // OROCOS_NUM_ARGS > 0

// Include user specified template.
#include OROCOS_SIGNATURE_TEMPLATE_HEADER

#undef OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS
#undef OROCOS_SIGNATURE_FUNCTION_N_HEADER
#undef OROCOS_SIGNATURE_FUNCTION_N

