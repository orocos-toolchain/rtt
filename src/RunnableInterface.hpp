/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RunnableInterface.hpp

                        RunnableInterface.hpp -  description
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

#ifndef CORELIB_RUNNABLE_INTERFACE_HPP
#define CORELIB_RUNNABLE_INTERFACE_HPP

#include "os/RunnableInterface.hpp"
#include "ActivityInterface.hpp"

namespace RTT
{
    /**
     * This class adds
     * Activity specific semantics to OS::RunnableInterface and sits in the RTT
     * namespace. Applications should always use this class
     * instead of the OS version.
     *
     * The getActivity() method is guaranteed to return a valid task
     * pointer during initialize(), step() or loop() and finalize(). This allows
     * the RunnableInterface to query the task's period(icity) and ThreadInterface.
     * Consequently, initialize() knows whether
     * step() or loop() will be called ( depending on ActivityInterface::isPeriodic() ).
     * @see ActivityInterface
     * @ingroup CoreLibActivities
     */
    class RunnableInterface
        : public OS::RunnableInterface
    {
        ActivityInterface* owner_task;
    public:
        RunnableInterface();

        /**
         * Checks if this is still in a task and if so, issues a critical warning.
         */
        ~RunnableInterface();
        /**
         * @brief Query for the task this interface is run in.
         *
         * Zero denotes that no task is present to run
         * it, and hence no detailed information is available.
         *
         * @return The Activity which runs this RunnableInterface.
         */
        inline ActivityInterface* getActivity() const;

        /**
         * @brief Set the task this interface is run in.
         *
         * A Zero means no task is running it.
         *
         * @param task The ActivityInterface running this interface.
         */
        virtual void setActivity( ActivityInterface* task );
    };


    ActivityInterface* RunnableInterface::getActivity() const { return owner_task; }
}

#endif
