/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:55 CEST 2006  RTT.hpp

                        RTT.hpp -  description
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

#include "rtt/rtt-config.h"

/**
 * @file RTT.hpp
 */

/**
 * @brief Contains TaskContext, Activity, OperationCaller, Operation, Property, InputPort, OutputPort, Attribute.
 *
 * The Real-Time Toolkit is documented in <a href="../../orocos-manual.html">
 * The Orocos Real-Time Toolkit Online Manual</a>
 */
namespace RTT {}

#include "rtt/rtt-fwd.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/Activity.hpp"
#include "rtt/OperationCaller.hpp"
#include "rtt/Operation.hpp"
#include "rtt/Property.hpp"
#include "rtt/Attribute.hpp"
#include "rtt/Port.hpp"

#ifndef DOXYGEN
namespace BFL {
}

namespace KDL {
}
#endif

/**
 * The project wide Orocos namespace contains all user classes
 * of various projects. When one writes @code using namespace Orocos;@endcode
 * all classes from the RTT, KDL and BFL namespaces will be visible.
 */
namespace Orocos
{
    using namespace RTT;
    using namespace BFL;
    using namespace KDL;
}
