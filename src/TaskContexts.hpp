/***************************************************************************
  tag: FMTC  do nov 2 13:06:05 CET 2006  TaskContexts.hpp

                        TaskContexts.hpp -  description
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



/**
 * @file TaskContexts.hpp
 * The TaskContext class is the fundamental building block for
 * creating Orocos components. This header includes the TaskContext
 * header and the header for each interface primitive.
 */

/**
 * @defgroup RTTComponentInterface Task Context Interface
 * @ingroup CompModel
 * The TaskContext can be 'decorated' with a number of interface classes.
 * Each interface class serves a specific type of communication in
 * a control application. Being it configuration, data communication or
 * execution of functionality.
 */

#include "TaskContext.hpp"
#include "Command.hpp"
#include "Method.hpp"
#include "Property.hpp"
#include "Ports.hpp"
#include "Events.hpp"
