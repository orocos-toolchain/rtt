/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  Types.hpp

                        Types.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef ORO_TEMPLATE_DATAFLOW_TYPEINFO_HPP
#define ORO_TEMPLATE_DATAFLOW_TYPEINFO_HPP

#include "Types.hpp"
#include "../Logger.hpp"
#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "PrimitiveTypeInfo.hpp"

#include "../rtt-config.h"

namespace RTT
{
    namespace types {

    /**
     */
    template<typename T, bool use_ostream = false>
    class DataFlowTypeInfo
        : public PrimitiveTypeInfo<T, use_ostream>
    {
    public:
        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

        DataFlowTypeInfo(std::string name)
            : PrimitiveTypeInfo<T,use_ostream>(name)
        {
        }

        virtual ~DataFlowTypeInfo()
        {
        }

        base::InputPortInterface*  inputPort(std::string const& name) const { return new InputPort<T>(name); }
        base::OutputPortInterface* outputPort(std::string const& name) const { return new OutputPort<T>(name); }

        base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const {
            return internal::ConnFactory::buildDataStorage<T>(policy);
        }

        base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelOutput(
                    static_cast<RTT::InputPort<T>&>(port), new internal::SimpleConnID());
        }

        base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelInput(
                    static_cast<RTT::OutputPort<T>&>(port), new internal::SimpleConnID(), 0 );
        }
    };
}}

#endif
