/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  DataSources.cxx

                        DataSources.cxx -  description
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


#include "rtt/internal/DataSources.hpp"

namespace RTT {
    namespace internal { 

    /**
     * Specialisation for std::string to keep capacity when set( ... ) is called.
     */
    template<>
    void ValueDataSource<std::string>::set( AssignableDataSource<std::string>::param_t t )
    {
        mdata = t.c_str();
    }

    /**
     * Specialisation for std::string to keep capacity when clone() is called.
     */
    template<>
    ValueDataSource<std::string>::ValueDataSource( std::string t )
        : mdata( t.c_str() )
    {
    }
    }
}

/*
 * Explicit template instantiation for core data source types
 */
template class RTT::internal::DataSource< bool >;
template class RTT::internal::AssignableDataSource< bool >;
template class RTT::internal::ValueDataSource< bool >;
template class RTT::internal::ConstantDataSource< bool >;
template class RTT::internal::ReferenceDataSource< bool >;
template class RTT::internal::DataSource< std::string >;
template class RTT::internal::AssignableDataSource< std::string >;
template class RTT::internal::ValueDataSource< std::string >;
template class RTT::internal::ConstantDataSource< std::string >;
template class RTT::internal::ReferenceDataSource< std::string >;
